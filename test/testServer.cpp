#include "../kvStore/kvServer.hpp"

using namespace KvStoreServer;

int main(int argc, char const *argv[])
{
    KvServer server(5, 8888);
    server.Start();

    return 1;
}