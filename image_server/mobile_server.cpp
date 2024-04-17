
//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux  
#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include "ProcessImage.h" 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sched.h>
#include "linked_list.h"
#include <opencv2/opencv.hpp>
#include "../model_server/run_model_server.h"

#include <chrono>
#include <ctime>


#define TRUE   1  
#define FALSE  0  
#define PORT 8888  
#define MAX_CLIENTS 100
#define MAX_NUM_THREADS 100
#define MAX_RECV_BUFFER_SIZE 1024
#define MAX_MOBILE_PARAMETERS 6
#define MAX_NUM_MODEL_THREADS 1
#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

pthread_t model_threads[MAX_NUM_MODEL_THREADS];
int model_ports[MAX_NUM_MODEL_THREADS];
int model_counts[MAX_NUM_MODEL_THREADS];

// POSIX thread declarations and scheduling attributes
//
// pthread_t threads[MAX_NUM_THREADS];
// threadParams_t threadParams[MAX_NUM_THREADS];

LinkedList* threads;
struct sockaddr_in address; 
int master_socket; 
int totalClientCount = 0;

int send_data(int socket, const char *buf, int len){
    
    int bytes_sent = 0, total_bytes_sent = 0;   
    while(total_bytes_sent != len){
        bytes_sent = send(socket, buf + total_bytes_sent, len - total_bytes_sent, 0);
        //printf("bytes_sent: %d\n", bytes_sent);
        if(bytes_sent == 0){
            printf("No bytes sent\n");
            break;
        }
        if(bytes_sent == -1){
            printf("Send threw error\n");
            break;
        }
        total_bytes_sent += bytes_sent;
    }

    return bytes_sent;

}


