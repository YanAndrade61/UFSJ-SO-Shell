CFLAGS := -Wall -Wextra -iquote ./include
DEFS := -D NONE
NAME := shellso

SRC_DIR := src
OBJ_DIR := obj

OBJ := $(addprefix $(OBJ_DIR)/, $(notdir $(patsubst %.c, %.o, $(wildcard $(SRC_DIR)/*.c))))

$(NAME): OBJ_DIR_CHECKING $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -lm -lreadline

obj/%.o: $(SRC_DIR)/%.c
	$(CC) $(DEFS) -c $(CFLAGS) $< -o $@

.PHONY: clean OBJ_DIR_CHECKING
OBJ_DIR_CHECKING:
	mkdir -p obj

clean:
	rm -f $(OBJ_DIR)/*.o ./$(NAME) *.out