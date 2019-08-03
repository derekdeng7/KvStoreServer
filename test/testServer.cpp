#include "../util/kvServer.hpp"
#include "../util/benchServer.hpp"

using namespace KvStoreServer;

void TestKvServer()
{
    KvServer server(1, 8888);
    server.Start();
}

void TestBenchServer()
{
    BenchServer server(8888);
    server.Start();
}

int main(int argc, char const *argv[])
{
    TestKvServer();
    //TestBenchServer();

    return 1;
}
