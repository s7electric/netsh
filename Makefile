FLAGS = -Wall -g
OBJECTS = netsh.o shell.o
NAME = netsh.elf

all: $(NAME) test.elf

$(NAME): $(OBJECTS)
	gcc $(FLAGS) $^ -o $(NAME)

%.o: %.c %.h
	gcc $(FLAGS) -c $< -o $@

test.elf: test.c shell.c
	gcc $(FLAGS) -o test.elf test.c shell.c

clean:
	rm *.o *.elf