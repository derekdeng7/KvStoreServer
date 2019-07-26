#include "../kvStore/kvClient.hpp"

using namespace KvStoreServer;

int main(int argc, char *argv[])
{
    KvClient client;
    client.Start();

    //client.StartBench("127.0.0.1", 8888, 1000, 16384, 4096);

    int sockfd = client.CreateConnection("127.0.0.1", 8888);
    if(sockfd == -1)
    {
        std::cout << "Fail to CreateConnection" << std::endl;
        return -1;
    }

    client.Send(sockfd, "hello derek!!!!");

    client.Loop();
    
    return 1;
}