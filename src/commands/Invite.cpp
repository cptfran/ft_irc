#include "commands/Invite.h"
#include "core/Server.h"  
#include "utils/Utils.h"  
#include "communication/Replier.h"  

Invite::Invite()
{

}  

Invite::~Invite()  
{  

}  

/**  
* @brief Executes the invite command to invite a user to a channel.  
*  
* @param serverManager Reference to the server manager.  
* @param requester Reference to the client who is requesting the invite.  
* @param args Vector of arguments passed to the command.  
*/  
void Invite::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const  
{  
   ConfigManager& serverConfig = serverManager.getConfigManager();  

   // Not enough parameters provided.  
   if (args.size() < 2)  
   {  
       Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(serverConfig.getName(),  
           requester.getNickname(), std::string("INVITE")));  
       return;  
   }  

   // Find the user which will be invited.  
   const std::string& nicknameToInvite = args[0];  
   const Client* clientToInvite = serverManager.getClientManager().getClientByNickname(nicknameToInvite);  

   // Invited user not found.  
   if (clientToInvite == NULL)  
   {  
       Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(serverConfig.getName(),  
           requester.getNickname(), nicknameToInvite));  
       return;  
   }  

   // Find the invitation channel.  
   const std::string& channelToInviteName = args[1];  
   Channel* channelToInvite = serverManager.getChannelManager().getChannel(channelToInviteName);  

   // Invitation channel not found.  
   if (channelToInvite == NULL)  
   {  
       Replier::addToQueue(requester.getFd(), Replier::rplInviting, Utils::anyToVec(serverConfig.getName(),  
           requester.getNickname(), clientToInvite->getNickname(), channelToInviteName));  
       Replier::addToQueue(requester.getFd(), Replier::rplInvite,  
           Utils::anyToVec(requester.getNickname(), requester.getNickname(), channelToInviteName));  
       return;  
   }  

   // Inviting client is not a member of the channel.  
   if (!channelToInvite->isUserOnChannel(requester.getNickname()))  
   {  
       Replier::addToQueue(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(serverConfig.getName(),  
           requester.getNickname(), channelToInviteName));  
       return;  
   }  

   // User to invite is already on the channel.  
   if (channelToInvite->isUserOnChannel(clientToInvite->getNickname()))  
   {  
       Replier::addToQueue(requester.getFd(), Replier::errUserOnChannel, Utils::anyToVec(serverConfig.getName(),  
           requester.getNickname(), nicknameToInvite, channelToInviteName));  
       return;  
   }  

   // Channel is invite-only and inviting client doesn't have operator privileges.  
   if (channelToInvite->isInviteOnly() && !channelToInvite->isUserOperator(requester.getNickname()))  
   {  
       Replier::addToQueue(requester.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(serverConfig.getName(),  
           requester.getNickname(), channelToInviteName));  
       return;  
   }  

   // Add invited user nickname to the invited list of the channel and send invite replies.  
   inviteUser(*channelToInvite, requester, *clientToInvite, serverConfig.getName());  
}  

/**  
* @brief Adds the invited user to the channel's invite list and sends invite replies.  
*  
* @param channelToInvite Reference to the channel to which the user is invited.  
* @param invitingClient Reference to the client who is inviting.  
* @param invitedClient Reference to the client who is invited.  
* @param serverName Name of the server.  
*/  
void Invite::inviteUser(Channel& channelToInvite, const Client& invitingClient, const Client& invitedClient,  
   const std::string& serverName) const  
{  
   // Add invited user nickname to the invited list of the channel.  
   channelToInvite.addToInviteList(invitedClient.getNickname());  

   // Send invite replies.  
   Replier::addToQueue(invitingClient.getFd(), Replier::rplInviting, Utils::anyToVec(serverName,  
       invitingClient.getNickname(), invitedClient.getNickname(), channelToInvite.getName()));  
   Replier::addToQueue(invitedClient.getFd(), Replier::rplInvite,  
       Utils::anyToVec(invitingClient.getNickname(), invitedClient.getNickname(), channelToInvite.getName()));  
}