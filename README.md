# NPR-Monitor

Distributed monitor implemented in C++ using ØMQ. Provides an interface exposing classic monitor operations, such as Enter, Exit, Wait, Signal and SignalAll, while also providing a buffer exchange between workstations. Conditional variables and buffers are identified using std::string names.

Uses modified Suzuki-Kasami token based approach, where queues of processes waiting on conditional variables and buffers are send together with the token to the process in critical section.

# Compilation remarks

Project created used ØMQ 3 and cppzmq wrapper for C++. Compile with g++ using -std=c++11 -lzmq -pthread command line options.
