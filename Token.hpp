#include "config.hpp"
#include <zmq.hpp>
#include <deque>

class Token
{
    public:
        int * LN;
        std::deque<int> * q;
        // conditional dict
        // buffer dict

        Token();
        ~Token();

        static zmq::message_t * Serialize(Token * token);
        static Token * Deserialize(zmq::message_t * msg);
};