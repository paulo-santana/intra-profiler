NAME = intra_profiler

SRC_DIR = ./src
OBJ_DIR = ./obj

SRC_FILES = index.c

SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJ_FILES = $(SRC_FILES:.c=.o)
OBJ = $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

CFLAGS = -Wall -Werror -Wextra
CC = clang $(CFLAGS)

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ 

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
