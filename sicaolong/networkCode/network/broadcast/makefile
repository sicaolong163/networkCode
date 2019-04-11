#.c----->xxx
srcFiles=$(wildcard *.c)
targetFiles=$(patsubst %.c,%,$(srcFiles))
all: $(targetFiles)

%:%.c
	gcc -o $@ $^ -lpthread -g
clean:
	rm -f $(targetFiles)
