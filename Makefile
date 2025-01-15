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

DIRS = $(OBJ_DIR) $(OBJ_DIR)/client $(OBJ_DIR)/commands $(OBJ_DIR)/channel $(OBJ_DIR)/replier $(OBJ_DIR)/server $(OBJ_DIR)/utils

all: $(NAME)

debug: FLAGS += -DDEBUGGING=1
debug: $(NAME)

debugre: FLAGS += -DDEBUGGING=1
debugre: re

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/client/%.o: $(SRC_DIR)/client/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/commands/%.o: $(SRC_DIR)/commands/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/channel/%.o: $(SRC_DIR)/channel/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/replier/%.o: $(SRC_DIR)/replier/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/server/%.o: $(SRC_DIR)/server/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/utils/%.o: $(SRC_DIR)/utils/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all
