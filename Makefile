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

strings.in:
	echo a > strings.in
	echo ab >> strings.in
	echo aba >> strings.in
	echo abb >> strings.in
	echo abba >> strings.in
	echo 0 >> strings.in
	echo 01 >> strings.in
	echo 010 >> strings.in
	echo 011 >> strings.in
	echo 0110 >> strings.in
	echo z >> strings.in
	echo zz >> strings.in
	echo zzz >> strings.in
	echo zzzz >> strings.in
	echo zzzzz >> strings.in
	echo zzzzzz >> strings.in
	echo zzzzzzz >> strings.in
	echo zzzzzzzz >> strings.in
	echo zzzzzzzzz >> strings.in
	echo zzzzzzzzzz >> strings.in
	echo "this shouldn't be checked" >> strings.in

test: all strings.in
	./master -s 4 -t 4 strings.in
	cat palin.out; echo; cat nopalin.out; echo; cat output.log
	rm palin.out nopalin.out output.log
	echo
	./master -s 3 -t 2 strings.in
	cat palin.out; echo; cat nopalin.out; echo; cat output.log

.PHONY: clean
clean:
	/bin/rm -f $(OUTPUT) *.o *.in *.out *.log
