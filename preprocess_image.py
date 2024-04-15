import sys
import cv2
import numpy as np



def estimate_illumination(image, sigma):
    # Convert image to float32 for accurate calculations
    image_float = image.astype(np.float32)

    # Initialize output illumination component
    illumination_component = np.zeros_like(image_float)

    # Iterate over different scales
    for s in sigma:
        # Generate Gaussian kernel
        kernel_size = int(6 * s) + 1  # Adjust kernel size based on scale
        kernel = cv2.getGaussianKernel(kernel_size, s)

        # Convolve image with Gaussian kernel
        illumination = cv2.filter2D(image_float, -1, kernel)

        # Add the weighted illumination component
        illumination_component += illumination / len(sigma)

    return illumination_component

def enhance_brightness(value, illumination_component, alpha):
    # Calculate adjustment coefficient k

    avg_saturation = np.mean(value)
    k = alpha * avg_saturation

    # Apply adaptive brightness correction
    enhanced_image = value * (255 + k) / (np.maximum(value, illumination_component) + k)

    # Clip pixel values to ensure they stay within valid intensity range (0-255)
    enhanced_image = np.clip(enhanced_image, 0, 255)

    return enhanced_image.astype(np.uint8)




def preprocess_image(image, alpha=0.9):

    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    (hue, sat, value) = cv2.split(hsv)

    sigma = [15, 80, 250]

    illumination_component = estimate_illumination(value,sigma)

    enhanced_image = enhance_brightness(value, illumination_component, alpha)

    image_merge = cv2.merge([hue, sat, enhanced_image]) 

    bgr_img = cv2.cvtColor(image_merge, cv2.COLOR_HSV2BGR)

    return bgr_img


