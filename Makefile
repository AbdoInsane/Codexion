NAME		:= codexion

SRC_DIR		:= src
OBJ_DIR		:= build

MODULES		:= 	coder \
				dongle \
			   	memory \
				monitor \
				parser \
				scheduler

MAIN		:= $(SRC_DIR)/main.c
SRCS		:= $(MAIN) $(foreach module,$(MODULES),$(wildcard $(SRC_DIR)/$(module)/*.c))
OBJS		:= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC		:= cc
CFLAGS		:= -Wall -Wextra -Werror -pthread -g
CPPFLAGS	:= -I$(SRC_DIR)

RM		:= rm -rf

ARGS = 1 200 50 50 50 1 0 fifo


all: $(NAME)
	@./$(NAME) $(ARGS)

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


#=== Debugging

grind: $(NAME)
	@valgrind --leak-check=full ./$(NAME) $(ARGS)

helgrind: $(NAME)
	@valgrind --tool=helgrind --history-level=none ./$(NAME) $(ARGS)

mem: $(NAME)
	@valgrind --tool=memcheck ./$(NAME) $(ARGS)

gdb: $(NAME)
	@gdb -tui --args ./$(NAME) $(ARGS)


#=== Formatting

format:
	@c_formatter_42 $(MAIN) $(SRC_DIR)/*/*.h $(SRC_DIR)/*/*.c


.PHONY: all clean fclean re grind helgrind mem gdb format
