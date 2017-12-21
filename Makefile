source-files = $(wildcard src/*.h)
object-files = $(patsubst src/%.h, .objects/%.o, $(source-files))

all: a.out

a.out: .objects $(object-files) .objects/main.o
	$(CC) $(CFLAGS) $(object-files) .objects/main.o -lm

.objects:
	mkdir .objects

.objects/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o .objects/main.o

.objects/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf .objects
