#include "../util/kvClient.hpp"
#include "../util/benchClient.hpp"

using namespace KvStoreServer;

int main(int argc, char *argv[])
{
    //BenchClient bClient("127.0.0.1", 8888, 1000, 1024, 16);
    //bClient.StartBench();

    KvClient client;
    client.Start();

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