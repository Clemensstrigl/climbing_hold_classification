import sys

sys.path.insert(0, '/home/clemens/.local/lib/python3.13/site-packages')
sys.path.insert(0, '/home/clemens/.local/lib/python3.10/site-packages')

import cv2 as cv
import numpy as np
import os

import math

def LAB_euclidean_distance(a, b):
    eps = 1e-5

    # calculate ci, hi, i=1,2
    c1 = math.sqrt(a[1]**2 + a[2]**2)
    c2 = math.sqrt(b[1]**2 + b[2]**2)
    meanC = (c1 + c2) / 2.0
    meanC7 = meanC ** 7

    g = 0.5 * (1 - (meanC7 / (meanC7 + 6103515625.)))  # 0.5 * (1 - sqrt(meanC^7 / (meanC^7 + 25^7)))
    a1p = a[1] * (1 + g)
    a2p = b[1] * (1 + g)

    c1 = math.sqrt(a1p**2 + a[2]**2)
    c2 = math.sqrt(a2p**2 + b[2]**2)
    h1 = math.fmod(math.atan2(a[2], a1p) + 2 * math.pi, 2 * math.pi)
    h2 = math.fmod(math.atan2(b[2], a2p) + 2 * math.pi, 2 * math.pi)

    # compute deltaL, deltaC, deltaH
    deltaL = b[0] - a[0]
    deltaC = c2 - c1  
    deltah = 0

    if c1 * c2 > eps:
        if abs(h2 - h1) <= math.pi:
            deltah = h2 - h1
        elif h2 > h1:
            deltah = h2 - h1 - 2 * math.pi
        else:
            deltah = h2 - h1 + 2 * math.pi

    deltaH = 2 * math.sqrt(c1 * c2) * math.sin(deltah / 2)

    # calculate CIEDE2000
    meanL = (a[0] + b[0]) / 2
    meanC = (c1 + c2) / 2.0
    meanC7 = meanC ** 7

    if c1 * c2 > eps:
        if abs(h1 - h2) <= math.pi + eps:
            meanH = (h1 + h2) / 2
        elif h1 + h2 < 2 * math.pi:
            meanH = (h1 + h2 + 2 * math.pi) / 2
        else:
            meanH = (h1 + h2 - 2 * math.pi) / 2

    T = 1 - 0.17 * math.cos(meanH - math.radians(30)) + 0.24 * math.cos(2 * meanH) + \
        0.32 * math.cos(3 * meanH + math.radians(6)) - 0.20 * math.cos(4 * meanH - math.radians(63))
    sl = 1 + (0.015 * (meanL - 50) ** 2) / math.sqrt(20 + (meanL - 50) ** 2)
    sc = 1 + 0.045 * meanC
    sh = 1 + 0.015 * meanC * T
    rc = 2 * math.sqrt(meanC7 / (meanC7 + 6103515625.))
    rt = -math.sin(math.radians(60 * math.exp(-((math.degrees(meanH) - 275) / 25) ** 2))) * rc

    finalResult = math.sqrt((deltaL / sl) ** 2 + (deltaC / sc) ** 2 + (deltaH / sh) ** 2 + rt * (deltaC / sc) * (deltaH / sh))

    return finalResult


def calculate_color_average(origional_image, origional_image_blurred, x1, y1, x2, y2):
    color_average = np.array([0, 0, 0])
    count = 0

    for i in range(y1,y2):
        for j in range(x2 - x1):
            orig_pixel = origional_image_blurred[y1 + i, x1 + j]
            roi_pixel = origional_image[y1 + i, x1 + j]

            diff = LAB_euclidean_distance(np.float32(orig_pixel) , np.float32(roi_pixel))
            if diff > 8.3:  # Threshold for significant color difference
                color_average += roi_pixel
                count += 1

    if count > 0:
        color_average //= count
    
    color_average_rgb = cv.cvtColor(np.float32([[color_average]]), cv.COLOR_LAB2BGR)[0][0]


    return color_average_rgb

