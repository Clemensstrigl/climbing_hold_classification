#ifndef LINKED_LIST_H  
#define LINKED_LIST_H

#include <iostream>
#include "thread_utils.h"


class Node {
public:
    threadParams_t* data;
    Node* next;

    Node(threadParams_t* value) : data(value), next(nullptr) {}
    ~Node(){
        delete data;
    }
};


class LinkedList {
private:
    Node* head;
    int size;

public:
    LinkedList() : head(nullptr), size(0) {}

    // Function to add a new node to the end of the linked list
    void append(threadParams_t* value) {
        Node* newNode = new Node(value);

        if (head == nullptr) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }

        size += 1;
    }

    int get_size() { return size; }

    threadParams_t* get_index(int index){
        if (index >= size) {
            return nullptr;
        }
        
        Node* temp = head;
        for(int i = 1; i < index+1;i++){
            temp = temp->next;
        }

        return temp->data;
    }

    bool pop_thread(int value){

        if(head == nullptr){
            return false;
        }


        Node* temp = head;
        if(temp->data->getThreadIdx() == value){
            head = head->next;
            size -= 1;
            delete temp;
            return true;
        }

        while(temp->next != nullptr){
            if(temp->next->data->getThreadIdx() == value){
                Node* t = temp->next;
                temp->next = t->next;
                size -= 1;
                delete temp->next;
                return true;
            }
        }
        
        return false;
        
    }

    bool pop_socket(int value){

        if(head == nullptr){
            return false;
        }


        Node* temp = head;
        if(temp->data->getSocketFd() == value){
            head = head->next;
            size -= 1;
            delete temp;
            return true;
        }

        while(temp->next != nullptr){
            if(temp->next->data->getSocketFd() == value){
                Node* t = temp->next;
                temp->next = t->next;
                size -= 1;
                delete temp->next;
                return true;
            }
        }
        
        return false;
        
    }


    threadParams_t* get_thread(int thread_Idx){
        

            Node* temp = head;
            while(temp != nullptr){
                if(temp->data->getThreadIdx() == thread_Idx){
                    return temp->data;
                }
                temp = temp->next;
            }
            
            return nullptr;
    }    

    threadParams_t* get_socket(int socket){

            Node* temp = head;
            while(temp != nullptr){
                if(temp->data->getSocketFd() == socket){
                    return temp->data;
                }
                temp = temp->next;

            }
            
            return nullptr;
    }    



    // Destructor to free memory when the object is destroyed
    ~LinkedList() {
        Node* current = head;
        Node* next;
        while (current != nullptr) {
            next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }
};


#endif
