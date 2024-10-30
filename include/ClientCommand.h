#pragma once
#include <string>
#include <vector>

using namespace std;

class ClientCommand
{
public:
	explicit ClientCommand(const string& buffer);
	~ClientCommand();

	vector<string> getTokens();

	void parser();

private:
	ClientCommand();

	string buffer;
	vector<string> tokens;
};
