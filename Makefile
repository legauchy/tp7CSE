CC=gcc
CFLAGS=-Wall -O
LDFLAGS=-pthread
EXEC=test

all: $(EXEC)
			rm *.o
			
test: stdes.o test.o
			$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
			$(CC) -o $@ -c $< $(CFLAGS)

clean:
			rm -f *.o *~

mrproper: clean
			rm -f $(EXEC) core
			
check : all
			./test


