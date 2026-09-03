NAME		:= codexion

SRC_DIR		:= src
OBJ_DIR		:= build

MODULES		:= 	coder \
				dongle \
				logger \
			   	memory \
				monitor \
				parser \
				table \

MAIN		:= $(SRC_DIR)/main.c
SRCS		:= $(MAIN) $(foreach module,$(MODULES),$(wildcard $(SRC_DIR)/$(module)/*.c))
OBJS		:= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC		:= cc
CFLAGS		:= -Wall -Wextra -Werror -pthread
CPPFLAGS	:= -I$(SRC_DIR)

RM		:= rm -rf


all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(NAME)

re: fclean all


.PHONY: all clean fclean re grind helgrind mem gdb format
