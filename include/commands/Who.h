#pragma once

#include "Command.h"

class Who: public Command
{
public:
	Who();
	~Who();

	void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
	
private:
	bool wildcardValidator(const std::string& pattern, const std::string& str);
	bool matchWildcard(const char* pattern, const char* str);
};