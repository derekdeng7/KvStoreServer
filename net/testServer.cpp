#include <iostream>

#include "server.hpp"

using namespace std;
using namespace Network;

static const int BUF_SIZE = 1024;

int main(int argc, char const *argv[])
{
    Server server(8888);
    server.Start();

    return 0;
}
