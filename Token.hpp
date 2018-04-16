#include "config.hpp"

class Token
{
    public:
        int * LN;
        std::deque<int> q;
        std::map<std::string, std::deque<int>> conditionalQueues;
        std::map<std::string, std::string> buffers;

        Token();
        ~Token();

        // to be implemented by the programmer
        static zmq::message_t * Serialize(Token * token);
        static Token * Deserialize(zmq::message_t * msg);
};