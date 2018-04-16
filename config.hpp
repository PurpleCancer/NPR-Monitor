#include <zmq.hpp>
#include <algorithm>
#include <deque>
#include <map>
#include <string>
#include <mutex>
#include <iostream>
#include <sstream>
#include <thread>

#define NUMBER_OF_MONITORS 2
#define MACHINES_STRINGS "lab1", "lab2"