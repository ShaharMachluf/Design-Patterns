#include "singleton.cpp"
#include <mutex>
#include <fstream>
#include <iostream>

using namespace std;

int main(){
    ofstream MyFile("example.txt");
    MyFile.close();
    Singleton<ofstream> *s = Singleton<ofstream>::Instance(MyFile);
    ofstream MyFile2("example2.txt");
    MyFile2.close();
    Singleton<ofstream> *s2 = Singleton<ofstream>::Instance(MyFile2);
    cout<<"first singleton address: "<<s<<endl;
    cout<<"second singleton address: "<<s2<<endl;
    return 0;
}