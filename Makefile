NAME := intra_profiler

SRC_DIR := ./src
OBJ_DIR := ./obj
INCLUDE_DIR := ./includes

SRC_FILES := index.c
DEPS = ./dependencies/mongoose.c

SRC := $(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJ_FILES := $(SRC_FILES:.c=.o)
OBJ := $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

CFLAGS = -Wall -Werror -Wextra -g3
CC := clang $(CFLAGS)

RM := rm -rf

all: $(NAME)

$(NAME): $(OBJ_DIR) $(DEPS) $(OBJ)
	$(CC) $(OBJ) $(DEPS) -o $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@ 

run: all
	./intra_profiler

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
