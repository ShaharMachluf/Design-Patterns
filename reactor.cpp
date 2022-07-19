/*
** pollserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <vector>
#include <functional>

using namespace std;

typedef struct Reactor{
    struct pollfd *pfds;
    int fd_count;
    int fd_size;
    vector<std::function<void*(void*)>> funcs;
}reactor;

void* accept(void* fd){
    char buf[256];
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    read(*(int*)fd, buf, sizeof(buf));
    return NULL;
}

void* recv(void* fd){
    char buf[256];
    read(*(int*)fd, buf, sizeof(buf));
    if (send(*(int*)fd, "sent\n", sizeof("sent\n"), 0) == -1) {
        perror("send");
    }
    return NULL;
}

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *pfds = (struct pollfd*)realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;

}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

void* react(void* r){
    int poll_count;
    reactor *re = (reactor*)r;
    while(true){
        poll_count = poll(re->pfds, re->fd_count, 2);
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }
        if(poll_count > 0){
             for(int i=0; i<re->fd_count; i++){
                if(re->pfds->revents & POLLIN){
                    re->funcs[i](&re->pfds[i].fd);
                }
            }
        }
    }
}

void* newReactor(){
    reactor *r = new reactor;
    r->fd_count = 0;
    r->fd_size = 5;
    r->pfds = (pollfd*)malloc((sizeof(*r->pfds)) * (r->fd_size));
    pthread_t thread;
    pthread_create(&thread, NULL, react, (void*)r);
 return r;
}

void installHandler(reactor *r, void* (*func)(void*), int *fd){
    add_to_pfds(&r->pfds, *fd, &r->fd_count, &r->fd_size);
    r->funcs.push_back(func);
}

void removeHandler(struct pollfd pfds[], int i, int *fd_count){
    del_from_pfds(pfds, i, fd_count);
}
