#include "manager/ConnectionManager.h"

ConnectionManager::ConnectionManager() : serverRunning(false)
{

}

ConnectionManager::~ConnectionManager()
{

}

/**
 * @brief Initializes the server with the specified port.
 *
 * This function sets up the server to listen for incoming connections on the specified port.
 * It performs the following steps:
 * 1. Validates that the port number is within the registered port range.
 * 2. Creates a socket for the server.
 * 3. Configures the server's address structure with the specified port and binds the socket to it.
 * 4. Sets the socket to listen for incoming connections.
 *
 * If any step fails, the function throws a runtime error with an appropriate error message.
 *
 * @param port The port number on which the server will listen for incoming connections.
 * @throws std::runtime_error If the port is out of range, socket creation fails, binding fails, or listening fails.
 */
void ConnectionManager::initSocket(const int port)
{
	if (port < REGISTERED_PORT_MIN || port > REGISTERED_PORT_MAX)
	{
		throw std::runtime_error(ERROR WRONG_PORT_RANGE);
	}

	this->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fd < 0)
	{
		throw std::runtime_error(ERROR SOCKET_FAIL);
	}

	// Set the socket option to allow the socket to be reusable.
	// This is useful for servers that need to restart quickly and bind to the same port
	// without waiting for the operating system to release the port.
	const int option = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)))
	{
		throw std::runtime_error(ERROR SETSOCKOPT_FAIL);
	}

	// Make the socket non-blocking.
	if (fcntl(this->fd, F_SETFL, O_NONBLOCK))
	{
		throw std::runtime_error(ERROR F_SETFL_FAIL);
	}

	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
	{
		close(this->fd);
		throw std::runtime_error(ERROR BIND_FAIL);
	}

	if (listen(this->fd, SOMAXCONN) < 0)
	{
		close(this->fd);
		throw std::runtime_error(ERROR LISTEN_FAIL);
	}
}

/**
 * @brief Runs the server, accepting and handling client connections.
 *
 * This function enters an infinite loop to continuously accept new client connections.
 * For each new connection, it attempts to authenticate the client using the authenticateClient function.
 * If the authentication fails, it closes the connection and logs an error message.
 * If the authentication succeeds, it logs a success message, sends a welcome message to the client,
 * adds the client's socket to the list of client sockets, and calls the handleRecv function to manage the client.
 * The function logs messages at various stages to provide information about the server's operation and any errors encountered.
 */
void ConnectionManager::runServer()
{
	Log::msgServer(INFO, SERVER_RUN);

	// Add the server to the poll.
	const pollfd listenFd = { this->fd, POLLIN, 0 };
	this->pollFds.push_back(listenFd);

	// Main loop, handling new client connections and already running clients.
	while (running)
	{
		const int pollResult = poll(&this->pollFds[0], this->pollFds.size(), 1000);
		if (pollResult < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			Log::msgServer(ERROR, "Poll error");
			return;
		}

		handleTimeouts();
		addNewClientsToPoll();

		for (std::vector<pollfd>::iterator it = this->pollFds.begin(); it != this->pollFds.end();)
		{
			if ((it->revents & POLLHUP) == POLLHUP)
			{
				disconnectClient(it->fd);
				it = this->pollFds.erase(it);
				continue;
			}
			else if ((it->revents & POLLOUT) == POLLOUT && Replier::clientInQueue(it->fd))
			{
				Replier::sendFromQueue(it->fd);
			}
			else if ((it->revents & POLLIN) == POLLIN)
			{
				if (it->fd == this->fd)
				{

					this->connectClient();
				}
				else
				{
					this->handleRecv(this->clients.at(it->fd));
				}
			}
			++it;
		}
	}
}

void ConnectionManager::stopServer()
{
	this->running = false;
}

void ConnectionManager::connectClient()
{
	Log::msgServer(INFO, NEW_CLIENT_START);

	sockaddr_in addr = {};
	socklen_t addrLen = sizeof(addr);

	// Create fd for the new client and accept it.
	const int clientFd = accept(this->fd, reinterpret_cast<sockaddr*>(&addr), &addrLen);
	if (clientFd < 0)
	{
		throw std::runtime_error(ERROR ACCEPT_FAIL);
	}

	// Create pollfd for the new client and add it to the pollfd list.
	const pollfd pollFd = { clientFd, POLLIN | POLLOUT, 0 };
	this->pollFdsToAdd.push_back(pollFd);

	// Create new client object.
	Client client(clientFd);

	// Fetch client hostname.
	client.setHostname(getClientHostname(addr, addrLen, clientFd));

	// Add new client to the clients list.
	this->clients.insert(std::make_pair(clientFd, client));

	Log::msgServer(INFO, "CLIENT", clientFd, NEW_CLIENT_SUCCESS);
}

void ConnectionManager::addNewClientsToPoll()
{
	for (std::vector<pollfd>::iterator it = this->pollFdsToAdd.begin(); it != this->pollFdsToAdd.end(); ++it)
	{
		this->pollFds.push_back(*it);
	}

	this->pollFdsToAdd.clear();
}

void ConnectionManager::disconnectClient(const int clientFd)
{
	for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		it->ejectUser(*this, this->clients.at(clientFd).getNickname());
	}
	this->clients.erase(clientFd);
	close(clientFd);
	Log::msgServer(INFO, "CLIENT", clientFd, CLIENT_DISCONNECTED);
}

void ConnectionManager::handleTimeouts()
{
	for (std::vector<pollfd>::iterator it = this->pollFds.begin(); it != this->pollFds.end();)
	{
		std::map<int, Client>::iterator clientIt = this->clients.find(it->fd);
		if (clientIt != this->clients.end() &&
			!clientIt->second.registered(this->password) &&
			difftime(std::time(0), clientIt->second.getTimeConnected()) > TIME_FOR_CLIENT_TO_REGISTER)
		{
			Replier::addToQueue(it->fd, Replier::errClosingLink, Utils::anyToVec(clientIt->second.getNickname(),
				clientIt->second.getHostname()));
			disconnectClient(it->fd);
			it = this->pollFds.erase(it);
			continue;
		}
		++it;
	}
}

void ConnectionManager::handleNicknameCollision(const int newClientFd, const std::string& newClientNickname)
{
	for (std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
	{
		if (newClientNickname == it->second.getNickname() && it->second.getFd() != newClientFd)
		{
			Replier::addToQueue(newClientFd, Replier::errNickCollision, Utils::anyToVec(this->name,
				newClientNickname, it->second.getUsername(), it->second.getHostname()));
			this->disconnectClient(it->second.getFd());
			return;
		}
	}
}