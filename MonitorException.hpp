#include <exception>

struct MonitorException : public std::exception
{
    virtual const char * what() const throw()
    {
        return "Invalid operation inside of Monitor object.";
    }
};