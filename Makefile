.PHONY: all clean fclean re

NAME = ircserv
CC = c++
FLAGS = -Wall -Wextra -std=c++98 -g

SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.cpp) \
      $(wildcard $(SRC_DIR)/client/*.cpp) \
      $(wildcard $(SRC_DIR)/commands/*.cpp) \
      $(wildcard $(SRC_DIR)/channel/*.cpp) \
      $(wildcard $(SRC_DIR)/replier/*.cpp) \
      $(wildcard $(SRC_DIR)/server/*.cpp) \
      $(wildcard $(SRC_DIR)/utils/*.cpp)

OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

all: $(NAME)

debug: FLAGS += -DDEBUGGING=1
debug: $(NAME)

debugre: FLAGS += -DDEBUGGING=1
debugre: re

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/client/%.o: $(SRC_DIR)/client/%.cpp | $(OBJ_DIR)/client
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/commands/%.o: $(SRC_DIR)/commands/%.cpp | $(OBJ_DIR)/commands
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/channel/%.o: $(SRC_DIR)/channel/%.cpp | $(OBJ_DIR)/channel
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/replier/%.o: $(SRC_DIR)/replier/%.cpp | $(OBJ_DIR)/replier
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/server/%.o: $(SRC_DIR)/server/%.cpp | $(OBJ_DIR)/server
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/utils/%.o: $(SRC_DIR)/utils/%.cpp | $(OBJ_DIR)/utils
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/client:
	mkdir -p $(OBJ_DIR)/client

$(OBJ_DIR)/commands:
	mkdir -p $(OBJ_DIR)/commands

$(OBJ_DIR)/channel:
	mkdir -p $(OBJ_DIR)/channel

$(OBJ_DIR)/replier:
	mkdir -p $(OBJ_DIR)/replier

$(OBJ_DIR)/server:
	mkdir -p $(OBJ_DIR)/server

$(OBJ_DIR)/utils:
	mkdir -p $(OBJ_DIR)/utils

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/client/*.o
	rm -f $(OBJ_DIR)/commands/*.o
	rm -f $(OBJ_DIR)/channel/*.o
	rm -f $(OBJ_DIR)/replier/*.o
	rm -f $(OBJ_DIR)/server/*.o
	rm -f $(OBJ_DIR)/utils/*.o

fclean: clean
	rm -f $(NAME)

re: fclean all