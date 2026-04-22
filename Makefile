FLAGS = -Wall -g
OBJECTS = eval.o command.o queue.o
NAME = netsh
TEST = test

all: $(NAME) $(TEST)

$(NAME): $(OBJECTS) $(NAME).o
	gcc $(FLAGS) $^ -o $(NAME)

%.o: %.c %.h
	gcc $(FLAGS) -c $< -o $@

$(TEST): $(OBJECTS) test.c
	gcc $(FLAGS) $^ -o $(TEST)

clean:
	rm *.o $(NAME) $(TEST)