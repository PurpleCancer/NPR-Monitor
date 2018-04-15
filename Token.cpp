#include "Token.hpp"

Token::Token()
{
    LN = new int[NUMBER_OF_MONITORS];

    q = new std::deque<int>;
}

Token::~Token()
{
    delete LN;
    delete q;
}

zmq::message_t * Token::Serialize(Token * token)
{
    return new zmq::message_t();
}

Token * Token::Deserialize(zmq::message_t * msg)
{
    return new Token();
}