def blurr_image(img):
    max_img_dim = max(img.shape[:2])
    ratio = max_img_dim / 800

    kernel_size = int(16 * ratio)
    kernel_size = kernel_size + 1 if kernel_size % 2 == 0 else kernel_size
    kernel_size_decr = int(1.5 * ratio)
    kernel_size_decr = kernel_size_decr if kernel_size_decr % 2 == 0 else kernel_size_decr + 1

    wall_colors = cv.medianBlur(img, kernel_size)
    for i in range(4):
        wall_colors = cv.medianBlur(wall_colors, kernel_size - (i * kernel_size_decr))


def main():
    alpha = 20

    work_dir = os.path.dirname(os.path.abspath(__file__)) + "/"
    data_directory       = work_dir + 'labeld_data/'
    image_data_directory = work_dir + 'labeld_data/images/'
    label_data_directory = work_dir + 'labeld_data/labels/'
                            
    if not os.path.isdir(data_directory):
        os.mkdir(data_directory)
        os.mkdir(image_data_directory)
        os.mkdir(label_data_directory)
        
    image_dir = os.listdir(image_data_directory)
    label_dir = os.listdir(label_data_directory)

    file_index = len(image_dir)
    



    origional_data_dir = work_dir + sys.argv[1]
    origional_image_dir = os.path.join(origional_data_dir , 'images/' )
    origional_label_dir = os.path.join(origional_data_dir , 'labels/' )

    origional_images = os.listdir(origional_image_dir)
    print(origional_label_dir)


    os.chdir(image_data_directory) 
    for origional_image_name in origional_images:
        origional_file_name = origional_image_name[:len(origional_image_name) -4] + ".txt"
        origional_rect = open(os.path.join(origional_label_dir, origional_file_name))
        origional_image = cv.imread(os.path.join(origional_image_dir, origional_image_name))
        h, w, _ = origional_image.shape
        origional_image_blurred = blurr_image(origional_image)

        for line in origional_rect:
            display_image = origional_image.copy()
            label, rx, ry, rh, rw = line.replace("/n","").split(' ')

            label = int(label)
            rx = int(w * float(rx))
            ry = int(h * float(ry))
            rh = int(h * float(rh)) + alpha
            rw = int(w * float(rw)) + alpha

            x1 = rx - int(rw/2)
            x2 = rx + int(rw/2)
            y1 = ry - int(rh/2)
            y2 = ry + int(rh/2)

            
            blank = np.zeros(origional_image.shape[:2], dtype='uint8')
            start_point = (x1, y1) 
            end_point = (x2, y2)
            roi = display_image[y1:y2, x1:x2]
            mask = cv.rectangle(blank, start_point, end_point, 255, -1)
            #fix masking such that we can only the actual rectangle at the end when saving it to disk
            masked = cv.bitwise_and(origional_image, origional_image, mask=mask)

            # Parse out the ROI from the original image
            

            # Save the ROI as a new image
            display_image = cv.rectangle(display_image, start_point, end_point, (0,0,255), 1)
            


            numpy_horizontal = np.hstack((display_image, masked))

            font = cv.FONT_HERSHEY_SIMPLEX
            bottomLeftCornerOfText = (10, numpy_horizontal.shape[0] - 10)  # Adjust position as needed
            fontScale = 0.5
            fontColor = (0, 0, 0)  # White color
            lineType = 1
            text = "Crimp: c, Juge: j, Sloper: s, Pinch: p"
            cv.putText(numpy_horizontal, text, bottomLeftCornerOfText, font, fontScale, fontColor, lineType)


            
            if label == 0:
                cv.imshow("Hold", numpy_horizontal)
                while True:
                    c = cv.waitKey(0)
                    if c in [106, 99, 112, 115, 27]:
                        break
                if c == 106:
                    label = 1
                elif c == 99:
                    label = 2    
                elif c == 112:
                    label = 3 
                elif c == 115:
                    label = 4
                elif c == 27:
                    origional_rect.close()
                    exit()

            else:
                label = 5
            

            

            cv.imwrite(str(file_index) + ".png", roi)
            l = open(os.path.join(label_data_directory , (str(file_index) + ".txt")), "w")
            l.write(str(label))
            l.close()
            file_index = file_index + 1

            



if __name__ == '__main__':
    main()
