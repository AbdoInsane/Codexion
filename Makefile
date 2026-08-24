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
CFLAGS		:= -Wall -Wextra -Werror -pthread -g
CPPFLAGS	:= -I$(SRC_DIR)

RM		:= rm -rf

I = 1

ARGS_1 = 3 3000 200 200 200 2 800 edf
ARGS_2 = 2 100 50 25 25 2 10 edf
ARGS_3 = 3 3000 200 200 200 10 800 edf
ARGS_4 = 3 3000 200 200 200 2 800 edf


all: $(NAME)

run:
	./$(NAME) $(ARGS_$(I))

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
	@valgrind --leak-check=full ./$(NAME) $(ARGS_$(I))

helgrind: $(NAME)
	@valgrind --tool=helgrind --history-level=none ./$(NAME) $(ARGS_$(I))

mem: $(NAME)
	@valgrind --tool=memcheck ./$(NAME) $(ARGS_$(I)

gdb: $(NAME)
	@gdb -tui --args ./$(NAME) $(word 1,$(ARGS_$(I))) 100000000 $(wordlist 3,8,$(ARGS_$(I)))


#=== Formatting

format:
	@c_formatter_42 $(MAIN) $(SRC_DIR)/*/*.h $(SRC_DIR)/*/*.c


.PHONY: all clean fclean re grind helgrind mem gdb format
