CC		= gcc
CFLAGS		= -Wall -g

MASTER_SRC	= master.c
MASTER_OBJ	= $(MASTER_SRC:.c=.o)
MASTER		= master

PALIN_SRC	= palin.c
PALIN_OBJ	= $(PALIN_SRC:.c=.o)
PALIN		= palin

SRC		= helper.h shared.h
OBJ		= helper.o shared.o

STANDARD	= constant.h

OUTPUT		= $(MASTER) $(PALIN)

all: $(OUTPUT)

$(MASTER): $(MASTER_OBJ) $(OBJ)
	$(CC) $(CFLAGS) $(MASTER_OBJ) $(OBJ) -o $(MASTER)

$(PALIN): $(PALIN_OBJ) $(OBJ)
	$(CC) $(CFLAGS) $(PALIN_OBJ) $(OBJ) -o $(PALIN)

%.o: %.c $(SRC) $(STANDARD)
	$(CC) $(CFLAGS) -c $*.c -o $*.o

.PHONY: clean
clean:
	/bin/rm -f $(OUTPUT) *.o *.out *.log