void *image_thread(void *threadp){

    printf("In ImageThread\n");

    threadParams_t* thread_param = (threadParams_t*) threadp;

    double *parameters = new double[MAX_MOBILE_PARAMETERS];
    int param_count = 0;
    std::vector<int> image_data_double;

    int start = 0;

    //int image_start = 0;
    //int image_end = 0;
    bool at_image_data = false;


    cv::String buffer = thread_param->getMobileBuffer();


    for(int i = 0; i < thread_param->getMobileBufferSize(); i++){
 
        if (buffer[i] == '-'){
            //potential off by one error
            //image_end = i;
            break;
        }
        if(buffer[i] == '+'){
            //image_start = i+1;
            at_image_data = true;
            start = i+1;
        }
        
        else if( buffer[i] == ' '){
            int size = i-start;
            // if(!at_image_data && size > 4)
            //     size = 4;
            char *num = new char[size];
            int num_idx = 0;
            for(int j = 0; j < size; j++){
                if(!std::isdigit(buffer[start + j])) continue;


                num[num_idx] = buffer[start + j];
                num_idx += 1;
                
            }
            start = i+1;
            

            float value = atof(num);
            if(!at_image_data){
                std::cout<<"Sizes: "<< num<<std::endl;
                parameters[param_count] = value;
                param_count += 1;

            }else{
             image_data_double.push_back((int)value);
             //printf("added value: %f\n", value);
            }

            delete num;

        }
    }

    printf("Parsing complete\n");

    //char * img_char = new char [image_end - image_start];
   // memcpy((void*)img_char, (void*) (thread_param->getMobileBuffer()+image_start),( image_end - image_start)*sizeof(char));
   // strcpy(img_char, thread_param->getMobileBuffer().c_str());

   // cv::Mat m;

    // if(image_data_double.size() == (int)parameters[0]* (int)parameters[1] * 3) // check that the rows and cols match the size of your vector
    // {
       // m = cv::Mat((int)parameters[0], (int)parameters[1], CV_8UC3); // initialize matrix of uchar of 1-channel where you will store vec data
        //copy vector to mat
    printf("memcpy\n");

   // printf("Error size of image is not equal to size of input\n");
 
    //cv::Mat testMat = cv::Mat( image_data_double ).reshape( 3, (int)parameters[1] );

   // testMat.convertTo( testMat, CV_8UC3 );
       // memcpy(m.data, image_data_double.data(), image_data_double.size()*sizeof(int)); // change uchar to any type of data values that you want to use instead

   // }else{

   // }
    // int offset = (int)parameters[0]* (int)parameters[1];

    // std::vector<int> final_data;
    // for(int i = 0; i <offset ; i++){
    //     final_data.push_back(image_data_double[i]);
    //     final_data.push_back(image_data_double[i+offset]);
    //     final_data.push_back(image_data_double[i+2*offset]);
    // } 

    printf("Expected size of image is: %d, got: %ld\n",(int)parameters[0]* (int)parameters[1] * 3, image_data_double.size());
    printf("Image: height = %d, width = %d\n", (int)parameters[0], (int)parameters[1]);

    if((int)parameters[0]* (int)parameters[1] * 3 != (int)image_data_double.size()){
        //close(thread_param->getSocketFd());
        return (void *)0;
    }

    cv::Mat image_data(cv::Size((int)parameters[1], (int)parameters[0]), CV_8UC3,cv::Scalar(0, 0, 0) );
    //int image_size = (int)parameters[1] * (int)parameters[0];
    for(int y=0;y<image_data.rows;y++)
        {
            for(int x=0;x<image_data.cols;x++)
                {
                    
                    // get pixel
                    cv::Vec3b &color = image_data.at<cv::Vec3b>(y,x);

                    // ... do something to the color ....
                    color[0] = image_data_double[3*(y*image_data.cols+x)];
                    color[1] = image_data_double[3*(y*image_data.cols+x)+1];
                    color[2] = image_data_double[3*(y*image_data.cols+x)+2];

                    // set pixel
                    //image.at<Vec3b>(Point(x,y)) = color;
                    //if you copy value
                }
    }   
    //cv::Mat image_data = cv::Mat(image_data_double).reshape(3, (int)parameters[1]);
    //cv::Mat image = cv::imdecode(image_data, cv::IMREAD_COLOR);

    // cv::imshow("Image from Phone", image_data);

    // int k;
    // while(k = cv::waitKey(0)){
    // if(k == 'c')
    //     exit(0);
    
    // if(k == 'n'){
    //      break;
    // }
    // }    


    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;
    duration<double, std::milli> ms_double;
    std::vector<struct box_t*> routes;
    t1 = high_resolution_clock::now();

    process_image(image_data, thread_param->getModelPort(),routes);

    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;

     

    printf("\n\n\n\nProcessing Complete with a total time ellapsed using %d number of threads: %f\n",OPENMP_THREAD_COUNT, ms_double.count());
    //printf("Hold Count: %ld\n", routes.size());
    //char* test = "Processing Complete";



    //const char * return_char = return_string.c_str();
    const char *return_char; 
    int size = convert_data_to_char_array(routes, return_char);
    
    //printf("sending Data:");
    if(size != -1){
        send_data(thread_param->getSocketFd(), return_char, size);
        free((void*)return_char);
    }
    else{
        send_data(thread_param->getSocketFd(), "Error Converting", 17);
    }

   // printf("done sending data\n");
    
    close(thread_param->getSocketFd());
    


    return((void *)0);

}


void init_model_server(){

    for(int i=0; i< MAX_NUM_MODEL_THREADS ; i++){
        int port = find_open_port(10000);
        model_ports[i] = port;
        model_counts[i] = 0;
        int error;
        if(( error = pthread_create(&model_threads[i], NULL, run_model_servers, (void *)(&model_ports[i]))) != 0){
                handle_error_en(error, "pthread_create");
                model_ports[i] = -1;
                model_counts[i] = -1;
        }
    }
    
}

int get_model_port(){
    int lowest_port_count = INT_MAX;
    int lowest_port = -1;
    for(int i=0; i< MAX_NUM_MODEL_THREADS ; i++){
        if(lowest_port_count > model_counts[i]){
            lowest_port_count = model_counts[i];
            lowest_port = model_ports[i];
        }   
    }

    return lowest_port;
}



