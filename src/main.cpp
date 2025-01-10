#include <iostream>
#include <sstream>
#include "../include/Server.h"
#include "../include/Log.h"
#include "../include/Utils.h"

// TODO: maybe use namespaces instead of defines? Need to check it.
// TODO: server lags sometimes. FE if connecting multiple times with nickname collision, need to check that on mac.
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
