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

    context = new zmq::context_t(2 * NUMBER_OF_MONITORS);
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
            sub->connect(GetMachineName(j, 5556));
        }
    }

    // to receive shutdown request
    sub->connect("tcp://localhost:5556");

   subThread = new std::thread(&Monitor::Subscriber, this);
}
Monitor::~Monitor()
{
    delete RN;
    if (HavePrivilege)
        delete token;

    // send termination request to subscriber thread
    struct Request r;
    r.procID = i;
    r.RN_i = -1;
    zmq::message_t msg(&r, sizeof(struct Request));
    pub->send(msg);
    subThread->join();

    delete push;
    delete pull;
    delete pub;
    delete sub;
    delete context;
}

void Monitor::Enter()
{
    if (InSection)
        throw MonitorException();

    localMtx.lock();

    InSection = true;
    if (!HavePrivilege)
    {
        RN[i]++;

        struct Request r;
        r.procID = i;
        r.RN_i = RN[i];
        zmq::message_t msg(&r, sizeof(struct Request));
        pub->send(msg);
        
        WaitingRoutine();
        HavePrivilege = true;
    }

    localMtx.unlock();
}
void Monitor::Exit()
{
    if (!InSection)
        throw new MonitorException();

    localMtx.lock();

    DisposeOfToken();
    InSection = false;

    localMtx.unlock();
}
void Monitor::Wait(std::string condVarIdent)
{
    if (!InSection)
        throw new MonitorException();

    localMtx.lock();

    token->conditionalQueues[condVarIdent].push_back(i);

    DisposeOfToken();    
    InSection = false;
    
    WaitingRoutine();
    InSection = true;

    localMtx.unlock();
}
void Monitor::Signal(std::string condVarIdent)
{
    if (!InSection)
        throw new MonitorException();

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
    if (!InSection)
        throw new MonitorException();

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
    if (!InSection)
        throw new MonitorException();

    localMtx.lock();

    return token->buffers[buffIdent];

    localMtx.unlock(); 
}
void Monitor::PutBuffer(std::string buffIdent, std::string buffer)
{
    if (!InSection)
        throw new MonitorException();

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
void Monitor::DisposeOfToken()
{
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
        std::string MachineName = GetMachineName(j, 5555);
        push->connect(MachineName);
        push->send(*Token::Serialize(token));
        push->disconnect(MachineName);

        delete token;
    }
}
void Monitor::Subscriber()
{
    while (1)
    {
        zmq::message_t msg;
        sub->recv(&msg);
        struct Request r = *((struct Request *)msg.data());

        if (r.RN_i < 0 || r.procID == i)
        {
            if (r.RN_i < 0 && r.procID == i) //termination request from this monitor's main thread
                break;
            else //termination request from another monitor or normal request from this monitor's main thread
                continue;
        }

        //normal request from another monitor
        localMtx.lock();

        RN[r.procID] = std::max(RN[r.procID], r.RN_i);
        if (HavePrivilege && !InSection)
        {
            HavePrivilege = false;
            std::string MachineName = GetMachineName(r.procID, 5555);
            push->connect(MachineName);
            push->send(*Token::Serialize(token));
            push->disconnect(MachineName);

            delete token;
        }

        localMtx.unlock();
    }
}

std::string Monitor::GetMachineName(int index, int port)
{
    std::ostringstream oss;
    oss << "tcp://" << machines[index] << ":" << port;
    return oss.str();
}