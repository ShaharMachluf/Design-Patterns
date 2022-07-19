#include <mutex>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

class Guard{
    mutex *my_mtx;
    public:
    Guard(mutex &mtx){
        my_mtx = &mtx;
        (*my_mtx).lock();
        cout<<"guard started"<<endl;
    }
    ~Guard(){
        (*my_mtx).unlock();
        cout<<"guard ended"<<endl;
    }
};
