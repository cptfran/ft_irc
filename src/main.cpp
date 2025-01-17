#include <iostream>
#include <sstream>
#include "core/Server.h"
#include "core/Log.h"
#include "utils/Utils.h"

// TODO: maybe use namespaces instead of defines? Need to check it.
// TODO: server lags sometimes. FE if connecting multiple times with nickname collision, need to check that on mac.
// More specificaly I think it hangs after leaving channel (checked with /part).
// TODO: send message to clients when disconnecting with ctrl + C?
int main(const int argc, char *argv[])
{
	if (argc != 3)
	{
		Log::msgServer(ERROR, USAGE);
		return 2;
	}
	const int port = Utils::cStringToPositiveInt(argv[1]);
	if (!port)
	{
		Log::msgServer(ERROR, INVALID_PORT_INPUT);
		return 3;
	}
	try
	{
		Server server("ft_irc.42wolfsburg.de", "1.0", argv[2], port);
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
