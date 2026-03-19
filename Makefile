FLAGS = -Wall -g
OBJECTS = shell.o dir.o commands.o
NAME = netsh.elf

all: $(NAME)

$(NAME): $(OBJECTS)
	gcc $(FLAGS) $^ -o $(NAME)

%.o: %.c %.h
	gcc $(FLAGS) -c $< -o $@

clean:
	rm *.o *.elf