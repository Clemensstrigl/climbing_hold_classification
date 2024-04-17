/** PyImgWriter.hpp
 * 
 *  Module containing the PyImgWriter class declaration
 *  and C functions that wrap its functionalities,
 *  to be used by python ctypes.
 *  
 *  PyImgWriter is responsible to serialize
 *  an OpenCV image (cv::Mat) so it can be consumed
 *  by a python function, that reconstructs it with Python's opencv
 * 
 *  This class is the counterpart of CppImgReader implemented in
 *  Python. PyImgWriter "writes" an image to Python.
 *  CppImgReader "reads" said image.
 * 
 */

#ifndef _PYIMGWRITER_H_
#define _PYIMGWRITER_H_


#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

/** PyImgWriter
 *  class responsible for serializing an OpenCV Mat
 *  into an array of bytes so a Python function can
 *  reconstruct it 
*/
class PyImgWriter
{
    public:
    /* the source image
    */
    cv::Mat box_mat, paths_mat;
    
    /* constructor that receives a pointer to the image
       to be serialized
    */
    // PyImgWriter(cv::Mat* img);
    
    // /* returns an array of bytes containing the image's raw data
    // */
    // unsigned char* getImg();
    
    // /* returns an array with the sizes
    //    of each of the image's dimensions (example: h, w, depth)
    // */
    // unsigned int* getShape();
    
    // /* returns the image's opencv Mat type constant,
    //    such as cv::CV_8UC3
    // */
    // int getType();
    
    // /* returns the image's number of dimensions
    //    a grayscale image has 2 dimensions,
    //    an RGB has 3, for example
    // */
    // int getDims();

    PyImgWriter(cv::Mat box_mat, cv::Mat paths_mat)
    {
        //copies the content of img to the object, for memory safety
        this->box_mat = box_mat.clone();
        this->paths_mat = paths_mat.clone();

        
    }


    unsigned char*  getBoxMat()
    {
        return this->box_mat.data;
    }


    unsigned int*  getBoxShape()
    {
        unsigned int* result = (unsigned int*)malloc(3 * sizeof(unsigned int));

        result[0] = this->box_mat.rows;
        result[1] = this->box_mat.cols;
        result[2] = this->box_mat.channels();

        return result;
    }


    int  getBoxType()
    {
        return this->box_mat.type();
    }


    int  getBoxDims()
    {
        return (this->box_mat.dims
                + ( (this->box_mat.channels() > 1) ? 1 : 0 )
            );
    }
    
    unsigned char*  getPathMat()
    {
        return this->paths_mat.data;
    }


    unsigned int*  getPathShape()
    {
        unsigned int* result = (unsigned int*)malloc(3 * sizeof(unsigned int));

        result[0] = this->paths_mat.rows;
        result[1] = this->paths_mat.cols;
        result[2] = this->paths_mat.channels();

        return result;
    }


    int  getPathType()
    {
        return this->paths_mat.type();
    }


    int  getPathDims()
    {
        return (this->paths_mat.dims
                + ( (this->paths_mat.channels() > 1) ? 1 : 0 )
            );
    }
};

/** Wrappers for PyImgWriter's methods, to be called by python,
 *  through ctypes package. ctypes only interacts with C functions.
*/
extern "C"
{
    /** Wrapper that creates and returns a PyImgWriter
     *  same arguments as PyImgWriter's constructor
    */
    PyImgWriter* PyImgW_new(cv::Mat boxMat, cv::Mat pathMat)
    {
        return new PyImgWriter(boxMat,pathMat);
    }
    
    
    /** Wrapper that returns a PyImgWriter's raw image data
    */
    unsigned char* PyImgW_getBoxMat(PyImgWriter* obj)
    {
        return obj->getBoxMat();
    }

    unsigned char* PyImgW_getPathMat(PyImgWriter* obj)
    {
        return obj->getPathMat();
    }


    /** Wrapper that returns a PyImgWriter shape array
    */
    unsigned int* PyImgW_getBoxShape(PyImgWriter* obj)
    {
        return obj->getBoxShape();
    }
    unsigned int* PyImgW_getPathShape(PyImgWriter* obj)
    {
        return obj->getPathShape();
    }


    /** Wrapper that returns a PyImgWriter OpenCV type constant
    */
    int PyImgW_getBoxType(PyImgWriter* obj)
    {
        return obj->getBoxType();
    }
    int PyImgW_getPathType(PyImgWriter* obj)
    {
        return obj->getPathType();
    }
    
    
    /** Wrapper that returns a PyImgWriter's number of dimensions
    */
    int PyImgW_getBoxDims(PyImgWriter* obj)
    {
        return obj->getBoxDims();
    }
    int PyImgW_getPathDims(PyImgWriter* obj)
    {
        return obj->getPathDims();
    }

    /** Wrapper that deletes a PyImgWriter
    */
    void PyImgW_delete(PyImgWriter* obj)
    {
        delete obj;
    }
}


#endif