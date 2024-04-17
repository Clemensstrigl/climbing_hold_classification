#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT "5432"
#define MAX_LINE 256
#define MAX_PENDING 5
#define MAX_FILES 10
#define MAX_FILENAME_LEN 255

struct peer_entry {
  uint32_t id; // ID of peer
  int sd;      // Socket descriptor for connection to peer
  char files[MAX_FILES][MAX_FILENAME_LEN]; // Files published by peer
  int file_c;
  struct sockaddr_in address; // Contains IP address and port number
};

/*
 * Create, bind and passive open a socket on a local interface for the provided
 * service. Argument matches the second argument to getaddrinfo(3).
 *
 * Returns a passively opened socket or -1 on error. Caller is responsible for
 * calling accept and closing the socket.
 */
int sendMessage( char *msg, int size, int s);
int bind_and_listen(const char *service);
int part_recieve(int socket,  char *finalBuf, int *length);
int join(int socket, struct peer_entry **peers, int *numPeers,
         struct sockaddr *remoteaddr,  char *buf, int length);
int publish(struct peer_entry *peer,  char *buf, int length);
int search(struct peer_entry **peers, int numPeers,  char *buf,
           int length);
int getAction(int s, char *buf, int length, struct peer_entry **peers,
              int *numPeers);
int alreadyAPeer(struct peer_entry **peers, int numPeers, int s);
/*
 * Return the maximum socket descriptor set in the argument.
 */
int find_max_fd(const fd_set *fs);

int main(void) {
  // all_sockets stores all active sockets
  // call_set is a temporary used for the select call
  printf("hi\n");
  fd_set all_sockets, call_set;
  FD_ZERO(&all_sockets);
  struct sockaddr_storage remoteaddr;
  char buf[256];
  socklen_t addrlen;
  int newfd;
  int j;

  struct peer_entry *peers[5];
  int numPeers = 0;

  // listen_socket is the fd on which the program can accept() new connections
  int listen_socket = bind_and_listen(SERVER_PORT);
  FD_SET(listen_socket, &all_sockets);
  printf("bound to socket: %d\n", listen_socket);
  // max_socket should always contain the socket fd with the largest value
  int max_socket = listen_socket;

  while (1) {
    call_set = all_sockets;
    printf("waiting for peer\n");
    int num_s = select(max_socket + 1, &call_set, NULL, NULL, NULL);
    if (num_s < 0) {
      perror("ERROR in select() call\n");
      return -1;
    }
    // Skip standard IN/OUT/ERROR -> start at 3
    for (int s = 3; s <= max_socket; ++s) {
      // Skip sockets that aren't ready
      if (!FD_ISSET(s, &call_set))
        continue;

      // A new connection is available
      if (s == listen_socket) {
        printf("found new connection\n");
        addrlen = sizeof remoteaddr;
        newfd = accept(listen_socket, (struct sockaddr *)&remoteaddr, &addrlen);
        if (newfd == -1)
          perror("accept\n");
        else {
          FD_SET(newfd, &all_sockets);
          if (newfd > max_socket) {
            max_socket = newfd;
          }
          printf("Socket %d connected\n", s);
        }
      } else {
        int nbytes;
        char *finalBuf;
        int length = 0;
        if ((nbytes = part_recieve(s, finalBuf, &length)) <= 0) {
          if (nbytes == 0) {
            // connection closed
            printf("Socket %d closed\n", s);
          } else {
            perror("recv");
          }
          close(s); // bye!
          printf("Bye from Socket: %d\n", s);
          FD_CLR(s, &all_sockets);
        }
        if (length > 0 &&
            FD_ISSET(s, &all_sockets)) { // for(j = 0; j <= max_socket; j++) {
          printf("Socket %d send: %s\n", s, finalBuf);
          int action;
          //}
        }
      }
    }
  }
  return 0;
}

