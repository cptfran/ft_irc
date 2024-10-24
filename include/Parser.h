#pragma once
#include <string>
#include <vector>

class Parser
{
public:
	enum cmdType
	{
		CAP,
		PASS,
		NICK,
		USER
	};
	explicit Parser(const std::string& buffer);
	~Parser();
private:
	Parser();
	cmdType commandType;
	std::vector<std::string> tokens;
};
