import sys

sys.path.insert(0, '/home/clemens/.local/lib/python3.13/site-packages')


import cv2 as cv
import numpy as np
import os

def main():
    alpha = 10

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
            mask = cv.rectangle(blank, start_point, end_point, 255, -1)
            #fix masking such that we can only the actual rectangle at the end when saving it to disk
            masked = cv.bitwise_and(origional_image, origional_image, mask=mask)

            mask = cv.rectangle(blank, start_point, end_point, 255, -1)

            display_image = cv.rectangle(display_image, start_point, end_point, (0,0,255), 1)



            numpy_horizontal = np.hstack((display_image, masked))

            
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
            

            

            cv.imwrite(str(file_index) + ".png", masked)
            l = open(os.path.join(label_data_directory , (str(file_index) + ".txt")), "w")
            l.write(str(label))
            l.close()
            file_index = file_index + 1

            



if __name__ == '__main__':
    main()