int getAction(int s,  char *buf, int length, struct peer_entry **peers,
              int *numPeers) {
  if (length <= 0) {
    perror("invalid Command\n");
    return -1;
  }
  // -1 if not exist, else index of peer
  int p = alreadyAPeer(peers, *numPeers, s);
  if ((int)buf[0] == 0) {
    if (p == -1) {
      // join(s)
    } else {
      // already exists
    }
  } else if ((int)buf[0] == 1) {
    // only publish if peer has joined.
    if (p != -1) {
      publish(peers[p], buf, length);
    }
  } else if ((int)buf[0] == 2) {
    if (p != -1) {
      search(peers, *numPeers, buf, length);
    }
  } else {
    printf("recieved invalid action!\n");
  }
  return (int)buf[0];
}

int join(int socket, struct peer_entry **peers, int *numPeers,
         struct sockaddr *remoteaddr,  char *buf, int length) {
  if (*numPeers < 5) {
    if (length != 5) {
      perror("invalid JOIN Command\n");
      return -1;
    }
    peers[*numPeers] = (struct peer_entry *)malloc(sizeof(struct peer_entry));
    peers[*numPeers]->address = *(struct sockaddr_in *)remoteaddr;
    peers[*numPeers]->id =
        (buf[1] << 24) | (buf[2] << 16) | (buf[3] << 8) | (buf[4]);
    peers[*numPeers]->sd = socket;
    peers[*numPeers]->file_c = 0;
    printf("JOIN %d\n", peers[*numPeers]->id);
    *numPeers = *numPeers + 1;
    return 1;
  } else {
    perror("MAX PEERS\n");
    return -1;
  }
  return 0;
}

int search(struct peer_entry **peers, int numPeers, char *buf, int length) {

  int p, p_files;

  for (p = 0; p < numPeers; p++) {
    for (p_files = 0; p_files < peers[p]->file_c; p_files++) {
      if (!strcmp(&buf[2], peers[p]->files[p_files])) {
        printf("Search match: %s   ==  %s\n", &buf[2],
               peers[p]->files[p_files]);
        char *ip = inet_ntoa(peers[p]->address.sin_addr);
        uint16_t port = htons(peers[p]->address.sin_port);
        printf(" SEARCH %s %d %s:%d\n", &buf[2], peers[p]->id, ip, port);
        char *msg = (char *)malloc(10);
        msg[0] = (peers[p]->id >> 24) & 0xFF;
        msg[1] = (peers[p]->id >> 16) & 0xFF;
        msg[2] = (peers[p]->id >> 8) & 0xFF;
        msg[3] = peers[p]->id & 0xFF;
        memcpy(&msg[4], ip, INET_ADDRSTRLEN);
        memcpy(&msg[8], &port, sizeof(port));
        sendMessage(msg, 10, peers[p]->sd);
        return 1;
      }
    }
  }
  printf(" SEARCH %s 0 0.0.0.0:0\n", &buf[2]);
  return 0;
}


// for peer, pass the peer entry * directly to function (peers[index])
int publish(struct peer_entry *peer, char *buf, int length) {
  int filecount = (buf[1] << 24) + (buf[2] >> 16) + (buf[3] >> 8) + buf[4];
  int findex = 0; // index for files in peer entry
  int bindex = 5; // index marking start of filemname in buffer
  // loop through buffer, concact to peer->file when null is found from bindex,
  // update bindex to position after null
  for (int i = 5; i < length; i++) {
    if (buf[i] == NULL) {
      strcat(peer->files[findex], buf + bindex);
      findex++;
      bindex = i + 1;
    }
  }
  if (findex != filecount) {
    return -1; // if did not recieve amount of files specified by peer
  }
  return 0;
}

int find_max_fd(const fd_set *fs) {
  int ret = 0;
  for (int i = FD_SETSIZE; i >= 0 && ret == 0; --i) {
    if (FD_ISSET(i, fs)) {
      ret = i;
    }
  }
  return ret;
}

