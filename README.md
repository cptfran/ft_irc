# ft_irc
ft_irc is an IRC (Internet Relay Chat) server implementation written in C++ (restricted to C++98). This project is part of the 42 Core Curriculum and aims to provide a functional IRC server. Irssi was chosen as reference client for most of the testing.

## Features
- able to authenticate, set a nickname, a username, join a channel, send and receive private messages
- channel operators
- available commands:
  - Cap, Invite, Join, Kick, Mode, Nick, Part, Pass, Ping, Privmsg, Topic, User, Who
- partial commands

## Installation
```
git clone https://github.com/cptfran/ft_irc.git
cd ft_irc
make OR make debug (for more information)
```

## Run the server
```
./ircserv <port> <server_pass>
```

## Connect with client
### Irssi
```
// Linux irssi installation:
sudo apt install irssi

// Mac irssi installation:
brew install irssi

// Run irssi and connect:
irssi
/connect localhost <port> <server_pass>
```

### Netcat
```
// Linux netcat installation:
sudo apt install netcat

// Mac netcat installation:
brew install netcat

// Run netcat and connect:
nc -C localhost <port>
```
