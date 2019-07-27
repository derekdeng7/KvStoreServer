#include "../util/kvClient.hpp"
#include "../util/benchClient.hpp"

using namespace KvStoreServer;

int main(int argc, char *argv[])
{
    KvClient client;
    client.Start();

    int sockfd = client.CreateConnection("127.0.0.1", 8888);
    if(sockfd == -1)
    {
        std::cout << "Fail to CreateConnection" << std::endl;
        return -1;
    }

    client.Loop(sockfd);
    
    return 1;
}