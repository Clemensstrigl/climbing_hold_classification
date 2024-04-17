    #include <opencv2/opencv.hpp>
    #include <opencv2/imgproc/imgproc.hpp>
    #include <stdio.h>
    #include "opencv2/highgui/highgui.hpp"
    #include "opencv2/imgproc/imgproc.hpp"
    #include <opencv2/dnn.hpp>
    #include <opencv2/dnn/all_layers.hpp>
    #include <sstream>
    #include <string>
    #include <iostream>
    #include <vector>
    #include <zmq_addon.hpp>
    #include <zmq.hpp>
    #include <cmath>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <zmq.h>
    #include <tuple>
    #include <filesystem>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <omp.h>
    #include <pthread.h>
    #include "PyImgWriter.hpp"
   // #include "PyImgWriter.cpp"
    #include "PyImgReader.hpp"
   // #include "PyImgReader.cpp"
    #include <unordered_map>

extern "C"
{

    namespace fs = std::filesystem;

    #define TCP_LOCALHOST "tcp://127.0.0.1:"
    #define BOX_PARAMS_COUNT 4
    #define OPENMP_THREAD_COUNT  8



    //dbscan for klustering nodes
    //or HDBscan or optics

    //for stereo depth map
    //https://docs.opencv.org/4.x/dd/d53/tutorial_py_depthmap.html



    // //Graphic Arts:
    // static const double K1 = 0.045;
    // static const double K2 = 0.015;
    // static const double Kl = 1.0;


    //Textiles:
    static const double K1 = 0.048;
    static const double K2 = 0.014;
    static const double Kl = 2.0;

    // static const double K1 = 0.048;
    // static const double K2 = 0.014;
    // static const double Kl = 3.0;

    #define SQR(x) ((x)*(x))
    #define POW2(x) SQR(x)
    #define POW3(x) ((x)*(x)*(x))
    #define POW4(x) (POW2(x)*POW2(x))
    #define POW5(x) (POW3(x)*POW2(x))
    #define POW6(x) (POW3(x)*POW3(x))
    #define POW7(x) (POW3(x)*POW3(x)*(x))
    #define DegToRad(x) ((x)*M_PI/180)
    #define RadToDeg(x) ((x)/M_PI*180)

    #define COLOR_DISTANCE_THRESbox_t 8.3
    #define CIRCLE_RADIUS_RATIO 0.01
    #define PATH_THICKNESS_RATIO 0.005
    #define COLOR_SATURATION_MIN 19
    #define COLOR_SATURATION_BLACK_MAX 10
    #define AVERAGE_HUMAN_HEIGHT 167
    #define HOLD_MAT_OUTPUT_ATTRIBUTE_COUNT 14
    #define PATH_MAT_OUTPUT_ATTRIBUTE_COUNT 5

    #define NOISE -1
    #define UNDEFINED 0
    void draw_circle(cv::Mat &img,   std::vector<struct box_t*> boxes);
    double point_dist(const cv::Mat &depth_img,struct box_t* p_1, struct box_t* p_2);
    std::vector<int> RangeQuery(  std::vector<struct box_t*> boxs, struct box_t *box, const cv::Mat &depth_img, double eps_xy, double eps_lab, int id, int *final_count);
    std::vector<int> set_union(std::vector<int> set1,  std::vector<int> set2);
    void dbscan(std::vector<struct box_t*> boxs, const cv::Mat &depth_img, double eps_xy, double eps_lab, int minPts, int *C_rtn);
    void convert_Box_color_RGB_to_LAB(std::vector<struct box_t*> box_loc_color, bool col_type);
    void convert_Box_color_LAB_to_RGB(std::vector<struct box_t*> box_loc_color, bool col_type);
    double GS_CIE2Hue( double a, double b );
    void lab2lch(struct color_t* lab, struct color_t* lch);
    void lch2lab(struct color_t* lch, struct color_t* lab);
    int partition(struct box_t** box_loc_color, int start, int end);
    void quickSort(struct box_t** box_loc_color, int start, int end);
    //void* threaded_quickSort(void * data);
    void increase_box_saturation(std::vector<struct box_t*> box_loc_color);
    void  avg_color_based_labels(std::vector<struct box_t*> &box_loc_color);
    struct color_t** convertImgRGBtoLAB(const cv::Mat &img, double *avg_l);
    void parseBoxes(std::vector<char> array, int Topic_len, int* depth_h);
    void exit_program();
    int init_sockets(std::string &port_out);
    std::vector<char> send_Image(const cv::Mat &img, int );
    void const print_box_results( std::vector<float*> boxes);
    void resize_img(cv::Mat& img,cv::Mat& dst, int max_height);
    double modifyRGBValue(double p);
    void rgb2lab(struct color_t* RGB, struct color_t* LAB);
    void lab2rgb(struct color_t* LAB, struct color_t* RGB);
    double LAB_euclidean_distance( struct color_t* a, struct color_t* b);
    bool not_similar_colors(struct color_t* a, struct color_t* b);
    struct box_t* get_avrg_box_t_color(const cv::Mat &img, struct color_t** img_lab_colors, struct color_t** wall_lab_colors, float*loc);
    std::vector<struct box_t*> get_box_colors(const cv::Mat &img, struct color_t** img_lab_colors, struct color_t** wall_colors,  cv::Mat boxes_loc);
    PyImgWriter* convert2PyWirterObj(std::vector<struct box_t*> boxes);




    struct color_t{
        double c1 = 0;
        double c2 = 0;
        double c3 = 0;

        std::string toString() const {
            return "color_t (" + std::to_string((int)this->c1) +
                "," + std::to_string((int)this->c2) +
                "," + std::to_string((int)this->c3) + ")";
        }
    };

    struct human_t {
        int id = -1;
        double x1 = -1;
        double y1 = -1;
        double x2 = -1;
        double y2 = -1;
        double height = -1;
        double width = -1;
        double depth = -1;
        double ratio = -1;
        double size_in_pxls = -1;
        double depth_x = -1;
        double depth_y = -1;

        std::string toString() const {
            return "human_t {id:" + std::to_string(this->id) +
                ",x1:" + std::to_string(this->x1) +
                ",y1:" + std::to_string(this->y1) +
                ",x2:" + std::to_string(this->x2) +
                ",y2:" + std::to_string(this->y2) +
                ",height:" + std::to_string(this->height) +
                ",width:" + std::to_string(this->width) +
                ",depth:" + std::to_string(this->depth) +
                ",ratio:" + std::to_string(this->ratio) +
                ",size_in_pxls:" + std::to_string(this->size_in_pxls) +
                ",depth_x:" + std::to_string(this->depth_x) +
                ",depth_y:" + std::to_string(this->depth_y) + "}";
        }
        
    };


    struct path_point_t {
        int id = -1;
        double x = -1;
        double y = -1;
        int from = -1;
        int to = -1;


        std::string toString() const {
            return "path_point_t (" + std::to_string((int)this->id) +
                "," + std::to_string((int)this->x) +
                "," + std::to_string((int)this->y) +
                "," + std::to_string((int)this->from) +
                "," + std::to_string((int)this->to) + ")";
        }

        int* py_Out(){

            int *arr = new int[PATH_MAT_OUTPUT_ATTRIBUTE_COUNT];
            arr[0] = this->id;
            arr[1] = (int)this->x;
            arr[2] = (int)this->y;
            arr[3] = this->from;
            arr[4] = this->to;
            
            return arr;
        }
    };


    struct cluster_t{
        int id = -1;
        int count = 0;
        double x1 = -1;
        double y1 = -1;
        double x2 = -1;
        double y2 = -1;
        
        std::vector<struct path_point_t*> path;   

        ~cluster_t(){
            for (int i = 0; i < (int)this->path.size();i++){
                delete this->path[i];
            }
        }

        std::string toString(bool close) const {
            std::string result = "cluster_t {" + std::to_string((int)this->id) +
                                ";" + std::to_string((int)this->count) +
                                ";" + std::to_string((int)this->x1) +
                                ";" + std::to_string((int)this->y1) +
                                ";" + std::to_string((int)this->x2) +
                                ";" + std::to_string((int)this->y2) +
                                (close? "}":";");
            return result;
        }

        std::string path_toString() const {
            std::string result = "path_point_list [";
            for (const auto& point : this->path) {
                result += point->toString() + ",";
            }
            // if (!path.empty()) {
            //     result.pop_back();  // remove trailing comma
            //    result.pop_back();  // remove space
            // }
            //result.pop_back();
            result += "]";
            return result;
        }


        
    };


    



    struct box_t{
        int id = -1;
        int label = -1;
        double x = -1;
        double y = -1; //4
        struct color_t *color_RGB; 
        struct color_t *color_LAB;
        struct color_t *cluster_color_RGB; //7
        struct color_t *cluster_color_LAB;
        double rad_size = -1; //8
        struct cluster_t *cluster_conf; //14

        box_t(){
            this->color_RGB = nullptr;
            this->color_LAB = nullptr;
            this->cluster_color_RGB = nullptr;
            this->cluster_color_LAB = nullptr;
            this->cluster_conf = nullptr;
        }

        ~box_t(){
            if (this->color_RGB != nullptr) delete this->color_RGB;
            if (this->color_LAB != nullptr) delete this->color_LAB;
            if (this->cluster_color_RGB != nullptr) delete this->cluster_color_RGB;
            if (this->cluster_color_LAB != nullptr) delete this->cluster_color_LAB;
            if (this->cluster_conf != nullptr)delete this->cluster_conf;
        }

        bool operator< (const struct box_t *b2){
            if (this->label != b2->label)
                return this->label < b2->label;
            if (this->y != b2->y)
                return this->y > b2->y;
            return this->x < b2->x;
        }

        bool operator<= (const struct box_t *b2){
            if (this->label != b2->label)
                return this->label < b2->label;
            if (this->y != b2->y)
                return this->y > b2->y;
            if(this->x != b2->x)
                return this->x < b2->x;
            return (this->id == b2->id);
        }

        std::string toString() const {
            return "box_t (" + std::to_string((int)this->id) +
                "," + std::to_string((int)this->x) +
                "," + std::to_string((int)this->y)  +
                "," + std::to_string((int)this->rad_size) + ")";
        }

        int* py_Out(){

            int *arr = new int[HOLD_MAT_OUTPUT_ATTRIBUTE_COUNT];
            arr[0] = this->id;
            arr[1] = this->label;
            arr[2] = (int)this->x;
            arr[3] = (int)this->y;
            arr[4] = (int)this->rad_size;
            arr[5] = (int)(this->id == -1 ? this->color_RGB->c1 : this->cluster_color_RGB->c1);
            arr[6] = (int)(this->id == -1 ? this->color_RGB->c2 : this->cluster_color_RGB->c2);
            arr[7] = (int)(this->id == -1 ? this->color_RGB->c3 : this->cluster_color_RGB->c3);
            arr[8] = (this->id == -1 ? -1 : this->cluster_conf->id);
            arr[9] = (this->id == -1 ? -1 : this->cluster_conf->count);
            arr[10] = (int)(this->id == -1 ? -1 : this->cluster_conf->x1);
            arr[11] = (int)(this->id == -1 ? -1 : this->cluster_conf->y1);
            arr[12] = (int)(this->id == -1 ? -1 : this->cluster_conf->x2);
            arr[13] = (int)(this->id == -1 ? -1 : this->cluster_conf->y2);
            
            return arr;
        }

    };

    

    //https://davidjohnstone.net/lch-lab-colour-gradient-picker

    double dtor(double x) { return x*M_PI/180; }
    double rtod(double x) { return x*180/M_PI; }

    void draw_circle(cv::Mat &img,   std::vector<struct box_t*> boxes){

        for (int i = 0; i < (int)boxes.size(); i++){
            
            struct box_t* box = boxes[i];
            
            cv::Point center(box->x, box->y);//Declaring the center point
            int radius = (int)(img.cols*CIRCLE_RADIUS_RATIO); //Declaring the radius
            
            //lab2rgb(, &r, &g, &b);
        //cv::Scalar line_Color((int)box->color_RGB->c1,(int)box->color_RGB->c2,(int)box->color_RGB->c3);//Color of the circle
            cv::Scalar line_Color((int)box->cluster_color_RGB->c1,(int)box->cluster_color_RGB->c2,(int)box->cluster_color_RGB->c3);//Color of the circle
            int thickness = 2;//thickens of the line
        // if(box->label == -1)
                cv::circle(img, center,radius, line_Color, thickness, cv::LINE_8);
            //else
            //  cv::circle(img, center,radius, line_Color, cv::FILLED, cv::LINE_8);

            // cv::Point txt_center(box->x-radius/2, box->y+radius/2);
            // int font_size = 1;//Declaring the font size//
            // cv::Scalar font_Color(0, 0, 0);//Declaring the color of the font//
            // int font_weight = 2;//Declaring the font weight//
            // std::string text = std::to_string(box->label) + " "+std::to_string(i);
        // cv::putText(img,text , txt_center,cv::FONT_HERSHEY_COMPLEX, font_size,font_Color, font_weight);//Putting the text in the matrix//
            
        }
    }

    void draw_path(cv::Mat &img,   std::vector<struct box_t*> boxes){
        int current_label = -1;
        for (int i = 0; i < (int)boxes.size(); i++){

            struct box_t* box = boxes[i];
            if (box->label == -1 || current_label == box->label) continue;
            
            current_label = box->label;
        
            for(int j = 0; j < (int)box->cluster_conf->path.size()-1; j++){
                cv::Point p1(box->cluster_conf->path[j]->x, box->cluster_conf->path[j]->y);//Declaring the center point
                cv::Point p2(box->cluster_conf->path[j+1]->x, box->cluster_conf->path[j+1]->y);//Declaring the center point

                cv::Scalar line_Color((int)box->cluster_color_RGB->c1,(int)box->cluster_color_RGB->c2,(int)box->cluster_color_RGB->c3);//Color of the circle
                int thickness = PATH_THICKNESS_RATIO*img.cols;//thickens of the line

                cv::line(img, p1, p2, line_Color,thickness, cv::LINE_AA);
            }        
        }
    }

    void draw_rectangle(cv::Mat &img,   std::vector<float *> boxes){

        for (int i = 0; i < (int)boxes.size(); i++){
            float* box = boxes[i];
            
            cv::Point p1(box[0], box[1]);//Declaring the center point
            cv::Point p2(box[2], box[3]);//Declaring the center point

            //lab2rgb(, &r, &g, &b);
            cv::Scalar line_Color(0,0,255);//Color of the circle
            int thickness = 2;//thickens of the line

            cv::rectangle(img,p1,p2,line_Color, thickness, cv::LINE_8);
        
            
        }
    }

    void draw_rectangle_around_cluster(cv::Mat &img,   std::vector<struct box_t*> boxes){

        for (int i = 0; i < (int)boxes.size(); i++){
            if (boxes[i]->label == -1) continue;

        
            struct box_t* box = boxes[i];
            
            cv::Point p1(box->cluster_conf->x1, box->cluster_conf->y1);//Declaring the center point
            cv::Point p2(box->cluster_conf->x2, box->cluster_conf->y2);//Declaring the center point

        
            //lab2rgb(, &r, &g, &b);
            cv::Scalar line_Color(0,255,0);//Color of the circle
            int thickness = 2;//thickens of the line

            cv::rectangle(img,p1,p2,line_Color, thickness, cv::LINE_8);
        
            
        }

    }

    double rgb_dist(struct color_t* p1, struct color_t* p2){
        double r = p2->c1 - p1->c1;
        double g = p2->c2 - p1->c2;
        double b = p2->c3 - p1->c3;

        return std::sqrt(r*r + g*g + b*b);

    }

    double point_dist(const cv::Mat &depth_img,struct box_t* p_1, struct box_t* p_2){
        
    // double p_1_depth = depth_img.at<cv::Vec3b>(p_1->y,p_1->x);
    //  double p_2_depth = depth_img.at<cv::Vec3b>(p_2->y,p_2->x);


        double a = p_2->x-p_1->x;
        double b = p_2->y-p_1->y;
    //  double c = p_2_depth-p_1_depth;
        

        return std::sqrt(a*a + b*b);
    }


    void convert_Box_color_RGB_to_LAB(std::vector<struct box_t*> box_loc_color, bool col_type){
        for(int i = 0; i < (int)box_loc_color.size(); i++){
            
            struct color_t* RGB, *LAB;
            if (col_type){
                RGB = box_loc_color[i]->color_RGB;
                LAB = box_loc_color[i]->color_LAB;
            }else{
                RGB = box_loc_color[i]->cluster_color_RGB;
                LAB = box_loc_color[i]->cluster_color_LAB;
            }
            rgb2lab(RGB, LAB);
        }
    }

    void convert_Box_color_LAB_to_RGB(std::vector<struct box_t*> box_loc_color, bool col_type){
        for(int i = 0; i < (int)box_loc_color.size(); i++){
            struct color_t* LAB, *RGB ;
            if (col_type){
                LAB = box_loc_color[i]->color_LAB;
                RGB = box_loc_color[i]->color_RGB;

            }else{
                LAB = box_loc_color[i]->cluster_color_LAB;
                RGB = box_loc_color[i]->cluster_color_RGB;

            }
            lab2rgb(LAB, RGB);
        }
    }

    double GS_CIE2Hue( double a, double b ){
        double xBias = 0.0;
        if (a >= 0 && b == 0)  return 0.0     ; 
        if (a <  0 && b == 0)  return 180.0   ; 
        if (a == 0 && b >  0)  return 90.0    ; 
        if (a == 0 && b <  0)  return 270.0   ; 
        if (a >  0 && b >  0)  xBias = 0.0   ; 
        if (a <  0)               xBias = 180.0 ; 
        if (a >  0 && b <  0)  xBias = 360.0 ; 
        return rtod( atan( b / a ) ) + xBias;
    }

    void lab2lch(struct color_t* lab,struct color_t* lch){
        double var_H = GS_CIE2Hue(lab->c2,lab->c3);

        double C = sqrt( POW2(lab->c2) + POW2(lab->c3) );
        double H = var_H;

        lch->c1 = lab->c1;
        lch->c2 = C;
        lch->c3 = H;

    }

    void lch2lab(struct color_t* lch,struct color_t* lab){
        
        double A = cos( DegToRad( lch->c3 ) ) * lch->c2;
        double B = sin( DegToRad( lch->c3 ) ) * lch->c2;

        lab->c1 = lch->c1;
        lab->c2 = A;
        lab->c3 = B;
    }



    
    std::vector<int> set_union(std::vector<int> set1,  std::vector<int> set2){
        std::vector<int> union_set;
        std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(union_set));
        return union_set;
    }

    void initClusterConf(struct box_t* box, bool labeldCluster){
        struct cluster_t* c = new struct cluster_t;
        c->x1 = labeldCluster ? box->x : -1 ;
        c->x2 = labeldCluster ? box->x : -1 ;
        c->y1 = labeldCluster ? box->y : -1;
        c->y2 = labeldCluster ? box->y : -1;
        c->id = labeldCluster ? box->label: -1;
        c->count = 1;
        box->cluster_conf = c;
    }

    double compareDimension(double a, double b){
        return (a>b? a : b);
    }

    void swapClusters(struct box_t* box1, struct box_t* box2){
        struct cluster_t* src = box1->cluster_conf;
        
        if(box2->x < src->x1 )  src->x1 = box2->x;
        if(box2->x > src->x2 )  src->x2 = box2->x;
        if(box2->y > src->y1 )  src->y1 = box2->y;
        if(box2->y < src->y2 )  src->y2 = box2->y;
        src->count = src->count + 1;

        if(box2->cluster_conf != nullptr){
            box2->cluster_conf->count -= 1;
            if (box2->cluster_conf->count == 0){
                delete box2->cluster_conf;
            }
        }

        box2->cluster_conf = box1->cluster_conf;
        box2->label = box1->label;
        
    }


    std::vector<int> RangeQuery(  std::vector<struct box_t*> boxs, struct box_t *box, const cv::Mat &depth_img, double eps_xy, double eps_lab, int id, int *final_count){
        std::vector<int> neighbors;

        int count = 0;
        //#pragma omp parallel for num_threads(OPENMP_THREAD_COUNT) shared(neighbors,boxs, box, depth_img) reduction(+:count)
        for(int i = 0; i < (int)boxs.size(); i++){
            //potential neighbors could incluide itsself
            double p_dst = point_dist(depth_img, box,boxs[i]);
            double l_dst = LAB_euclidean_distance(box->color_LAB, boxs[i]->color_LAB);
            //double r_dst = rgb_dist(box->color_RGB, boxs[i]->color_RGB);

            


            if(p_dst == 0 && l_dst == 0)
                continue;
            

            if(p_dst <= eps_xy && l_dst <= eps_lab){
                //#pragma omp critical
                neighbors.push_back(i);
                count+=1;
            }
        }
        *final_count = count;
        return neighbors;
    }

    // std::vector<int> RangeQuery(  std::vector<struct box_t*> boxs, struct box_t *box, const cv::Mat &depth_img, double eps_xy, double eps_lab, int id, int *final_count){
    //     std::vector<int> neighbors;
    //     std::vector<bool> isNeighbor(boxs.size(), false);

    //     int count = 0;
    //     #pragma omp parallel for num_threads(OPENMP_THREAD_COUNT) shared(neighbors,boxs, box, depth_img) reduction(+:count)
    //     for(int i = 0; i < (int)boxs.size(); i++){
    //         //potential neighbors could incluide itsself
    //         double p_dst = point_dist(depth_img, box,boxs[i]);
    //         double l_dst = LAB_euclidean_distance(box->color_LAB, boxs[i]->color_LAB);
    //         //double r_dst = rgb_dist(box->color_RGB, boxs[i]->color_RGB);

            


    //         if(p_dst == 0 && l_dst == 0)
    //             continue;
            

    //         if(p_dst <= eps_xy && l_dst <= eps_lab){
    //             // #pragma omp critical
    //             // neighbors.push_back(i);
    //             count+=1;
    //             isNeighbor[i] = true;
    //         }
    //     }
    //     *final_count = count;

    //     for(int i = 0; i < (int)isNeighbor.size(); i++){
    //         if(isNeighbor[i]) neighbors.push_back(i);
    //     }

    //     return neighbors;
    // }

    //  https://davidjohnstone.net/lch-lab-colour-gradient-picker
    void dbscan(std::vector<struct box_t*> boxs, const cv::Mat &depth_img, double eps_xy, double eps_lab, int minPts, int *C_rtn){

        //  for(int i=0; i<boxs.size(); i++)
        //     printf("I: %d ,P%d, %d: %f, %f, %f, %f, %f\n",i,boxs[i]->id,boxs[i]->label, boxs[i]->x,boxs[i]->y,boxs[i]->color_LAB->c1,boxs[i]->color_LAB->c2,boxs[i]->color_LAB->c3);
         
        
        int C = 0;
        
        int neighbor_count = 0;
        for(int p = 0 ; p < (int)boxs.size() ; p++){
            if(boxs[p]->label != UNDEFINED) continue;
            //printf("First RangeQuery: %d: \n", p);
            std::vector<int> neighbors = RangeQuery(boxs, boxs[p],depth_img,eps_xy,eps_lab, p, &neighbor_count);
            if((int)neighbors.size() < minPts){
                boxs[p]->label = NOISE;
                continue;
            }
            C+= 1;
            boxs[p]->label = C;
            initClusterConf(boxs[p], true);
            
            while(!neighbors.empty()){
                int q = neighbors.back();
                neighbors.pop_back();
                if(boxs[q]->label == NOISE ) swapClusters(boxs[p],boxs[q]);
                //here is potentially room for impreovement where i might be able to double check if the node is truely in the right cluster with more range checking istead of just continueing
                else if(boxs[q]->label != UNDEFINED) continue;
                else swapClusters(boxs[p],boxs[q]);
                std::vector<int> neighbors_of_neighbor = RangeQuery(boxs, boxs[q],depth_img,eps_xy, eps_lab, q, &neighbor_count);
                if((int)neighbors_of_neighbor.size() >= minPts){
                // printf("UNION");
                    //printf("size of neighbors: %ld\n", neighbors.size());
                    //printf("size of neighbors_of_neighbor: %ld\n", neighbors_of_neighbor.size());
                    neighbors = set_union(neighbors,neighbors_of_neighbor);
                    //printf("size of neighbors: %ld\n", neighbors.size());

                }
            }


        }

        *C_rtn = C;
        // for(int i=0; i<boxs.size(); i++)
        //     printf("I: %d ,P%d, %d: %f, %f, %f, %f, %f\n",i,boxs[i]->id,boxs[i]->label, boxs[i]->x,boxs[i]->y,boxs[i]->color_LAB->c1,boxs[i]->color_LAB->c2,boxs[i]->color_LAB->c3);
         

    }



    //Geeks for Geeks quicksort Implementation
    int partition(struct box_t ** box_loc_color, int start, int end)
    {
        //printf("partition begin %d, %d\n", start, end);
        struct box_t *pivot = box_loc_color[start];
    
        int count = 0;
        for (int i = start + 1; i <= end; i++) {
            //printf("%d: %d, %d \n", i, pivot->id, box_loc_color[i]->id);
            if (box_loc_color[i]->operator<=(pivot))
                count++;
        }
    
        // Giving pivot element its correct position
        int pivotIndex = start + count;
        //printf("pivot index: %d\n", pivotIndex);
        std::swap(box_loc_color[pivotIndex], box_loc_color[start]);
        //std::iter_swap(box_loc_color.begin()+pivotIndex,box_loc_color.begin()+start);
        // Sorting left and right parts of the pivot element
        int i = start, j = end;
    
        while (i < pivotIndex && j > pivotIndex) {
    
            while (box_loc_color[i]->operator<=(pivot)) 
                i++;
            
    
            while (pivot->operator<(box_loc_color[j]))
                j--;
            
    
            if (i < pivotIndex && j > pivotIndex) 
            std::swap(box_loc_color[i++], box_loc_color[j--]);
                //std::iter_swap(box_loc_color.begin()+i+1,box_loc_color.begin()+j-1);

            
        }
    
        return pivotIndex;
    }

    void quickSort(struct box_t ** box_loc_color, int start, int end)
    {
    
        // base case
        if (start >= end || start < 0 )
            return;
    
        // partitioning the array
        int p = partition(box_loc_color, start, end);
        
        // Sorting the left part
        quickSort(box_loc_color, start, p - 1);
    
        // Sorting the right part
        quickSort(box_loc_color, p + 1, end);
    }



    struct color_t** convertImgRGBtoLAB(const cv::Mat &img, double *avg_l){

        struct color_t **img_pixels = new struct color_t*[img.rows*img.cols];

        double averageBrightness = 0.0;


        #pragma omp parallel for num_threads(OPENMP_THREAD_COUNT) shared(img_pixels) reduction(+:averageBrightness)
        for(int y=0;y<img.rows;y++){
            for(int x=0;x<img.cols;x++){

                cv::Vec3b img_pxl = img.at<cv::Vec3b>(y,x);
                struct color_t* c_RGB = new struct color_t;
                c_RGB->c1 = img_pxl[2];
                c_RGB->c2 = img_pxl[1];
                c_RGB->c3 = img_pxl[0];
                rgb2lab(c_RGB, c_RGB);
                img_pixels[y*img.cols+x] = c_RGB;
                averageBrightness += c_RGB->c1;

            }
        }

        averageBrightness = averageBrightness/ (img.rows * img.cols);
        *avg_l = averageBrightness;
        
        return img_pixels;

    }

    struct color_t** convertImgLABtoRBG(const cv::Mat &img, double *avg_l){

        struct color_t **img_pixels = new struct color_t*[img.rows*img.cols];
        double averageBrightness = 0.0;

        #pragma omp parallel for num_threads(OPENMP_THREAD_COUNT) shared(img_pixels) reduction(+:averageBrightness)
        for(int y=0;y<img.rows;y++){
            for(int x=0;x<img.cols;x++){

                cv::Vec3b img_pxl = img.at<cv::Vec3b>(y,x);
                struct color_t* c_RGB = new struct color_t;
                c_RGB->c1 = img_pxl[2];
                c_RGB->c2 = img_pxl[1];
                c_RGB->c3 = img_pxl[0];
                lab2rgb(c_RGB, c_RGB);
                img_pixels[y*img.cols+x] = c_RGB;
                averageBrightness += c_RGB->c1;
            }
        }

        averageBrightness = averageBrightness/ (img.rows * img.cols);
        *avg_l = averageBrightness;
        
        return img_pixels;

    }








    void increase_box_saturation(std::vector<struct box_t*> box_loc_color){
        #pragma omp parallel for num_threads(OPENMP_THREAD_COUNT) shared(box_loc_color)
        for(int i = 0; i < (int)box_loc_color.size(); i++){
            lab2lch(box_loc_color[i]->color_LAB, box_loc_color[i]->color_LAB);

            if(box_loc_color[i]->color_LAB->c1 > COLOR_SATURATION_BLACK_MAX && box_loc_color[i]->color_LAB->c1 <= COLOR_SATURATION_MIN+10){
                box_loc_color[i]->color_LAB->c1 = 80;
            }
            if(box_loc_color[i]->color_LAB->c1 <= COLOR_SATURATION_BLACK_MAX){
                box_loc_color[i]->color_LAB->c1 = 0;
            }

            box_loc_color[i]->color_LAB->c2 = 120;
            
            lch2lab(box_loc_color[i]->color_LAB, box_loc_color[i]->color_LAB);

        }
    }





    void  avg_color_based_labels(std::vector<struct box_t*> &box_loc_color){
        struct box_t ** arr_boxes = &box_loc_color[0];
        // for(int i=0;i<box_loc_color.size();i++){
        //     printf("P%d: label: %d, X: %f, Y: %f,R: %f, G: %f, B: %f\n",i, arr_boxes[i]->label,  arr_boxes[i]->x, arr_boxes[i]->y,arr_boxes[i]->color_LAB->c1,arr_boxes[i]->color_LAB->c2,arr_boxes[i]->color_LAB->c3);
        // }

        // for(int i=0;i<(int)box_loc_color.size();i++){
        //     printf("I: %d ,P%d, %d: %f, %f, %f, %f, %f\n",i,arr_boxes[i]->id,arr_boxes[i]->label, arr_boxes[i]->x,arr_boxes[i]->y,arr_boxes[i]->color_LAB->c1,arr_boxes[i]->color_LAB->c2,arr_boxes[i]->color_LAB->c3);
        // }

     printf("\n\n\n\n\nstarting quicksort %d\n", box_loc_color.size());
        
        quickSort(arr_boxes,0,box_loc_color.size()-1);
        

    
     printf("comepleted quicksort\n\n\n");
        // for(int i=0;i<box_loc_color.size();i++){
        //     printf("I: %d, P%d, %d: %f, %f, %f, %f, %f\n",i,arr_boxes[i]->id,arr_boxes[i]->label, arr_boxes[i]->x,arr_boxes[i]->y,arr_boxes[i]->color_LAB->c1,arr_boxes[i]->color_LAB->c2,arr_boxes[i]->color_LAB->c3);
        // }

        //memcpy(&box_loc_color[0], &arr_boxes[0], box_loc_color.size());

        for(int i=0;i<(int)box_loc_color.size();i++){
            //box_loc_color[i] = arr_boxes[i];
            //printf("P%d: label: %d, ID: %d\n",i, box_loc_color[i]->label,  box_loc_color[i]->id);
        }

        int avg_r=0,
            avg_g=0,
            avg_b=0, 
            start = -1, 
            count=0;

        if((int)box_loc_color.size() != 0 && box_loc_color[0]->label != -1){
            //printf("box id 0: label not -1\n");
            avg_r=box_loc_color[0]->color_RGB->c1;
            avg_g=box_loc_color[0]->color_RGB->c2;
            avg_b=box_loc_color[0]->color_RGB->c3;
            count ++;
            start = 0;
        }else{
            initClusterConf(box_loc_color[0], false);
            
        }



        for(int i=0;i<(int)box_loc_color.size()-1;i++){
            if(box_loc_color[i]->label == -1){ 
                //printf("%d is label -1\n", i);
                
                box_loc_color[i]->cluster_conf = box_loc_color[0]->cluster_conf;
                box_loc_color[0]->cluster_conf->count += 1;

                
                continue;
            }
            if(start == -1){
                //printf("start is at %d, with box id: %d\n", i, box_loc_color[i]->id);
                avg_r=box_loc_color[i]->color_RGB->c1;
                avg_g=box_loc_color[i]->color_RGB->c2;
                avg_b=box_loc_color[i]->color_RGB->c3;
                count++;
                start = i;
            }
            if(box_loc_color[i]->label == box_loc_color[i+1]->label){
                //printf("box %d and %d have the same label\n", i, i+1);
                avg_r+=box_loc_color[i+1]->color_RGB->c1;
                avg_g+=box_loc_color[i+1]->color_RGB->c2;
                avg_b+=box_loc_color[i+1]->color_RGB->c3;
                count++;
            }else{
                
                int r = avg_r / count;
                int g = avg_g / count;
                int b = avg_b / count;
                //printf("Average color of cluster %d: R: %d, G: %d, B: %d\n",box_loc_color[i]->label, r,g,b );
                for(int j = start; j < i+1; j++){
                box_loc_color[j]->cluster_color_RGB->c1 = r;
                box_loc_color[j]->cluster_color_RGB->c2 = g;
                box_loc_color[j]->cluster_color_RGB->c3 = b;
                }
                start = i+1;
                count = 1;
                avg_r = box_loc_color[i+1]->color_RGB->c1;
                avg_g = box_loc_color[i+1]->color_RGB->c2;
                avg_b = box_loc_color[i+1]->color_RGB->c3;
            }
        }
        if(count != 1){
            count = (int)box_loc_color.size() - start;
            int r = avg_r / count;
            int g = avg_g / count;
            int b = avg_b / count;

            //printf("Average color of cluster %d: R: %d, G: %d, B: %d\n",box_loc_color[box_loc_color.size()-1]->label, r,g,b );
            for(int j = start; j < (int)box_loc_color.size(); j++){
                

                box_loc_color[j]->cluster_color_RGB->c1 = r;
                box_loc_color[j]->cluster_color_RGB->c2 = g;
                box_loc_color[j]->cluster_color_RGB->c3 = b;
            }
        }

        // box_loc_color[j][0] = box_loc_color[j][0];
                // box_loc_color[j][1] = box_loc_color[j][1];

                // box_loc_color[j][2]= r;
                // box_loc_color[j][3] = g;
                // box_loc_color[j][4] = b;

    // printf("Location vec: %d,  Location array: %d\n",&box_loc_color[0] ,&box_loc_color[0]);

        
    }


    /*
    struct human_t * getPixelSize(const cv::Mat &img,const cv::Mat &depth_img, std::vector<float*> human_loc){
        int height = img.rows;
        int width = img.cols;
        int count = 0 ;
        double avg_human_height = 0;
        printf("getting Human Size: %ld\n", human_loc.size());
        int reference_human_x = -1;
        int reference_human_y = -1;
        double reference_human_depth = -1;
        bool set_ref = false;
        struct human_t * human_ref = new struct human_t;

        for(int i = 0; i < human_loc.size(); i++){
            
            double human_width = human_loc[i][2]-human_loc[i][0];
            double human_height = human_loc[i][3]-human_loc[i][1];
            printf("%dx%d , %dx%d " , human_loc[i][0],human_loc[i][1],human_loc[i][2],human_loc[i][3]);
            double ratio = human_width/human_height;
            printf("Human Ratio: %f\n", ratio);
    

            if( ratio > 0.42 && ratio < 0.23) //women = 0.229  men = 0.267
                continue;
                
            double avg_depth = 0;
            double depth_y = human_loc[i][3] - human_height*0.05;
            for(int j = human_loc[i][0] ; j < human_loc[i][2] ; j++)
                avg_depth += depth_img.at<int>(depth_y, j);
            
            if(!set_ref) {
                human_ref->x1 = human_loc[i][0];
                human_ref->y1 = human_loc[i][1];
                human_ref->x2 = human_loc[i][2];
                human_ref->y2 = human_loc[i][3];
                human_ref->height = 

                reference_human_x = human_loc[i][0] + human_width/2;
                *ref_width = reference_human_x;
                *ref_height = reference_human_y;
                
                reference_human_y = depth_y;
                reference_human_depth = avg_depth/(human_width);
                set_ref = true;
                avg_human_height += human_height;
                count ++;
                continue;
            }

            double normalized_height = human_height/avg_depth * reference_human_depth;

            printf("Human Ratio Accepted: %f\n", ratio);
            avg_human_height += normalized_height;
            count ++;
            
        }

        if (count == 0){
            human_ref->depth_x = depth_img.cols * 0.50;
            human_ref->depth_y = depth_img.rows * 0.85;
            human_ref->ratio = 0.3;
            human_ref->size_in_pxls = img.rows/7;
        }else{
            human_ref->height = avg_human_height/count;
        }
        
        return human_ref;
    }
    */





    bool boxs_in_line(struct box_t* a,  struct box_t* b, struct box_t* c){
        if( a->x >= b->x && b->x >= c->x) return true;
        if(a->x <= b->x && b->x <= c->x) return true;
        return false;
    }

    void add_point_to_path(struct box_t* box_a, double x, double y, int id_to){
          //printf("adding point from %d to %d: X: %f Y: %f", box_a->id, id_to, x, y);
        struct path_point_t* p = new struct path_point_t;
        p->id = box_a->cluster_conf->path.size();
        p->x = x;
        p->y = y;
        p->from = box_a->id;
        p->to = id_to;
        if(id_to == -1){
            p->from = -1;
            p->to = box_a->id;
        }
        box_a->cluster_conf->path.push_back(p);
    }


    void calculate_avg_cluster_pathing(std::vector<struct box_t*> &boxes){


        
        for(int i=0; i< (int)boxes.size()-1; i++){
            struct box_t* box_a = boxes[i];
            if(box_a->label == -1)
                continue;
            if((int)box_a->cluster_conf->path.size() == 0){
                add_point_to_path(box_a, box_a->x , box_a->y, -1);
                continue;
            }

            struct box_t* box_b = boxes[i+1];

            if(box_b->label != box_a->label){
                
                add_point_to_path(box_a, box_a->x , box_a->y, -1);
                continue;
            }
            if(i+1 == (int)boxes.size()-1){
                
                add_point_to_path(box_a, box_b->x , box_b->y, box_b->id);
                break;
            }
            struct box_t* box_c = boxes[i+2];
            if (box_b->label != box_c->label){
                add_point_to_path(box_a, box_b->x , box_b->y, box_b->id);
                continue;
            }
            if(boxs_in_line(box_a, box_b, box_c)){
                //values of offstest will have to be updated every for evvery image size so make it a ratio
                double x_1_offset = 0;
                double y_1_offset = 10;
                double x_2_offset = (box_c->x-box_b->x)/2;
                double y_2_offset = (box_c->y-box_b->y)/3*2;
                add_point_to_path(box_a, box_b->x + x_1_offset , box_b->y + y_1_offset, box_b->id);
                add_point_to_path(box_a, box_b->x + x_2_offset , box_b->y + y_2_offset, box_c->id);

                i = i+1;
            }else{
                
                double x = (box_b->x-box_a->x)/2 + box_a->x;
                double y = (box_b->y-box_a->y)/2 + box_a->y;
                add_point_to_path(box_a, x , y, box_b->id);
            }
        }
    }

    //https://www.youtube.com/watch?v=7fCheEYUpgU
  



    PyImgWriter* process_image(PyImgReader* reader){

        
        std::vector<struct box_t*>  box_loc_color;

       //std::string depth_model_file_path = "../models/image_depth/";

        // Read Network
    // std::string depth_model = "dpt_next_vit_large_384.pt"; // MiDaS v2.1 Large
        //string model = "model-small.onnx"; // MiDaS v2.1 Small


    // cv::Mat img = cv::imread(file_path, cv::IMREAD_COLOR);

        //printf("connected to sockets");
    /*
        // Read in the neural network from the files
        auto depth_net = cv::dnn::dnn4_v20230620::readNet(depth_model_file_path+depth_model);


        if (depth_net.empty())
        {
            return -1;
        }
        depth_net.setPreferableBackend(cv::dnn::dnn4_v20230620::DNN_BACKEND_CUDA);
        depth_net.setPreferableTarget(cv::dnn::dnn4_v20230620::DNN_TARGET_CUDA);



        //if(argv[3] == "d")
        //for (const auto & entry : fs::directory_iterator(argv[3]))
        
        {
        //std::cout << entry.path() << std::endl;
        //cv::Mat img = cv::imread(entry.path(),cv::IMREAD_ANYDEPTH | cv::IMREAD_COLOR);
        
        
        cv::Mat img = cv::imread(argv[3], cv::IMREAD_COLOR);

        cv::Mat results1, results2;

        //removeShadow(img, results1, results2);

    // cv::imshow("results1", results1);
        //cv::imshow("results2", results2);



        cv::Mat depth_blob = cv::dnn::blobFromImage(img, 1 / 255.f, cv::Size(384, 384), cv::Scalar(123.675, 116.28, 103.53), true, false);


        depth_net.setInput(depth_blob);

            // Forward pass of the blob through the neural network to get the predictions
        cv::Mat output = depth_net.forward(getOutputsNames(depth_net)[0]);

            // Convert Size to 384x384 from 1x384x384
        const std::vector<int32_t> size = { output.size[1], output.size[2] };
        output = cv::Mat(static_cast<int32_t>(size.size()), &size[0], CV_32F, output.ptr<float>());
            // Resize Output Image to Input Image Size
            cv::resize(output, output, img.size());


            // Visualize Output Image

            double min, max;
            cv::minMaxLoc(output, &min, &max);
            const double range = max - min;

            // 1. Normalize ( 0.0 - 1.0 )
            output.convertTo(output, CV_32F, 1.0 / range, -(min / range));

            // 2. Scaling ( 0 - 255 )
            output.convertTo(output, CV_8U, 255.0);



            //putText(output, "FPS: " + to_string(int(fps)), Point(50, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 2, false);

            cv::imshow("image", img);
            cv::imshow("depth", output);

    */



        //printf("Processing Image size: %d %d %d\n", mobile_image.cols, mobile_image.rows, mobile_image.depth());
        

        cv::Mat mobile_image = *reader->getImg();
        cv::Mat depth_img = *reader->getDepthImg();
        cv::Mat hold_loc = *reader->getHoldLoc();
        cv::Mat human_loc = *reader->getHumanLoc();
        std::cout << "hold_loc Mat:" << std::endl;
        std::cout << hold_loc << std::endl<< std::endl<< std::endl<< std::endl;

        std::cout << "human_loc Mat:" << std::endl;
        std::cout << human_loc << std::endl;

        


        struct human_t * human_ref = new struct human_t;
        human_ref->depth_x = depth_img.cols * 0.50;
        human_ref->depth_y = depth_img.rows * 0.85;
        human_ref->ratio = 0.3;
        human_ref->size_in_pxls = mobile_image.rows/7;

        //printf("Human Height: %f at ref pixels: %fx%f\n", human_ref->size_in_pxls, human_ref->depth_x,human_ref->depth_y);
        //print_box_results(hold_loc);
        ////printf("HUMAN LOCATIONS:\n");
        //print_box_results(human_loc);
        //speed up by only having to focus on the wall instead of the entire image 
        //see if first and last box from vector are actually the first and last boxes found to get area of wall potentially
        // printf("Starting median blurr\n");
        cv::Mat wall_colors, resized_img, resized_wall_colors , img_lab, img_labeld = mobile_image , img_color_classify = mobile_image;
        //resize_img(img, wall_colors, 800);

        int max_img_dim = mobile_image.rows>mobile_image.cols?mobile_image.rows:mobile_image.cols;
        double ratio = max_img_dim/800;

        int kernal_size = 16*ratio;
        kernal_size = kernal_size %2 == 0 ? kernal_size+1: kernal_size;
        int kernal_size_decr = 1.5*ratio;

        kernal_size_decr = kernal_size_decr %2 == 0 ? kernal_size_decr: kernal_size_decr+1;

        //printf("kernal_size: %d, kernal_size_decr: %d\n", kernal_size,kernal_size_decr);
        cv::medianBlur(mobile_image, wall_colors, kernal_size);
        for(int i = 0; i < 4; i++)
            cv::medianBlur(wall_colors, wall_colors, kernal_size-(i*kernal_size_decr));
        
        //resize_img(wall_colors, resized_wall_colors, img.rows);
        
    // cv::imshow("wall_colors", resized_wall_colors);

        //printf("wall_colors image size: %d %d %d\n", wall_colors.cols, wall_colors.rows, mobile_image.depth());
        //printf("image size: %d %d %d\n", mobile_image.cols, mobile_image.rows, mobile_image.depth());

        double avg_brightness_blurr = 0.0 ;

        
        struct color_t **wall_pixels= convertImgRGBtoLAB(wall_colors, &avg_brightness_blurr);

        
    

        printf("wall_pixels avg brightness blur: %f\n", avg_brightness_blurr);
        double avg_brightness_org = 0.0;
        struct color_t **img_pixels = convertImgRGBtoLAB(mobile_image, &avg_brightness_org);
        printf("wall_pixels avg brightness blur: %f\n", avg_brightness_org);


      
    // printf("got wall colors\n");


        

        //printf("pthread_join completed\n");



        

        //Data calculations complete: now use it to calculate app features

        //Data Processing start:

        box_loc_color =  get_box_colors(mobile_image, img_pixels, wall_pixels, hold_loc);
        printf("finished parsing boxes\n");

        //done with using the converted images
        for (int i = 0; i < mobile_image.rows*mobile_image.cols; ++i) {
            delete img_pixels[i];
            delete wall_pixels[i];
        }
        delete [] img_pixels;
        delete [] wall_pixels;


        

        //setting up for clustering 

        convert_Box_color_RGB_to_LAB(box_loc_color, false);
        int num_labels;

       //increase_box_saturation(box_loc_color);




        //COLOR_DISTANCE_THRES box_t+0.65

     printf("finished increasing box_saturation\n");

        dbscan(box_loc_color, depth_img, human_ref->size_in_pxls*1.36, 8.5, 3, &num_labels);


     printf("finished dbscan\n");

        convert_Box_color_LAB_to_RGB(box_loc_color, false);
     printf("finished convert_Box_color_LAB_to_RGB\n");

        avg_color_based_labels(box_loc_color);
     printf("finished avg_color_based_labels\n");

        calculate_avg_cluster_pathing(box_loc_color);

     printf("Completed calculate_avg_cluster_pathing\n");





    //     img_color_classify = mobile_image ;

    // //     cv::Mat graph_mat(cv::Size(mobile_image.cols, mobile_image.rows), CV_8UC3,cv::Scalar(0, 0, 0) );

    // //     graph_mat = cv::Scalar::all(0);
    // //     draw_circle(graph_mat, box_loc_color);
    //      draw_circle(img_color_classify, box_loc_color);
    //  //   draw_rectangle(img_color_classify, human_loc);
    // //    // draw_rectangle_around_cluster(img_color_classify,box_loc_color);

    // //     draw_path(graph_mat,box_loc_color);
    //     draw_path(img_color_classify,box_loc_color);

    // //     resize_img(img_color_classify, img_color_classify, 500);
    // //     resize_img(graph_mat, graph_mat, 500);




    //      cv::imshow("img_color_classify", img_color_classify);
    // //     cv::imshow("img_Graph", graph_mat);




    //     int k;
    //     while(k = cv::waitKey(0)){
    //     if(k == 'c')
    //         exit(0);
        
    //     if(k == 'n'){
    //          break;
    //     }
    //     }

        


        PyImgWriter* ret_obj = convert2PyWirterObj(box_loc_color);

        //delete human_ref;   
        // for(int i = 0; i < (int)box_loc_color.size(); i++) 
        //     delete box_loc_color[i];
        
        printf("done with Processing\n");
        

        return ret_obj;
    }

    

    PyImgWriter* convert2PyWirterObj(std::vector<struct box_t*> boxes){


        cv::Mat box_mat = cv::Mat::zeros((int)boxes.size(), HOLD_MAT_OUTPUT_ATTRIBUTE_COUNT, CV_32S);

        std::vector<std::vector<struct path_point_t*>> paths;
        
        int currentLabel = -1;
        int highestPathCount = 0;
        bool newPath = false;
        
    // Push your own values onto the matrix 
        for (int i = 0; i < box_mat.rows; ++i) {
            int *hold_output = boxes[i]->py_Out();

            if(boxes[i]->label != -1 && boxes[i]->label != currentLabel){
                if(currentLabel != -1) newPath = true;
                currentLabel = boxes[i]->label;
            }


            for (int j = 0; j < box_mat.cols; ++j){
            
                box_mat.at<int>(i, j) = hold_output[j];
            }

            if (newPath){
                paths.push_back(boxes[i-1]->cluster_conf->path);
                if (highestPathCount < (int)boxes[i-1]->cluster_conf->path.size()){
                    highestPathCount = boxes[i-1]->cluster_conf->path.size();
                    printf("Max Path Size: %d\n", highestPathCount);
                }
                newPath = false;
            }


            delete hold_output;
        }

        if (currentLabel != -1){
            paths.push_back(boxes[boxes.size()-1]->cluster_conf->path);
        }

        printf("paths count: %d\n", paths.size());

        cv::Mat paths_mat = cv::Mat::zeros((int)paths.size(), highestPathCount*PATH_MAT_OUTPUT_ATTRIBUTE_COUNT, CV_32S);
        
        for (int i = 0; i < (int)paths.size(); ++i) {
            for (int j = 0; j < highestPathCount; ++j ) {
                int * path_output;
                printf("Path %d: Current: %d\n", i, j);
                if (j < (int)paths[i].size()) path_output = paths[i][j]->py_Out();
                else{
                    path_output = new int[PATH_MAT_OUTPUT_ATTRIBUTE_COUNT];
                    for (int k=0; k<PATH_MAT_OUTPUT_ATTRIBUTE_COUNT; k++)
                        path_output[k] = -1;
                }

                
                
                
                for(int k = 0; k < PATH_MAT_OUTPUT_ATTRIBUTE_COUNT; ++k){
                    printf("Val: %d\n",path_output[k]);
                    paths_mat.at<int>(i, j*PATH_MAT_OUTPUT_ATTRIBUTE_COUNT + k) = path_output[k];
                }
                delete path_output;
                
            }
        }


        return new PyImgWriter(box_mat,paths_mat);
  
    }



    void const print_box_results( std::vector<float*> boxes){
        std::cout.precision(10);
            for(int i = 0; i < (int)boxes.size() ; i++){
                std::cout<<"[";
                for(int j = 0; j < BOX_PARAMS_COUNT; j++)
                    std::cout << boxes.at(i)[j] << ", ";
                std::cout<<"]\n";
            }
    }

    void resize_img(cv::Mat& img,cv::Mat& dst, int max_height){
        int width = img.cols, height = img.rows;
        int max_dim = ( width >= height ) ? width : height;
        int min_dim = ( width >= height ) ? height : width;
        //double ratio = (double)max_height / (double)max_dim;
        double ratio = 0.0;
        if (max_dim == width)
            ratio = (double)max_dim / (double)min_dim;
        else
            ratio = (double)min_dim / (double)max_dim;

        int new_width = (int) (max_height*ratio);

        //printf("resize_img:max_dim = %d, ratio = %f, height = %d, width=%d\n",max_dim, ratio, max_height,new_width);

        cv::resize(img, dst, cv::Size(new_width,max_height));
    }

    double modifyRGBValue(double p){
        if ( p > 0.04045 ) return pow(( ( p + 0.055 ) / 1.055 ) , 2.4);
        else               return p = p / 12.92;
    }

    //amazing color conversion reference: https://www.easyrgb.com/en/math.php

    void rgb2lab(struct color_t* RGB, struct color_t* LAB){

        double var_R = modifyRGBValue( (double)RGB->c1 / 255.0 )* 100;
        double var_G = modifyRGBValue( (double)RGB->c2 / 255.0 )* 100;
        double var_B = modifyRGBValue( (double)RGB->c3 / 255.0 )* 100;


        double X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
        double Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
        double Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;
        double Reference_X = 95.047;
        double Reference_Y = 100.0;
        double Reference_Z = 108.883;


        double var_X = X / Reference_X;
        double var_Y = Y / Reference_Y;
        double var_Z = Z / Reference_Z;

        if ( var_X > 0.008856 ) var_X = std::cbrt(var_X);
        else                    var_X = ( 7.787 * var_X ) + ( 16 / 116 );
        if ( var_Y > 0.008856 ) var_Y = std::cbrt(var_Y);
        else                    var_Y = ( 7.787 * var_Y ) + ( 16 / 116 );
        if ( var_Z > 0.008856 ) var_Z = std::cbrt(var_Z);
        else                    var_Z = ( 7.787 * var_Z ) + ( 16 / 116 );

        double l = ( 116 * var_Y ) - 16;
        double a = 500 * ( var_X - var_Y );
        double b = 200 * ( var_Y - var_Z );

        LAB->c1 = l;
        LAB->c2 = a;
        LAB->c3 = b;

        
        
    }

    void lab2rgb(struct color_t* LAB,struct color_t* RGB){
        double var_Y = ( LAB->c1 + 16.0 ) / 116.0;
        double var_X = LAB->c2 / 500.0 + var_Y;
        double var_Z = var_Y - LAB->c3 / 200.0;

        if ( pow(var_Y,3)  > 0.008856 ) 
            var_Y = pow(var_Y,3);
        else                       
            var_Y = ( var_Y - 16.0 / 116.0 ) / 7.787;
        if ( pow(var_X,3)  > 0.008856 ) 
            var_X = pow(var_X,3);
        else                       
            var_X = ( var_X - 16.0 / 116.0 ) / 7.787;
        if ( pow(var_Z,3)  > 0.008856 ) 
            var_Z = pow(var_Z,3);
        else                       
            var_Z = ( var_Z - 16.0 / 116.0 ) / 7.787;
        double Reference_X = 95.047;
        double Reference_Y = 100.0;
        double Reference_Z = 108.883;
        double X = var_X * Reference_X;
        double Y = var_Y * Reference_Y;
        double Z = var_Z * Reference_Z;

        var_X = X / 100;
        var_Y = Y / 100;
        var_Z = Z / 100;

        double var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
        double var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
        double var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

        if ( var_R > 0.0031308 ) var_R = 1.055 * ( pow(var_R , ( 1.0 / 2.4 )) ) - 0.055;
        else                     var_R = 12.92 * var_R;
        if ( var_G > 0.0031308 ) var_G = 1.055 * ( pow(var_G , ( 1.0 / 2.4 )) ) - 0.055;
        else                     var_G = 12.92 * var_G;
        if ( var_B > 0.0031308 ) var_B = 1.055 * ( pow(var_B , ( 1.0 / 2.4 )) ) - 0.055;
        else                     var_B = 12.92 * var_B;

        double R = (double)(var_R * 255);
        double G = (double)(var_G * 255);
        double B = (double)(var_B * 255);

        RGB->c1 = R;
        RGB->c2 = G;
        RGB->c3 = B;


    }

    //CIEDE2000 color comparison algorithm
    // A hue rotation term (RT), to deal with the problematic blue region (hue angles in the neighborhood of 275°):[19]
    // Compensation for neutral colors (the primed values in the L*C*h differences)
    // Compensation for lightness (SL)
    // Compensation for chroma (SC)
    // Compensation for hue (SH)
    double LAB_euclidean_distance( struct color_t* a, struct color_t* b){

        const double eps = 1e-5;

        // calculate ci, hi, i=1,2
        double c1 = std::sqrt(SQR(a->c2) + SQR(a->c3));
        double c2 = std::sqrt(SQR(b->c2) + SQR(b->c3));
        double meanC = (c1 + c2) / 2.0;
        double meanC7 = POW7(meanC);

        double g = 0.5*(1 - std::sqrt(meanC7 / (meanC7 + 6103515625.))); // 0.5*(1-sqrt(meanC^7/(meanC^7+25^7)))
        double a1p = a->c2 * (1 + g);
        double a2p = b->c2 * (1 + g);

        c1 = std::sqrt(SQR(a1p) + SQR(a->c3));
        c2 = std::sqrt(SQR(a2p) + SQR(b->c3));
        double h1 = fmod(atan2(a->c3, a1p) + 2*M_PI, 2*M_PI);
        double h2 = fmod(atan2(b->c3, a2p) + 2*M_PI, 2*M_PI);

            // compute deltaL, deltaC, deltaH
        double deltaL = b->c1 - a->c1;
        double deltaC = c2 - c1;  
        double deltah;

        if (c1*c2 < eps) {
            deltah = 0;
        }
        if (std::abs(h2 - h1) <= M_PI) {
            deltah = h2 - h1;
        }
        else if (h2 > h1) {
            deltah = h2 - h1 - 2* M_PI;
        }
        else {
            deltah = h2 - h1 + 2 * M_PI;
        }
        double deltaH = 2 * std::sqrt(c1*c2)*std::sin(deltah / 2);
            // calculate CIEDE2000
        double meanL = (a->c1 + b->c1) / 2;
        meanC = (c1 + c2) / 2.0;
        meanC7 = POW7(meanC);
        double meanH;

        if (c1*c2 < eps) {
            meanH = h1 + h2;
        }
        if (std::abs(h1 - h2) <= M_PI + eps) {
            meanH = (h1 + h2) / 2;
        }
        else if (h1 + h2 < 2*M_PI) {
            meanH = (h1 + h2 + 2*M_PI) / 2;
        }
        else {
            meanH = (h1 + h2 - 2*M_PI) / 2;
        }

        double T = 1
                - 0.17*std::cos(meanH - DegToRad(30))
                + 0.24*std::cos(2 * meanH)
                + 0.32*std::cos(3 * meanH + DegToRad(6))
                - 0.2*std::cos(4 * meanH - DegToRad(63));
        double sl = 1 + (0.015*SQR(meanL - 50)) / std::sqrt(20 + SQR(meanL - 50));
        double sc = 1 + 0.045*meanC;
        double sh = 1 + 0.015*meanC*T;
        double rc = 2 * std::sqrt(meanC7 / (meanC7 + 6103515625.));
        double rt = -std::sin(DegToRad(60 * std::exp(-SQR((RadToDeg(meanH) - 275) / 25)))) * rc;

        double finalResult = std::sqrt(SQR(deltaL / sl) + SQR(deltaC / sc) + SQR(deltaH / sh) + rt * deltaC / sc * deltaH / sh);
        //printf("final Calculated Distance: %f between L: %f, A: %f, B: %f     L: %f, A: %f, B: %f\n",finalResult,a->c1 ,a->c2 ,a->c3 ,b->c1,b->c2,b->c3);
        return finalResult;
    }


    bool not_similar_colors(struct color_t* a, struct color_t* b){
        //printf("not_similar_colors\n");
        //printf("A:  R: %f, G: %f, B: %f\n", a->c1,a->c2,a->c3);
    // printf("B:  R: %f, G: %f, B: %f\n\n", b->c1,b->c2,b->c3);

        double finalResult = LAB_euclidean_distance(a, b);
        //printf("final Calculated Distance: %f\n",finalResult);

        if(finalResult <= COLOR_DISTANCE_THRESbox_t)
            return false;
        

        return true;
    }

    struct box_t* get_avrg_box_t_color(const cv::Mat &img, struct color_t** img_lab_colors, struct color_t** wall_lab_colors, float*loc){
        int rows = img.rows;
        int cols = img.cols;
        int centerX = (int)(loc[0] + loc[2])/2;
        int centerY = (int)(loc[1] + loc[3])/2;
        //printf("center: %d, %d\n", centerX, centerY);
        
        double r=img.at<cv::Vec3b>(centerY+2,centerX-2)[0] + img.at<cv::Vec3b>(centerY+2,centerX+2)[0];
        double g=img.at<cv::Vec3b>(centerY+2,centerX-2)[1] + img.at<cv::Vec3b>(centerY+2,centerX+2)[1];
        double b=img.at<cv::Vec3b>(centerY+2,centerX-2)[2] + img.at<cv::Vec3b>(centerY+2,centerX+2)[2];
        // double r=img.at<cv::Vec3b>(centerY,centerX)[0];
        // double g=img.at<cv::Vec3b>(centerY,centerX)[1];
        // double b=img.at<cv::Vec3b>(centerY,centerX)[2];
        double avg_r =0, avg_g =0, avg_b =0, p_count = 0;

        //printf("X1: %f, Y1: %f, X2: %f, Y2: %f\n", loc[0], loc[1], loc[2], loc[3]);
    // printf("int size: %ld\n", sizeof(img_lab_colors)/sizeof(img_lab_colors[0]));
        for(int y=(int)loc[1]; y <= (int)loc[3]; y++){
            for(int x=(int)loc[0]; x <=(int)loc[2]; x++){
            
            // printf("X: %d, Y: %d\n",i,j);
                if ( x >= 0 && y>= 0 && x < cols && y < rows){
                //printf("entered if at i: %d, j: %d\n", i, j);

                struct color_t* img_pxl = img_lab_colors[cols*y+x];
                struct color_t* wall_pxl = wall_lab_colors[cols*y+x];
                //printf("LAB COLOR OF IMAGE: L = %f, A = %f, B = %f\n ", img_pxl->c1,  img_pxl->c2,  img_pxl->c3);
                if(not_similar_colors(img_pxl,wall_pxl)){
                    
                    avg_r += img.at<cv::Vec3b>(y,x)[0];
                    avg_g += img.at<cv::Vec3b>(y,x)[1];
                    avg_b += img.at<cv::Vec3b>(y,x)[2];
                    p_count += 1;
                    }
                }
            }
        }

        //double new_l,new_a,new_b;
        if (p_count != 0){
            r = avg_r / p_count;
            g = avg_g / p_count;
            b = avg_b / p_count;
            
        //rgb2lab(r,g,b, &new_l, &new_a, &new_b);
        }

        struct color_t* c = new struct color_t;
        // arr[0] = new_l;
        // arr[1] = new_a;
        // arr[2] = new_b;
        c->c1 = r;
        c->c2 = g;
        c->c3 = b;

        struct box_t* box = new struct box_t;
        box->x = centerX;
        box->y = centerY;

        box->color_RGB = c;

        box->color_LAB = new struct color_t;
        rgb2lab(box->color_RGB, box->color_LAB);
        box->cluster_color_RGB = new struct color_t;
        box->cluster_color_LAB = new struct color_t;
        box->label = UNDEFINED;
        box->cluster_conf = nullptr;
        box->rad_size = (loc[2] - loc[0])/2;

        return box;
    }


    std::vector<struct box_t*> get_box_colors(const cv::Mat &img, struct color_t** img_lab_colors, struct color_t** wall_colors,  cv::Mat boxes_loc){
        //std::vector<std::tuple<int, int, std::tuple<int, int, int>>> box_loc_color;
        std::vector<struct box_t*> box_loc_color;
        //printf("num_boxes: %ld\n", boxes_loc.size());

        for(int i = 0; i < boxes_loc.rows;i++){
            
            float x1 = boxes_loc.at<float>(0,i);
            float y1 = boxes_loc.at<float>(1,i);
            float x2 = boxes_loc.at<float>(2,i);
            float y2 = boxes_loc.at<float>(3,i);
            float loc[4] = {x1,y1,x2,y2};

            int centerX = (int)(x1 + x2)/2;
            int centerY = (int)(y1 + y2)/2;
            //if(centerX < img.cols && centerY < img.rows && centerX > 0 && centerY  > 0)
            {
                printf("total: %ld, I = %d, centerX: %d, centerY: %d       MaxX: %d, MaxY: %d\n", box_loc_color.size(),i, centerX, centerY, img.cols, img.rows);
                struct box_t* box = get_avrg_box_t_color(img, img_lab_colors, wall_colors, loc);
                box->id = i;
                box_loc_color.push_back(box);
            }
        }

        //printf("finished Parsing Boxes\n");

        return box_loc_color;

    }

}