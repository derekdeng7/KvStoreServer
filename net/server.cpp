#include <iostream>
#include <cassert>
#include <unistd.h>

#include "server.hpp"

namespace Network{

    Server::Server(uint16_t port):port_(port), buffer_size(1024)
    {}

    Server::~Server()
    {}

    void Server::Start()
    {
        assert(socket_.Create() && socket_.SetReuseAddress() &&
               socket_.Bind(port_) && socket_.Listen());
        
        std::cout << "start to listen, waiting for connection..." << std::endl;
    }

    void Server::Close()
    {
        socket_.Close();
    }

    int Server::Accept()
    {
        return socket_.Accept();
    }

    void Server::Echo()
    {
        while(1)
        {
            int read_size;
            char message[buffer_size];
            int client_sock = socket_.Accept();
            if (client_sock == -1)
            {
                perror("accept failed");
                return;
            }

            Address cliaddr;
            if(!socket_.GetPeerName(cliaddr, client_sock))
            {
                perror("getname failed");
                return;
            }

            std::cout << "[connect " << client_sock << "] " << cliaddr.ToString() << std::endl;

            while( (read_size = recv(client_sock, message, buffer_size, 0)) > 0 )
            {
                std::cout << "[recv " << client_sock << "] " << cliaddr.ToString() << " " << message << std::endl;
                //Send the message back to client
                send(client_sock, message, sizeof(message), 0);
                std::cout << "[send " << client_sock << "] " << cliaddr.ToString() << " " << message << std::endl;
            }
        
            std::cout << "[disconn " << client_sock << "] " << cliaddr.ToString() << std::endl;
            close(client_sock);
        }
    }

    uint16_t Server::Port() const
    {
        return port_;
    }

}