void cleanup(){
    delete threads;
    for(int i=0; i< MAX_NUM_MODEL_THREADS ; i++){
        int error;
        if(( error = pthread_cancel(model_threads[i])) != 0){
                handle_error_en(error, "pthread_create");
        }
    }

    
    close(master_socket); 
}


int main(int argc , char *argv[])   
{   

    std::atexit(cleanup);
    init_model_server();

    threads = new LinkedList();
    int opt = TRUE;   
    int addrlen , new_socket , activity, i , valread;   
    int max_sd;   
    int thread_count = 0;
     
         
    char buffer[MAX_RECV_BUFFER_SIZE];  //data buffer of 1K  
         
    //set of socket descriptors  
    fd_set readfds;   
         
     
     
         
    //create a master socket  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    //bind the socket to localhost port 8888  
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listener on port %d \n", PORT);   
         
    //try to specify maximum of 3 pending connections for the master socket  
    if (listen(master_socket, 3) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    puts("Waiting for connections ...");   
         
    while(TRUE)   
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        //add child sockets to set  
        for ( i = 0 ; i < threads->get_size() ; i++)   
        {   
            //socket descriptor 
            threadParams_t* sd_thread = threads->get_index(i); 
               
                 
            //if valid socket descriptor then add to read list  
            if(!sd_thread->isClientDone() && sd_thread->getSocketFd() > 0)   
                FD_SET( sd_thread->getSocketFd() , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(!sd_thread->isClientDone() && sd_thread->getSocketFd() > max_sd)   
                max_sd = sd_thread->getSocketFd();   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
             
                 
            

            threadParams_t* t = new threadParams_t(totalClientCount);
            totalClientCount += 1;
            t->setSocketFd(new_socket);
            t->setClientPort(address.sin_port);
            t->setClientAddr(inet_ntoa(address.sin_addr));
            threads->append(t);     

            printf("Stack_size:%d\n" , threads->get_size());
               
        }   
             
        //else its some IO operation on some other socket 
        for ( i = 0 ; i < threads->get_size() ; i++)   
        {   
            threadParams_t* sd_thread = threads->get_index(i); 
            //printf("recieved threads_obj: %d\n", sd_thread->getObjId());

            if(sd_thread == nullptr) break;
            
                 
            if (!sd_thread->isDoneRecv() && FD_ISSET( sd_thread->getSocketFd(), &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
               // printf("reading from socket\n");
                if ((valread = read( sd_thread->getSocketFd() , buffer, MAX_RECV_BUFFER_SIZE)) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd_thread->getSocketFd() , (struct sockaddr*)&address , (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    sd_thread->setClientDone(true);
                    sd_thread->setDoneRecv(true);   
                    close(sd_thread->getSocketFd());
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    
                    // for(int i = 0; i < valread ; i++){
                    //     printf("%c", buffer[i]);
                    // }
                    // printf("recieved Data: %d\n", valread);
                    sd_thread->concat_mobile_array(buffer, valread);
                   // printf("final thread array size: %d\n", sd_thread->getMobileBufferSize());
                    // printf("Last Character: %c\n", sd_thread->getLastChar() );
                    // printf("final Length: %d\n", sd_thread->getMobileBufferSize());
                    if(buffer[valread-1] == '-'){
                        sd_thread->setThreadIdx(thread_count);
                        sd_thread->setDoneRecv(true);
                        //sd_thread->setModelPort(5555);
                        sd_thread->setModelPort(get_model_port());
                        thread_count +=1;    

                        printf("send complete\n");
                        printf("Size of end string: %ld\n", sd_thread->getMobileBuffer().size());

                        //image_thread(sd_thread);

                        int error;
                        if(( error = pthread_create(sd_thread->getThread(), NULL, image_thread, (void *)(sd_thread))) != 0){
                                handle_error_en(error, "pthread_create");
                        }
                        printf("processing complete\n");

                    }
                }   
            }   
        }   
    }   
         
    return 0;   
}   