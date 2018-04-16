#include "Monitor.hpp"
#include <iostream>

int main()
{
    Monitor monitor(1);
    
    std::cout<<"start"<<std::endl;
    monitor.Enter();
    std::cout<<"entered"<<std::endl;
    monitor.Signal("abc");
    monitor.PutBuffer("buf", "ala ma kota");
    monitor.Exit();

    std::cout<<"ok"<<std::endl;

    monitor.Enter();
    monitor.SignalAll("abc");
    std::cout<<"buffer: "<<monitor.GetBuffer("buf")<<std::endl;
    monitor.Wait("abc");
    std::cout<<"after wait"<<std::endl;
    monitor.Exit();
}