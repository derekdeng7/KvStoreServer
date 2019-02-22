#include <iostream>
#include <memory>
#include <functional>

#include "server.hpp"
#include "threadGuard.hpp"

using namespace KvStoreServer;

int main(int argc, char const *argv[])
{
    std::shared_ptr<Server> kvServer(new Server(8888));
    ThreadGuard thd1(std::thread(
        [&kvServer]{
            kvServer->Start();
        }
    ));

    std::cout << "press any key to exit" << std::endl;
    std::cin.get();

    //kvServer->Close();

    return 0;
}
