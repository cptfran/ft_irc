#include <map>
#include "../include/ClientCommand.h"
#include <sstream>
#include <algorithm>
#include "Client.h"
#include "Logging.h"

ClientCommand::ClientCommand(const string& buffer) : buffer(buffer)
{
}

ClientCommand::~ClientCommand()
{

}

void ClientCommand::parser()
{
	istringstream iss(this->buffer);
	string token;
	const string commandList[] = {"CAP", "JOIN", "PASS", "NICK", "USER"};
	while (iss >> token)
	{
		this->tokens.push_back(token);
	}
	if (tokens.empty())
	{
		throw runtime_error(ERROR EMPTY_CLIENT_PROMPT);
	}
	const string* first = commandList;
	const string* last = commandList + sizeof(commandList) / sizeof(commandList[0]);
    if (find(first, last, tokens[0]) == last)
	{
		throw runtime_error(ERROR WRONG_CLIENT_COMMAND);
	}
}

vector<string> ClientCommand::getTokens()
{
	return this->tokens;
}