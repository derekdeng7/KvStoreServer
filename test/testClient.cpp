#include "../kvStore/kvClient.hpp"

using namespace KvStoreServer;

int main(int argc, char *argv[])
{
    Client client("127.0.0.1", 8888, 1);
    client.Start();
    
    return 1;
}
