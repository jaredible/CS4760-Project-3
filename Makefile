CC		= gcc
CFLAGS		= -Wall -g

MASTER_SRC	= master.c
MASTER_OBJ	= $(MASTER_SRC:.c=.o)
MASTER		= master

PALIN_SRC	= palin.c
PALIN_OBJ	= $(PALIN_SRC:.c=.o)
PALIN		= palin

OUTPUT		= $(MASTER) $(PALIN)
all: $(OUTPUT)

$(MASTER): $(MASTER_OBJ)
	$(CC) $(CFLAGS) $(MASTER_OBJ) -o $(MASTER)

$(PALIN): $(PALIN_OBJ)
	$(CC) $(CFLAGS) $(PALIN_OBJ) -o $(PALIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

.PHONY: clean
clean:
	/bin/rm -f $(OUTPUT) *.o
