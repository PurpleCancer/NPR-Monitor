#include <string>

class IMonitor
{
    public:
        virtual ~IMonitor() {};

        virtual void Enter() = 0;
        virtual void Exit() = 0;
        virtual void Wait(std::string condVarIdent) = 0;
        virtual void Signal(std::string condVarIdent) = 0;
        virtual void SignalAll(std::string condVarIdent) = 0;

        virtual std::string GetBuffer(std::string buffIdent) = 0;
        virtual void PutBuffer(std::string buffIdent, std::string buffer) = 0;
};