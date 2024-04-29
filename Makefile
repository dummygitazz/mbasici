CC := gcc
CFLAGS := -I./src -O2


SRCS := $(shell find src -name '*.c')
OBJS := $(SRCS:src/%.c=obj/%.o)

TARGET := mbci

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lm

src/lexer/lexer.c: src/lexer/lexer.l src/parser/parser.h
	flex -o src/lexer/lexer.c src/lexer/lexer.l

src/parser/parser.h: src/parser/parser.y
	bison -d src/parser/parser.y -o src/parser/parser.c

obj/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -r obj/ mbci