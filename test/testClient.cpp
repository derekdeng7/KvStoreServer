#include "../util/kvClient.hpp"
#include "../util/benchClient.hpp"

using namespace KvStoreServer;

void TestKvClient()
{
    KvClient client;
    client.Start();

    int sockfd = client.CreateConnection("127.0.0.1", 8888);
    if(sockfd == -1)
    {
        std::cout << "Fail to CreateConnection" << std::endl;
        return;
    }

    client.Loop(sockfd);
}

void TestBenchClient()
{
    BenchClient client("127.0.0.1", 8888, 1000, 16384, 512);
    client.StartBench();
}

int main(int argc, char *argv[])
{
    TestKvClient();
    //TestBenchClient();
   
    return 1;
}