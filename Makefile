NAME := intra_profiler

VALGRIND = #valgrind

SRC_DIR := ./src
OBJ_DIR := ./obj
DEPS_DIR = ./dependencies
MBEDTLS_DIR = /usr/local

INCLUDE_DIR := ./includes #-I$(MBEDTLS_DIR)/include
MBEDTLS = $(MBEDTLS_DIR)/lib/libmbedtls.a

SRC_FILES := index.c \
			 http_client.c \
			 intra_token_generator.c \
			 utils.c

DEPS_FILES = mongoose.c \
			 mjson.c

DEPS = $(addprefix $(DEPS_DIR)/, $(DEPS_FILES))
DEPS_OBJ = $(DEPS:./dependencies/%.c=./obj/%.o)

SRC := $(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJ_FILES := $(SRC_FILES:.c=.o)
OBJ := $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

CFLAGS := -Wall -Werror -Wextra -g3 -fsanitize=address
CFLAGS += -DMG_ENABLE_MBEDTLS=1
LFLAGS = -L$(MBEDTLS_DIR)/library -lmbedtls -lmbedcrypto -lmbedx509
CC := gcc $(CFLAGS)

RM := rm -rf

all: $(NAME)

$(NAME): $(OBJ_DIR) $(DEPS_OBJ) $(OBJ)
	$(CC) $(OBJ) $(DEPS_OBJ) -o $(NAME) $(LFLAGS)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(DEPS_DIR)/%.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@ 

#$(MBEDTLS):
#	make -C $(MBEDTLS_DIR) no_test

run: all
	$(VALGRIND) ./intra_profiler

clean:
	$(RM) $(OBJ) $(DEPS_OBJ)
	$(RM) vgcore*

fclean: clean
	$(RM) $(NAME)

re: fclean all
