#pragma once

#include "Client.h"
#include <vector>

class Replier
{
public:
	~Replier();

private:
	typedef std::string (*ReplyFunction)(const std::vector<std::string>&);

	Replier();

	// Reply functions.
	static void reply(const Client& client, ReplyFunction func, const std::vector<std::string>& args);
	static std::string rplWelcome(const std::vector<std::string>& args); // 001
	static std::string rplYourHost(const std::vector<std::string>& args); // 002
	static std::string rplCreated(const std::vector<std::string>& args); // 003
	static std::string rplMyInfo(const std::vector<std::string>& args); // 004
	static std::string rplPong(const std::vector<std::string>& args); // none
	static std::string rplCap(const std::vector<std::string>& args); // none
	static std::string rplNoTopic(const std::vector<std::string>& args); // 331
	static std::string errUnknownCommand(const std::vector<std::string>& args); // 421
	static std::string errNickCollision(const std::vector<std::string>& args); // 436
	static std::string errNotRegistered(const std::vector<std::string>& args); // 451
	static std::string errNeedMoreParams(const std::vector<std::string>& args); // 461
	static std::string errPasswdMismatch(const std::vector<std::string>& args); // 464
	static std::string errChannelIsFull(const std::vector<std::string>& args); // 471
	static std::string errInviteOnlyChan(const std::vector<std::string>& args); // 473

	friend class Server;
	friend class Cap;
	friend class Join;
	friend class Nick;
	friend class Pass;
	friend class Ping;
	friend class User;
};