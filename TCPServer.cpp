#include "TCPServer.h"
#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <string>
#include <utility>
#include <cstring>
#include <cctype>
#include <functional>
#include <errno.h>

TCPServer::TCPServer(unsigned short port) : 
_connectionPort(port),
_isRunning(false),
_nbConnections(0)
{
    _runningThread = std::thread(TCPServer::run, this);
}

TCPServer::~TCPServer()
{
}

int TCPServer::run(TCPServer * server){

    if(server->init() < 0){
        std::cerr << "TCPServer failed to initialize for previous reasons" << std::endl;
        return -1;
    }
    if(server->start() < 0){
        return -1;
    }
    std::cout << "End Run" << std::endl;
    return 0;
}

int TCPServer::init()
{
    if((_connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        std::cerr << "Socket creation failed : " << strerror(errno) << std::endl;
        return -1;
    }
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_connectionPort);
    if(bind(_connectionSocket, (sockaddr*) &_address, sizeof(_address)) < 0){
        std::cerr << "Socket binding failed : " << strerror(errno) << std::endl;
        return -1;
    }
    return 0;
}

void TCPServer::communicationHandler(thread_params threadParameters){
    // Do something for the client
    while(threadParameters.server->_isRunning) {
        usleep(100000);
        
        if(send(threadParameters.socket, "Hi\n", sizeof("Hi\n"), 0) < sizeof("Hi\n")){
            break;
        }
    }
    std::cout << "Bye" << std::endl;
    send(threadParameters.socket, "Bye\n", sizeof("Bye\n"), 0);
    close(threadParameters.socket);
    return;
}


int TCPServer::start()
{
    std::cout << "ADRESS OF THE SERVER START" << this << std::endl;
    if(listen(_connectionSocket, 6) < 0){
        std::cerr << "Socket listening failed : " << strerror(errno) << std::endl;
        return -1;
    }
    _isRunning = true;
    while (_isRunning){
        usleep(10);
        sockaddr_in newClientAddr;
        int newClientAddrLen = sizeof(newClientAddr);
        int newSocket;
        if((newSocket = accept(_connectionSocket, (sockaddr*)&newClientAddr, (socklen_t*)&newClientAddrLen)) < 0 && _isRunning){
            std::cerr << "Socket accepting failed : " << strerror(errno) << std::endl;
            return -1;
        }
        else {
            char ipAddress[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(newClientAddr.sin_addr), ipAddress, INET_ADDRSTRLEN);
            // Accepting the client
            std::cout << "Client connected" << std::endl;
            thread_params params;
            params.address = newClientAddr;
            params.addrlen = sizeof(newClientAddr);
            params.socket = newSocket;
            params.server = this;
            _comThreads.push_back(std::thread(startThreadCommunication, params));
            // Creating the communication thread
        }
    }
    std::cout << "End start" << std::endl;
    return 0;
}

int TCPServer::stop(){
    std::cout << "Stop !" << std::endl;
    _isRunning = false;
    for(unsigned int i = 0; i < _comThreads.size(); i++){
        _comThreads[i].join();
    }
    close(_connectionSocket);
    return 0;
}