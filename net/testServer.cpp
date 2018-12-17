#include <iostream>
#include <memory>

#include "server.hpp"

using namespace KvStoreServer;

int main(int argc, char const *argv[])
{
    std::shared_ptr<Server> kvServer(new Server(8888));
    kvServer->Start();

    std::cin.get();

    kvServer->Stop();

    return 0;
}
