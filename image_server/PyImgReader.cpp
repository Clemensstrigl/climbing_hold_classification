/**
build:  g++ -shared -fPIC -o libPyImageReader.so PyImgReader.cpp -lopencv_core -lopencv_imgcodecs -std=c++11
*/

#include "PyImgReader.hpp"
#include <vector>


PyImgReader::PyImgReader(void* image_data_arr, int image_h, int image_w, int image_data_type, 
                            void* depth_image_data_arr, int depth_data_type, 
                            void* hold_loc_arr, int hold_arr_h, int hold_arr_w, int hold_arr_type,
                            void* human_loc_arr, int human_arr_h, int human_arr_w, int human_arr_type)
{

    cv::Mat img(image_h, image_w, image_data_type, image_data_arr);
    this->img = img.clone();
    cv::Mat depth_img(image_h, image_w, depth_data_type, depth_image_data_arr);
    this->depth_img = depth_img.clone();
    cv::Mat hold_loc_mat(hold_arr_h, hold_arr_w, hold_arr_type, hold_loc_arr);
    this->depth_img = hold_loc_mat.clone();
    cv::Mat human_loc_mat(human_arr_h, human_arr_w, human_arr_type, human_loc_arr);
    this->depth_img = human_loc_mat.clone();
    
}

// void PyImgReader::convertHoldMat2Vec(cv::Mat* hold_mat)
// {
//     if (hold_mat.isContinuous()) {
//         this->hold_loc.assign((float*)hold_mat.datastart, (float*)hold_mat.dataend);
//     } else {
//         for (int i = 0; i < hold_mat.rows; ++i) {
//             float* row = hold_mat.ptr<float>(i);
//             this->hold_loc.insert(this->hold_loc.end(), row, row + hold_mat.cols);
//         }
//     }
// }

// void PyImgReader::convertHumanMat2Vec(cv::Mat* human_mat)
// {

//     if (human_mat.empty()) {
//         return;
//     }
    
//     if (human_mat.isContinuous()) {
//         this->human_loc.assign((float*)human_mat.datastart, (float*)human_mat.dataend);
//     } else {
//         for (int i = 0; i < human_mat.rows; ++i) {
//             float* row = human_mat.ptr<float>(i);
//             this->human_loc.insert(this->human_loc.end(), row, row + human_mat.cols);
//         }
//     }


// }


cv::Mat* PyImgReader::getImg()
{
    return &(this->img);
}

cv::Mat* PyImgReader::getDepthImg()
{
    return &(this->depth_img);
}

cv::Mat* PyImgReader::getHoldLoc()
{
    return &(this->hold_loc);
}

cv::Mat* PyImgReader::getHumanLoc()
{   
    return &(this->human_loc);
}
