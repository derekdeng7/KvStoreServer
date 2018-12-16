#include <iostream>
#include "server.hpp"

using namespace Network;

int main(int argc, char const *argv[])
{
    Server server(8888);
    server.Start();

    return 0;
}
