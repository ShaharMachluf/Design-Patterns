#include <mutex>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "guard.cpp"

int* ptr;

void* start_thread(void* mtx){
    Guard g{*((mutex*)mtx)};
    (*ptr)++;
    cout<<*ptr<<endl;
}

int main(){
    int x = 0;
    ptr = &x;
    mutex mtx;
    pthread_t t1;
    pthread_t t2;
    for(int i=0; i<15; i++){
        pthread_create(&t1, NULL, start_thread, (void*)&mtx);
        pthread_create(&t2, NULL, start_thread, (void*)&mtx);
        pthread_join(t1, NULL);
    }
    return 0;
}