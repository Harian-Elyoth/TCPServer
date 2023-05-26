#include <csignal>
#include "TCPServer.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>

bool _isRunning = false;

void signalHandler(int signum){
    _isRunning = false;
}

int main(int argc, char const *argv[])
{
    signal(SIGINT, signalHandler);
    TCPServer tcpserver(20002);

    _isRunning = true;
    std::cout << "Running !" << std::endl;
    while(_isRunning){
        usleep(10);
    }
    std::cout << "Time to Stop !" << std::endl;
    tcpserver.stop();
    tcpserver.join();
    return 0;
}
