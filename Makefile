.PHONY: all clean fclean re

NAME = ircserv
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o

fclean: clean
	rm -f $(NAME)

re: fclean all