.PHONY: all clean fclean re

NAME = ircserv
CC = c++
FLAGS = -Wall -Wextra -std=c++98 -g

SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/commands/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

all: $(NAME)

debug: FLAGS += -DDEBUGGING=1
debug: re

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/commands/%.o: $(SRC_DIR)/commands/%.cpp | $(OBJ_DIR)/commands
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/commands:
	mkdir -p $(OBJ_DIR)/commands

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/commands/*.o

fclean: clean
	rm -f $(NAME)

re: fclean all