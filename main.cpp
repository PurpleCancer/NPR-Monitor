#include "Monitor.hpp"
#include <iostream>
#include <unistd.h>

int main(int argc, char * argv[])
{
    if (argc < 2)
        return 1;
    Monitor monitor(std::stoi(argv[1]));

    sleep(2);
    
    std::cout<<"start"<<std::endl;
    monitor.Enter();
    std::cout<<"entered"<<std::endl;
    monitor.Signal("abc");
    monitor.PutBuffer("buf", "ala ma kota");
    monitor.Exit();

    while(1);

    monitor.Enter();
    monitor.SignalAll("abc");
    std::cout<<"buffer: "<<monitor.GetBuffer("buf")<<std::endl;
    monitor.Wait("abc");
    std::cout<<"after wait"<<std::endl;
    monitor.Exit();

    return 0;
}