FLAGS = -Wall -g
OBJECTS = netsh.o eval.o command.o
NAME = netsh.elf
TEST = test.elf

all: $(NAME) $(TEST)

$(NAME): $(OBJECTS)
	gcc $(FLAGS) $^ -o $(NAME)

%.o: %.c %.h
	gcc $(FLAGS) -c $< -o $@

$(TEST): test.c eval.c
	gcc $(FLAGS) -o $(TEST) test.c eval.c

clean:
	rm *.o *.elf