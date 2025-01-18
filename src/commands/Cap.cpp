#include "commands/Cap.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "communication/Replier.h"

Cap::Cap()
{

}

Cap::~Cap()
{

}

/**
 * @brief Executes the CAP command which handles capability negotiation.
 *
 * This method processes the CAP command sent by a client. If the command
 * argument is "LS", it replies with the server's capabilities.
 *
 * @param serverManager Reference to the server's manager object.
 * @param requester Reference to the client who sent the command.
 * @param args Vector of arguments passed with the command.
 */
void Cap::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
	if (!args.empty() && args[0] == "LS")
	{
		Replier::addToQueue(requester.getFd(), Replier::rplCap, 
			Utils::anyToVec(serverManager.getConfigManager().getName()));
	}
}
