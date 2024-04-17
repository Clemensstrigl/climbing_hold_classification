#ifndef THREAD_UTILS_H  
#define THREAD_UTILS_H
#include <stdio.h>
#include <string.h>
#include <cwchar>

#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

class threadParams_t{

    private:
        int obj_id = -1;
        pthread_t thread;
        int threadIdx = -1;
        int socket_fd = 0;
        int mobile_buffer_size = 0;
        int send_buffer_size = 0;
        std::string mobile_buffer;
        std::string send_buffer;
        bool send_buffer_ready = false;
        bool done_recv = false;
        bool client_done = false;
        uint16_t client_port = -1;
        int model_port = -1;
        char* client_addr;
    public:

    threadParams_t(int id): obj_id(id) {}
    ~threadParams_t(){
        close(socket_fd);
        int error;
        if(( error = pthread_cancel(thread)) != 0){
                handle_error_en(error, "pthread_create");
        }

    }




    char32_t NextUTF8Char(const char*& str)
    {
        static const int max_utf8_len = 5;
        auto s = reinterpret_cast<const char*>(str);
        wchar_t c;
        std::mbstate_t state;
        auto len = std::mbrtowc(&c, s, max_utf8_len, &state);
        if (len > max_utf8_len) { return 0; }
        str += len;
        return c;
    }


   void concat_mobile_array(const char *b, int size_b){
        //int final_size = mobile_buffer_size + size_b ;
       // char32_t c;
       // std::size_t i = 0;

    // //     while ((c = NextUTF8Char(b)) != 0) {
    // //    // std::cout << '[' << i++ << "] = " << (std::uint_fast32_t)c << '\n';
    // //     // c is our utf-8 character in unsigned int format
    // //     mobile_buffer += c;
    // //     }
       
    //    std::string s = b;
        for(int i = 0; i < size_b; i++){
            mobile_buffer += b[i];
        }


        //    for(int i = 0; i < mobile_buffer.size(); ++i){
        //      printf("%c", mobile_buffer[i]);
        //    }
    //    printf("Size: %ld\n", mobile_buffer.size());
    //     for(int i = 10; i > 1; i--)
    //         printf("Last character: %c\n", mobile_buffer[mobile_buffer_size - i]);

        mobile_buffer_size += size_b;

    }     

   void concat_send_array(const char *b, int size_b){
        int final_size = mobile_buffer_size + size_b ;
       
        std::string s = b;

        send_buffer += s;
        
        

        send_buffer_size = final_size;

    }

    void setModelPort(int port){
        model_port = port;
    }

    int getModelPort() const { 
        return model_port; 
    }

    int getObjId() const{
        return obj_id;
    }

    char* getClientAddr() const {
        return client_addr;
    }

    void setClientAddr(char* addr) { 
        client_addr = addr; 
    }

    uint16_t getClientPort() const{
        return client_port;
    }

    void setClientPort(uint16_t port) { 
        client_port = port; 
    }

    pthread_t* getThread() {
        return &thread;
    }

    int getThreadIdx() const {
        return threadIdx;
    }

    void setThreadIdx(int value) {
        threadIdx = value;
    }

    // Getter and Setter for socket_fd
    int getSocketFd() {
        return socket_fd;
    }

    void setSocketFd(int value) {
        socket_fd = value;
    }

    // Getter and Setter for mobile_buffer_size
    int getMobileBufferSize() const {
        return mobile_buffer_size;
    }

    void setMobileBufferSize(int value) {
        mobile_buffer_size = value;
    }

    // Getter and Setter for send_buffer_size
    int getSendBufferSize() const {
        return send_buffer_size;
    }

    void setSendBufferSize(int value) {
        send_buffer_size = value;
    }

    // Getter and Setter for mobile_buffer
   const std::string& getMobileBuffer() const {
        return mobile_buffer;
    }

    char getLastChar() const {
        return mobile_buffer[mobile_buffer_size - 1];
    }

    void setMobileBuffer(std::string value) {
        mobile_buffer = value;
    }

    // Getter and Setter for send_buffer
    std::string getSendBuffer() const {
        return send_buffer;
    }

    void setSendBuffer(std::string value) {
        send_buffer = value;
    }

    // Getter and Setter for send_buffer_ready
    bool isSendBufferReady() const {
        return send_buffer_ready;
    }

    void setSendBufferReady(bool value) {
        send_buffer_ready = value;
    }

    // Getter and Setter for done_recv
    bool isDoneRecv() const {
        return done_recv;
    }

    void setDoneRecv(bool value) {
        done_recv = value;
    }

    // Getter and Setter for client_done
    bool isClientDone() const {
        return client_done;
    }

    void setClientDone(bool value) {
        client_done = value;
    }  
 

};



#endif