.PHONY: all clean fclean re

NAME = ircserv
CC = c++
FLAGS = -Wall -Wextra -std=c++98 -g

SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.cpp) \
      $(wildcard $(SRC_DIR)/commands/*.cpp) \
      $(wildcard $(SRC_DIR)/communication/*.cpp) \
      $(wildcard $(SRC_DIR)/manager/*.cpp) \
      $(wildcard $(SRC_DIR)/data/*.cpp) \
      $(wildcard $(SRC_DIR)/manager/*.cpp) \
      $(wildcard $(SRC_DIR)/utils/*.cpp)

OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

DIRS = $(OBJ_DIR) $(OBJ_DIR)/commands $(OBJ_DIR)/communication $(OBJ_DIR)/core $(OBJ_DIR)/data $(OBJ_DIR)/manager $(OBJ_DIR)/utils

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

$(OBJ_DIR)/commands/%.o: $(SRC_DIR)/commands/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/communication/%.o: $(SRC_DIR)/communication/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/manager/%.o: $(SRC_DIR)/manager/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/data/%.o: $(SRC_DIR)/data/%.cpp
	@mkdir -p $(DIRS)
	$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/manager/%.o: $(SRC_DIR)/manager/%.cpp
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
