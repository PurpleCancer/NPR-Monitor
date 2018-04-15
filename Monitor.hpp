#include "IMonitor.hpp"
#include "Token.hpp"
#include <zmq.hpp>
#include <string>
#include <mutex>

class Monitor : public IMonitor
{
    private:
        int * RN;
        int i, j;
        bool HavePrivilege, InSection;
        Token * token;
        std::mutex localMtx;

        void WaitingRoutine();
        void Subscriber();
    public:
        Monitor(int i);

        virtual void Enter();
        virtual void Exit();
        virtual void Wait(std::string condVarIdent);
        virtual void Signal(std::string condVarIdent);
        virtual void SignalAll(std::string condVarIdent);

        virtual std::string ReadBuffer(std::string buffIdent);
        virtual void PutBuffer(std::string buffIdent, std::string buffer);
};