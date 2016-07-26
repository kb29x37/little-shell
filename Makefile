EXEC=shell
CFLAGS	+=	-I  -pedantic
RM = rm -f
SRC = adds.c \
			builtin.c \
			main.c \
			linked.c
OBJ = $(SRC:.c=.o)

all : $(EXEC)

$(EXEC) : $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

main.o : main.c builtin.h
	gcc -o $@ -c $< $(CFLAGS)

builtin.o : builtin.c builtin.h
	gcc -o $@ -c $< $(CFLAGS)

adds.o : adds.c
	gcc -o $@ -c $< $(CFLAGS)

linked.o : linked.c adds.h
	gcc -o $@ -c $< $(CFLAGS)

clean :
	$(RM) $(OBJ)

fclean : clean
	$(RM) $(EXEC)

run :
	./$(EXEC)
