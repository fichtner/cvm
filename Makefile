CC=gcc

all: cvm

clean:
	@rm -f cvm

%: %.c
	$(CC) -o $@ $^
