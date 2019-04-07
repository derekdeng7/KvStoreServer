#include "../kvStore/kvClient.hpp"

using namespace KvStoreServer;

int main(int argc, char *argv[])
{
    KvClient client("0,0,0,0", 8888);
    client.Start();
    
    return 1;
}
