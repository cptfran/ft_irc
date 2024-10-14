#include <iostream>
#include <sstream>
#include "../include/Server.h"
#include "../include/Logging.h"

int stringToPositiveInt(const char *str)
{
	std::stringstream ss(str);
	int num;
	ss >> num;
	if (ss.fail() || num <= 0)
	{
		return 0;
	}
	return num;
}

int main(const int argc, char *argv[])
{
	if (argc != 3)
	{
		logMessage(ERROR, USAGE);
		return 2;
	}
	const int port = stringToPositiveInt(argv[1]);
	if (!port)
	{
		logMessage(ERROR, INVALID_PORT_INPUT);
		return 3;
	}
	try
	{
		Server server(argv[2]);
		server.init(port);
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
