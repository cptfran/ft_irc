#include <iostream>
#include "core/Server.h"
#include "core/Log.h"
#include "utils/Utils.h"

static bool initServer(const char* host, const char* version, const char* password, int port);

// TODO: clients not leaving the channel
// TODO: segfault when wrong parameters in /mode +o
int main(const int argc, char* argv[])
{
    if (argc != 3)
    {
        Log::msgServer(Log::ERROR, Log::USAGE);
        return 1;
    }

    const int port = Utils::cStringToPositiveInt(argv[1]);
    if (!port)
    {
        Log::msgServer(Log::ERROR, Log::INVALID_PORT_INPUT);
        return 2;
    }

    if (!initServer("ft_irc.42wolfsburg.de", "1.0", argv[2], port))
    {
        return 3;
    }

    return 0;
}

static bool initServer(const char* host, const char* version, const char* password, int port)
{
    try
    {
        Server server(host, version, password, port);
        server.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}
