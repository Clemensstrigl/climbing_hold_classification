
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <sstream>
#include <string>
#include <iostream>
#include <vector>


using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 2) {
    cout << "Usage: display_Image ImageToLoadandDisplay" << endl;
    return -1;
}else{
    Mat image;
    Mat grayImage;

    image = imread(argv[1], IMREAD_COLOR);
    if (!image.data) {
        cout << "Could not open the image file" << endl;
        return -1;
    }
    else {
        int height = image.rows;
        int width = image.cols;
        resize(image,image,Size(image.cols/4, image.rows/4));
        //namedWindow("Display Image", WINDOW_AUTOSIZE);
        cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

        namedWindow("Gray Image", WINDOW_AUTOSIZE);
        imshow("Gray Image", grayImage);

    cv::    Mat image_blurred_with_3x3_kernel;

    //Blur the image with 5x5 Gaussian kernel
    cv::Mat image_blurred_with_5x5_kernel;
    //GaussianBlur(grayImage, grayImage, Size(5, 5), 0);
    medianBlur(grayImage, image_blurred_with_5x5_kernel, 11);
    String window_name_blurred_with_5x5_kernel = "Lotus Blurred with 5 x 5 Gaussian Kernel";
    namedWindow(window_name_blurred_with_5x5_kernel);

    // Show our images inside the created windows.
    imshow(window_name_blurred_with_5x5_kernel, image_blurred_with_5x5_kernel);
    
    /*
    vector<Vec4i> lines;
    vector<vector<Point> > contours0;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    cv::Mat erodeElement = getStructuringElement( MORPH_RECT,Size(10,10));
    cv::Mat dilateElement = getStructuringElement( MORPH_RECT,Size(10,10));


    erode(grayImage,grayImage,erodeElement);
    dilate(grayImage,grayImage,dilateElement);
    Canny(grayImage, image_blurred_with_5x5_kernel, 150,300, 3);


    findContours( image_blurred_with_5x5_kernel , contours0, hierarchy,
        cv::RETR_CCOMP  , cv::CHAIN_APPROX_SIMPLE );

    contours.resize(contours0.size());

    contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ ){
         approxPolyDP(Mat(contours0[k]), contours[k], 5, true);
    }

    int idx = 0;
    for( ; idx >= 0; idx = hierarchy[idx][0] )
    {
        drawContours( image, contours, idx, Scalar(128,255,255), 5, 8, hierarchy );
    }
    imshow("image", image);
    imshow("grey", grayImage);
    imshow("edges", image_blurred_with_5x5_kernel);
    
    */




    
    cv::Mat gX, gY ,edgeDetection;
    edgeDetection = image_blurred_with_5x5_kernel;
    //  compute gradients along the X and Y axis, respectively
    cv::Sobel(edgeDetection, gX, CV_64F, 1, 0);
    cv::Sobel(edgeDetection, gY, CV_64F, 0, 1);
    // gX value after sobel conversion -52.0
    cout << "gX value after sobel conversion: " << (int)gX.at<double>(100, 200) << endl;


    // gX and gY are decimal number with +/- values
    // change these values to +ve integer format
    cv::convertScaleAbs(gX, gX);
    // gX value after Absolute scaling 52
    cv::convertScaleAbs(gY, gY);
    cout << "gX value after Absolute scaling: " << (int)gX.at<uchar>(100, 200) << endl;


    cv::Mat sobelCombined;
     cv::addWeighted(gX, 0.5, gY, 0.5, 0, sobelCombined);
    cv::imshow("Sobel", sobelCombined);


    
    
    
    
    int threshold_value = 1;
    int threshold_type = 1;
    int const max_value = 200;
    int const max_type = 4;
    int const max_binary_value = 200;    

   // createTrackbar( trackbar_type, window_name, &threshold_type, max_type, Threshold_Demo ); // Create a Trackbar to choose type of Threshold
   // createTrackbar( trackbar_value, window_name, &threshold_value, max_value, Threshold_Demo ); // Create a Trackbar to choose Threshold value
    
    cv::threshold( sobelCombined, sobelCombined, threshold_value, max_binary_value, threshold_type );







   // medianBlur(sobelCombined, sobelCombined, 7);


    // show the output images
    //cv::imshow("Sobel X", gX);
    //cv::imshow("Sobel Y", gY);
    cv::imshow("Sobel Combined", sobelCombined);


    
        cv::waitKey(0);
        // image.release();
        // grayImage.release();
        return 0;
    }

  }

}