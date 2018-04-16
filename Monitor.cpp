#include "Monitor.hpp"


Monitor::Monitor(int i)
{
    machines = new std::string[NUMBER_OF_MONITORS] { MACHINES_STRINGS };

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

    context = new zmq::context_t(1);
    pull = new zmq::socket_t(*context, ZMQ_PULL);
    push = new zmq::socket_t(*context, ZMQ_PUSH);
    pub = new zmq::socket_t(*context, ZMQ_PUB);
    sub = new zmq::socket_t(*context, ZMQ_SUB);

    pull->bind("tcp://*:5555");
    pub->bind("tcp://*:5556");

    sub->setsockopt(ZMQ_SUBSCRIBE, "", 0);

    for (int j = 0; j < NUMBER_OF_MONITORS; ++j)
    {
        if (j != i)
        {
            sub->connect(GetMachineName(j));
        }
    }

   subThread = new std::thread(&Monitor::Subscriber, this);
}
Monitor::~Monitor()
{
    delete RN;
    if (HavePrivilege)
        delete token;
}

void Monitor::Enter()
{
    localMtx.lock();

    InSection = true;
    if (!HavePrivilege)
    {
        RN[i]++;

        struct Request r;
        r.procID = i;
        r.RN_i = RN[i];
        zmq::message_t msg(&r, sizeof(struct Request));
        push->send(msg);
        
        WaitingRoutine();
        HavePrivilege = true;
    }

    localMtx.unlock();
}
void Monitor::Exit()
{
    localMtx.lock();

    token->LN[i] = RN[i];
    for (int j = 0; j < NUMBER_OF_MONITORS; ++j)
    {
        if (j != i)
        {
            if (std::find(token->q.begin(), token->q.end(), j) == token->q.end()// machine not in the token queue 
            && RN[j] == token->LN[j] + 1)
            {
                token->q.push_back(j);
            }
        }
    }
    if (!token->q.empty())
    {
        int j = token->q.front();
        token->q.pop_front();
        HavePrivilege = false;
        std::string MachineName = GetMachineName(j);
        push->connect(MachineName);
        push->send(*Token::Serialize(token));
        push->disconnect(MachineName);

        delete token;
    }
    InSection = false;

    localMtx.unlock();
}
void Monitor::Wait(std::string condVarIdent)
{
    localMtx.lock();

    InSection = false;
    token->conditionalQueues[condVarIdent].push_back(i);
    WaitingRoutine();
    InSection = true;

    localMtx.unlock();
}
void Monitor::Signal(std::string condVarIdent)
{
    localMtx.lock();

    if (!token->conditionalQueues[condVarIdent].empty())
    {
        int j = token->conditionalQueues[condVarIdent].front();
        token->conditionalQueues[condVarIdent].pop_front();
        token->q.push_front(j);
    }

    localMtx.unlock();
}
void Monitor::SignalAll(std::string condVarIdent)
{
    localMtx.lock();

    while (!token->conditionalQueues[condVarIdent].empty())
    {
        int j = token->conditionalQueues[condVarIdent].front();
        token->conditionalQueues[condVarIdent].pop_front();
        token->q.push_front(j);
    }

    localMtx.unlock();
}

std::string Monitor::GetBuffer(std::string buffIdent)
{
    localMtx.lock();

    return token->buffers[buffIdent];

    localMtx.unlock(); 
}
void Monitor::PutBuffer(std::string buffIdent, std::string buffer)
{
    localMtx.lock();

    token->buffers[buffIdent] = buffer;

    localMtx.unlock();
}

void Monitor::WaitingRoutine()
{
    localMtx.unlock();

    zmq::message_t msg;
    pull->recv(&msg);
    token = Token::Deserialize(&msg);
    HavePrivilege = true;

    localMtx.lock();
}
void Monitor::Subscriber()
{
    while (1)
    {
        zmq::message_t msg;
        sub->recv(&msg);
        struct Request r = *((struct Request *)msg.data());


        localMtx.lock();

        RN[r.procID] = std::max(RN[r.procID], r.RN_i);
        if (HavePrivilege && !InSection)
        {
            HavePrivilege = false;
            std::string MachineName = GetMachineName(r.procID);
            push->connect(MachineName);
            push->send(*Token::Serialize(token));
            push->disconnect(MachineName);

            delete token;
        }

        localMtx.unlock();
    }
}

std::string Monitor::GetMachineName(int index)
{
    std::ostringstream oss;
    oss << "tcp://" << machines[index] << ":5556";
    return oss.str();
}