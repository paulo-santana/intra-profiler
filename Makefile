NAME := intra_profiler

VALGRIND = #valgrind

SRC_DIR := ./src
OBJ_DIR := ./obj
DEPS_DIR = ./dependencies
MONGOC = /usr/include/libmongoc-1.0/
BSON = /usr/include/libbson-1.0/

INCLUDE_DIR := ./includes -I$(MONGOC) -I$(BSON)

SRC_FILES := index.c \
			 http_client.c \
			 intra_token_generator.c \
			 student_json.c \
			 utils.c

DEPS_FILES = mongoose.c \
			 mjson.c

DEPS = $(addprefix $(DEPS_DIR)/, $(DEPS_FILES))
DEPS_OBJ = $(DEPS:./dependencies/%.c=./obj/%.o)

SRC := $(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJ_FILES := $(SRC_FILES:.c=.o)
OBJ := $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

CFLAGS := -Wall -Werror -Wextra -g3
CFLAGS += -DMG_ENABLE_MBEDTLS=1
LFLAGS = -L$(MONGOC)/library -lmbedtls -lmbedcrypto -lmbedx509 -lm
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

docker: fclean
	docker build --tag intra-profiler . 
	docker rm -f intra-profiler
	docker run -it -p 80:80 --name intra-profiler --env-file .env \
		intra-profiler:latest

run: all
	./intra_profiler

clean:
	$(RM) $(OBJ) $(DEPS_OBJ)
	$(RM) vgcore*

fclean: clean
	$(RM) $(NAME)

re: fclean all
