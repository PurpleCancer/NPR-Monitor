#include "Token.hpp"

Token::Token()
{
    LN = new int[NUMBER_OF_MONITORS];
}

Token::~Token()
{
    delete LN;
}

zmq::message_t * Token::Serialize(Token * token)
{
    return new zmq::message_t();
}

Token * Token::Deserialize(zmq::message_t * msg)
{
    return new Token();
}