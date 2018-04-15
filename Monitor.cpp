#include "Monitor.hpp"
#include "config.hpp"
#include <string>
#include <iostream>


Monitor::Monitor(int i)
{
    RN = new int[NUMBER_OF_MONITORS];
    this->i = i;
    this->InSection = false;

    if (i == 0)
    {
        this->HavePrivilege = true;
        this->token = new Token();
    }
    else
    {
        this->HavePrivilege = false;
    }
}

void Monitor::Enter()
{
    localMtx.lock();
    InSection = true;
    if (!HavePrivilege)
    {
        RN[i]++;
        // send requests
        WaitingRoutine();
        HavePrivilege = true;
    }

}
void Monitor::Exit()
{

}
void Monitor::Wait(std::string condVarIdent)
{

}
void Monitor::Signal(std::string condVarIdent)
{

}
void Monitor::SignalAll(std::string condVarIdent)
{

}

std::string Monitor::ReadBuffer(std::string buffIdent)
{
        
}
void Monitor::PutBuffer(std::string buffIdent, std::string buffer)
{
        
}

void Monitor::WaitingRoutine()
{

}
void Monitor::Subscriber()
{

}