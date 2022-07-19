#include <mutex>
#include <fstream>
#include <iostream>

template <typename T> class Singleton{
    std::mutex mtx;
    T *_data;
    static Singleton* instance; 
    Singleton(T &data){
        mtx.lock();
        _data = &data;
    }
    public:
    static Singleton* Instance(T& data);
    void Destroy();
};

template <typename T>
Singleton<T>* Singleton<T>::instance = nullptr;

template <typename T>
Singleton<T>* Singleton<T>::Instance(T& data){
    if(instance == nullptr){
        instance = new Singleton(data);
    }
    return instance;
}

template <typename T>
void Singleton<T>::Destroy(){
    instance = nullptr;
    mtx.unlock();
}

