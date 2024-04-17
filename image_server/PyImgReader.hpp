/** PyImgReader.hpp
 * 
 *  Module containing the PyImgReader class declaration
 *  and C functions that wrap its functionalities,
 *  to be used by python ctypes.
 *  
 *  PyImgReader is responsible to receive data from a python call
 *  and translate it into an OpenCV image (cv::Mat)
 * 
 *  This class is the counterpart of CppImgWriter implemented in
 *  Python. CppImageWriter "writes" an image to C++.
 *  PyImgReader "reads" said image.
 * 
 */

#ifndef _PYIMGREADER_H_
#define _PYIMGREADER_H_


#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

/** PyImgReader
 *  a class that translate an array of bytes into an OpenCV Mat
*/
class PyImgReader
{
    public:
    /*the resulting image
    */
    cv::Mat img;
    cv::Mat depth_img;
    cv::Mat hold_loc;
    cv::Mat human_loc;
    

    /* contructor that receiver a pointer to data and makes
       it into an image.

       void* data is a pointer to the future image data
       int h is the desired image height
       int w is the desired image width
       int type is the pixel's type. One of OpenCV's
                constants, such as cv::CV_8UC3
    */
    PyImgReader(void* image_data_arr, int image_h, int image_w, int image_data_type, 
                            void* depth_image_data_arr, int depth_data_type, 
                            void* hold_loc_arr, int hold_arr_h, int hold_arr_w, int hold_arr_type,
                            void* human_loc_arr, int human_arr_h, int human_arr_w, int human_arr_type)
        {

            cv::Mat img(image_h, image_w, image_data_type, image_data_arr);
            this->img = img.clone();
            cv::Mat depth_img(image_h, image_w, depth_data_type, depth_image_data_arr);
            this->depth_img = depth_img.clone();
            cv::Mat hold_loc_mat(hold_arr_h, hold_arr_w, hold_arr_type, hold_loc_arr);
            this->hold_loc = hold_loc_mat.clone();
            cv::Mat human_loc_mat(human_arr_h, human_arr_w, human_arr_type, human_loc_arr);
            this->human_loc = human_loc_mat.clone();
            
        }
    /*Function that returns a pointer to the object's image
    */
    cv::Mat*  getImg()
    {
        return &(this->img);
    }

    cv::Mat*  getDepthImg()
    {
        return &(this->depth_img);
    }

    cv::Mat*  getHoldLoc()
    {
        return &(this->hold_loc);
    }

    cv::Mat*  getHumanLoc()
    {   
        return &(this->human_loc);
    }

    // private:

    // void convertHoldMat2Vec(cv::Mat* hold_mat);
    // void convertHumanMat2Vec(cv::Mat* human_mat);
    
};


/** Wrappers for PyImgReader's methods, to be called by python,
 *  through ctypes package. ctypes only interacts with C functions.
*/
extern "C"
{
    /** Wrapper that returns a new PyImgReader
     *  Arguments are the same as in its constructor
     * 
    */
    PyImgReader* PyImgR_new(void* image_data_arr, int image_h, int image_w, int image_data_type, 
                            void* depth_image_data_arr, int depth_data_type, 
                            void* hold_loc_arr, int hold_arr_h, int hold_arr_w, int hold_arr_type,
                            void* human_loc_arr, int human_arr_h, int human_arr_w, int human_arr_type)
    {
        return new PyImgReader( image_data_arr,  image_h,  image_w,  image_data_type, 
                             depth_image_data_arr,  depth_data_type, 
                             hold_loc_arr,  hold_arr_h,  hold_arr_w,  hold_arr_type,
                             human_loc_arr,  human_arr_h,  human_arr_w, human_arr_type);
    }

    /** Wrapper that receives a PyImgReader (obj)
     *  and returns a pointer to its image
    */
    

    cv::Mat* PyImgR_getImg(PyImgReader* obj)
    {
        return obj->getImg();
    }

    cv::Mat* PyImgR_getdepthImg(PyImgReader* obj)
    {
        return obj->getDepthImg();
    }

    cv::Mat* PyImgR_getHoldLoc(PyImgReader* obj)
    {
        return obj->getHoldLoc();
    }

    cv::Mat* PyImgR_getHumanLoc(PyImgReader* obj)
    {
        return obj->getHumanLoc();
    }

    /** Wrapper that deletes a PyImgReader
    */
    void PyImgR_delete(PyImgReader* obj)
    {
        delete obj;
    }
}

#endif
