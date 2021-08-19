NAME := intra_profiler

VALGRIND = #valgrind

SRC_DIR := ./src
OBJ_DIR := ./obj
INCLUDE_DIR := ./includes
MBEDTLS = ./mbedtls

SRC_FILES := index.c \
			 http_client.c \
			 intra_token_generator.c \
			 utils.c

DEPS = ./dependencies/mongoose.c \
	   ./dependencies/mjson/src/mjson.c

SRC := $(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJ_FILES := $(SRC_FILES:.c=.o)
OBJ := $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

CFLAGS := -Wall -Werror -Wextra -g3 -fsanitize=address
CFLAGS += -DMG_ENABLE_MBEDTLS=1 -I$(MBEDTLS)/include
LFLAGS = -L$(MBEDTLS)/library -lmbedtls -lmbedcrypto -lmbedx509
CC := gcc $(CFLAGS)

RM := rm -rf

all: $(NAME)

$(NAME): $(OBJ_DIR) $(DEPS) $(OBJ)
	$(CC) $(OBJ) $(DEPS) -o $(NAME) $(LFLAGS)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@ 

run: all
	$(VALGRIND) ./intra_profiler

clean:
	$(RM) $(OBJ)
	$(RM) vgcore*

fclean: clean
	$(RM) $(NAME)

re: fclean all