int bind_and_listen(const char *service) {
  struct addrinfo hints;
  struct addrinfo *rp, *result;
  int s;

  /* Build address data structure */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0;

  /* Get local address info */
  if ((s = getaddrinfo(NULL, service, &hints, &result)) != 0) {
    fprintf(stderr, "stream-talk-server: getaddrinfo: %s\n", gai_strerror(s));
    return -1;
  }

  /* Iterate through the address list and try to perform passive open */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    if ((s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
      continue;
    }

    if (!bind(s, rp->ai_addr, rp->ai_addrlen)) {
      break;
    }

    close(s);
  }
  if (rp == NULL) {
    perror("stream-talk-server: bind");
    return -1;
  }
  if (listen(s, MAX_PENDING) == -1) {
    perror("stream-talk-server: listen");
    close(s);
    return -1;
  }
  freeaddrinfo(result);
  printf("binded to socket: %d", s);
  return s;
}

int part_recieve(int socket, char *finalBuf, int *length) {
  // code to recieve and also handle partial recv.  on first recv handles first
  // byte as the action response sent by peer. firstRec checks for this
  int bytesRecv = 0, bufsize = 200, finalBufSize = 0;
  *finalBuf = malloc(sizeof( char) * 10);
   char *recvBuf = malloc(sizeof( char) * bufsize);
  bool firstRec = true;
  do {
    bytesRecv = 0;
    bytesRecv = recv(socket, recvBuf, bufsize, 0);

    if (firstRec == true) {
      u_int8_t responseAction =
          recvBuf[0]; // possibly unnecessary? never recieved an error from
                      // solution to test
      if (responseAction != 0) {
        printf("Error sent by solution peer!\n");
        return -1;
      }
    }
    if (bytesRecv < 0) {
      perror("recieved error!\n");
      return -1;
    } else if (bytesRecv > 0) {
       char *tempBuf = malloc(sizeof(char) * finalBufSize);
      if (firstRec == true) {
        finalBuf = realloc(finalBuf, finalBufSize + bytesRecv);
        memcpy(finalBuf + finalBufSize, &recvBuf[1], bytesRecv - 1);
        finalBufSize = finalBufSize + bytesRecv - 1;
        firstRec = false;
      } else {
        memcpy(tempBuf, finalBuf, finalBufSize);
        finalBuf = realloc(finalBuf, finalBufSize + bytesRecv);
        memcpy(finalBuf, tempBuf, finalBufSize);
        memcpy(finalBuf + finalBufSize, recvBuf, bytesRecv);
        finalBufSize = finalBufSize + bytesRecv;
        free(tempBuf);
      }
    }
  } while (bytesRecv > 0);

  // // Write to file. creates file if it does not exist, erases contents if it
  // // does.
  // size_t bytesWritten = 0;
  // FILE *f1 = fopen(filename, "wb+");
  // if (f1) {
  //   bytesWritten = fwrite(finalBuf, sizeof finalBuf[0], finalBufSize, f1);
  //   fclose(f1);
  //   printf("%s Fetched! Wrote %zu bytes!\n", filename, bytesWritten);
  // }
  *length = finalBufSize;
  free(recvBuf);
  return 0;
}

int alreadyAPeer(struct peer_entry **peers, int numPeers, int s) {
  int p;
  for (p = 0; p < numPeers; p++) {
    if (s == peers[p]->sd)
      return p;
  }
  return -1;
}

int sendMessage(char *msg, int size, int s) {
  int total = 0;
  int bytsSent = 0;
  int bytsLeft = size;
  do {
    bytsSent = send(s, msg + total, bytsLeft, 0);
    if (bytsSent < 0) {
      free(msg);
      return 0; // if error
    }
    total = total + bytsSent;
    bytsLeft = bytsLeft - bytsSent;
  } while (total < size);
  free(msg);
  msg = NULL;
  return 1;
}
