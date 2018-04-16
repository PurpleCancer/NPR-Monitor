#include "IMonitor.hpp"
#include "Token.hpp"
#include "Request.hpp"
#include "config.hpp"

class Monitor : public IMonitor
{
    public:
        std::string * machines;

        int * RN;
        int i;
        bool HavePrivilege, InSection;
        Token * token;
        std::mutex localMtx;

        zmq::context_t * context;
        // privilege
        zmq::socket_t * push, * pull;
        // request
        zmq::socket_t * pub, * sub;

        std::thread * subThread;

        void WaitingRoutine();
        void Subscriber();

        std::string GetMachineName(int index);
    public:
        Monitor(int i);
        ~Monitor();

        virtual void Enter();
        virtual void Exit();
        virtual void Wait(std::string condVarIdent);
        virtual void Signal(std::string condVarIdent);
        virtual void SignalAll(std::string condVarIdent);

        virtual std::string GetBuffer(std::string buffIdent);
        virtual void PutBuffer(std::string buffIdent, std::string buffer);
};