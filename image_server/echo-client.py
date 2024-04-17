#!/usr/bin/env python3

import socket
import cv2
import sys
import string

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8888  # The port used by the server

img = cv2.imread(sys.argv[1])
img_bytes = cv2.imencode('.jpg', img)

string_ = [chr(i) for i in img_bytes[1]] # outputs: ['b', 'o', 'o', 'k']

string_ = ''.join(string_)


output_str = ""
output_str += str(img.shape[0]) + " " + str(img.shape[1]) + " "
output_str += string_ + "\r"

print(output_str)
output_str = bytes(output_str, 'utf-8')


sys.getsizeof(output_str)

#output_str += img_bytes


#print(output_str)


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(output_str)
    #data = s.recv(1024)

#print(f"Received {data!r}")
