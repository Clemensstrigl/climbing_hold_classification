# import cv2
# import numpy as np
# import sys

# # Read the image
# image = cv2.imread(sys.argv[1], cv2.IMREAD_GRAYSCALE)

# # Apply Sobel edge detection
# sobelx = cv2.Sobel(image, cv2.CV_64F, 1, 0, ksize=3)
# sobely = cv2.Sobel(image, cv2.CV_64F, 0, 1, ksize=3)

# # Combine the Sobel x and y images
# sobel_combined = cv2.addWeighted(cv2.convertScaleAbs(sobelx), 0.5, cv2.convertScaleAbs(sobely), 0.5, 0)

# # Apply thresholding
# _, binary_image = cv2.threshold(sobel_combined, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)

# # Apply morphological operations
# kernel = np.ones((5, 5), np.uint8)
# binary_image = cv2.morphologyEx(binary_image, cv2.MORPH_CLOSE, kernel)

# # Find contours
# contours, _ = cv2.findContours(binary_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# # Threshold contours based on area
# min_area = 100  # Minimum area to consider a contour
# max_area = 10000  # Maximum area to consider a contour

# filtered_contours = [cnt for cnt in contours if min_area < cv2.contourArea(cnt) < max_area]

# # Create a mask
# mask = np.zeros_like(image)

# # Draw the filtered contours on the mask
# cv2.drawContours(mask, filtered_contours, -1, (255), thickness=cv2.FILLED)

# # Extract pixels inside the closed loops
# result = cv2.bitwise_and(image, mask)

# # Display the result
# cv2.imshow('Result', result)
# cv2.waitKey(0)
# cv2.destroyAllWindows()


import cv2
import numpy as np
import sys
# Read the image
image = cv2.imread(sys.argv[1], cv2.IMREAD_GRAYSCALE)
# Apply Sobel edge detection
sobelx = cv2.Sobel(image, cv2.CV_64F, 1, 0, ksize=3)
sobely = cv2.Sobel(image, cv2.CV_64F, 0, 1, ksize=3)

# Combine the Sobel x and y images
sobel_combined = cv2.addWeighted(cv2.convertScaleAbs(sobelx), 0.5, cv2.convertScaleAbs(sobely), 0.5, 0)

# Apply thresholding
_, binary_image = cv2.threshold(sobel_combined, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)

# Find contours
contours, _ = cv2.findContours(binary_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# Find the contour with the largest area (assuming it's the closed loop)
largest_contour = max(contours, key=cv2.contourArea)

# Create a mask
mask = np.zeros_like(image)
cv2.drawContours(mask, [largest_contour], -1, (255), thickness=cv2.FILLED)

# Extract pixels inside the closed loop
result = cv2.bitwise_and(image, mask)

# Display the result
cv2.imshow('Result', result)
cv2.waitKey(0)
cv2.destroyAllWindows()

