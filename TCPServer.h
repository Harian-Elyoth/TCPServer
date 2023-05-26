#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_set>
#include <thread>
#include <vector>

class TCPServer;

struct thread_params {
    sockaddr_in address;
    socklen_t addrlen;
    int socket;
    unsigned short listeningPort;
    TCPServer * server;
};

class TCPServer {

public:

    TCPServer(unsigned short port);
    ~TCPServer();
    int init();
    int start();
    int stop();
    static int run(TCPServer * server);
    static void startThreadCommunication(thread_params parameters){
        TCPServer *server = parameters.server;
        return server->communicationHandler(parameters);
    }
    inline void join(){
        _runningThread.join();
    }

private:

    // Connection
    unsigned short _connectionPort;
    int _connectionSocket;
    sockaddr_in _address;

    // Thread Management
    static void communicationHandler(thread_params);
    std::vector<std::thread> _comThreads;
    bool _isRunning;
    std::thread _runningThread;
    // Attributes
    unsigned short _nbConnections;
    static unsigned int _idGenerationNb;
};