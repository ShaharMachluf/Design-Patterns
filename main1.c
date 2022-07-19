#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct Node{
    void* data;
    struct Node* next;
}node, *pnode;

typedef struct Queue{
    pnode start;
    pnode end; 
    pthread_cond_t cond;
    pthread_mutex_t mutex;
}queue;

// static queue *q1;//
// static queue *q2;//
// static queue *q3;//
// static queue qu1;
// static queue qu2;
// static queue qu3;

pnode new_node(void* data){
    pnode n = (pnode)malloc(sizeof(node));
    n->data= data;
    n->next = NULL;
    return n;
}

queue createQ(){
    queue *my_q = (queue*)malloc(sizeof(queue));
    my_q->start = NULL;
    my_q->end = my_q->start;
    pthread_mutex_init(&my_q->mutex, NULL);
    if (pthread_cond_init(&my_q->cond, NULL) != 0) {                                    
        perror("pthread_cond_init() error");                                        
        exit(1);                                                                    
    } 
    return *my_q;
}

void destroyQ(queue *my_queue){
    if (pthread_cond_destroy(&my_queue->cond) != 0) {                                       
        perror("pthread_cond_destroy() error");                                     
        exit(2);                                                                    
    }  
    pthread_mutex_destroy(&my_queue->mutex);
    pnode temp = NULL;
    while(my_queue->start != NULL){
        temp = my_queue->start;
        my_queue->start = (my_queue->start)->next;
        free(temp);
    }
    free(my_queue);
}

void* enQ(void* data, void *q){
    queue *my_q = (queue*)q;
    pthread_mutex_lock(&my_q->mutex);
    pnode n = new_node(data);
    if(my_q->start == NULL){
        my_q->start = n;
        my_q->end = n;
        pthread_cond_signal(&my_q->cond);
    }
    else{
        (my_q->end)->next = n;
        my_q->end = (my_q->end)->next;
    }
    pthread_mutex_unlock(&my_q->mutex);
    return NULL;
}

void* deQ(queue *my_q, void** ret){
    pthread_mutex_lock(&my_q->mutex);
    if(my_q->start == NULL){
        pthread_cond_wait(&my_q->cond, &my_q->mutex);
    }
    *ret=(my_q->start)->data;
    pnode temp = my_q->start;
    if(my_q->start == my_q->end){
        my_q->start = NULL;
        my_q->end = my_q->start;
    }
    else{
        my_q->start = (my_q->start)->next;
    }
    free(temp);
    pthread_mutex_unlock(&my_q->mutex);
    return NULL;
}

// typedef struct active_object{ //part 2
//     queue* q;
//     void* (*func1)(void*);
//     void* (*func2)(void*);
// }AO; 

typedef struct active_object{ //part3
    queue* q;
    void* (*func1)(void*);
    void* (*func2)(void*, void*);
}AO;

typedef struct for_thread{
    AO *ao;
    queue *next_q;
    int *socket;
}t;

AO* newAO(queue* q, void* (*func1)(void*), void* (*func2)(void*, void*)){
    AO *curr = (AO*)malloc(sizeof(AO));
    curr->q = q;
    curr->func1 = func1;
    curr->func2 = func2;
    return curr;
}

void destroyAO(AO *curr){
    destroyQ(curr->q);
    free(curr);
    pthread_cancel(pthread_self());
}

void* cap(void* in){
    char* input = (char*)in;
    int i=0;
    while(input[i] != 0){
        if(input[i]>='a' && input[i] <= 'z'){
            input[i]-=32;
        }
        else if(input[i]>='A' && input[i] <= 'Z'){
            input[i]+=32;
        }
        i++;
    }
    return input;
}

void* caesar(void* in){
    char *input = (char*) in;
    int i=0;
    while(input[i] != 0){
        if(input[i]>='a' && input[i] <= 'y'){
            input[i]++;
        }
        else if(input[i]>='A' && input[i] <= 'Y'){
            input[i]++;
        }
        else if(input[i] == 'z'){
            input[i] = 'a';
        }
        else if(input[i] == 'Z'){
            input[i] = 'A';
        }
        i++;
    }
    return input;
}

char* get_data(int sock, queue *q){
    char data[1024];
    memset(data, 0, 1024);
    recv(sock, data, 1024, 0);
    enQ(data, q);
}

void* send_data(void* d, void* s){
    int sock = *(int*)s;
    char* data = (char*)d;
    if (send(sock, data, strlen(data)*sizeof(char), 0) == -1)
        perror("send");
}

void* dummy(void* d){
    return d;
}

void* activate(void *ft){
    t *curr = (t*)ft;
    AO* ao = curr->ao;
    char res[1024];
    void* ans;
    while(1){
        deQ(ao->q, &ans);
        strcpy(res,(char*)ao->func1(ans));
        ao->func2((void*)res, (void*)curr->next_q);
    }
}

void* activate2(void *ft){
    t *curr = (t*)ft;
    AO* ao = curr->ao;
    char res[1024];
    void* ans;
    while(1){
        deQ(ao->q, &ans);
        strcpy(res,ao->func1(ans));
        ao->func2((void*)res, (void*)curr->socket);
    }
}

// void *start_thread(void* fd){
//     int *sock = (int*)fd; 
//     qu1 = createQ();
//     q1 = &qu1;
//     AO* ao1 = newAO(q1, caesar, enQ); 
//     qu2 = createQ();
//     q2 = &qu2;
//     AO* ao2 = newAO(q2, cap, enQ); 
//     qu3 = createQ();
//     q3 = &qu3;
//     AO* ao3 = newAO(q3, dummy, send_data); 
//     t t1 = {ao1, q2, sock};
//     t t2 = {ao2, q3, sock};
//     t t3 = {ao3, q2, sock};
//     pthread_t thread1;
//     pthread_t thread2;
//     pthread_t thread3;
//     pthread_create(&thread1, NULL, activate, (void*)&t1);
//     pthread_create(&thread2, NULL, activate, (void*)&t2);
//     pthread_create(&thread3, NULL, activate2, (void*)&t3);
//     while(1){
//         get_data(*sock, q1);
//         sleep(3);
//     }
// }

