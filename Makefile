object-files = $(patsubst src/%.c, .objects/%.o, $(wildcard src/*.c))
dependencies = $(patsubst %.o, %.d, $(object-files))

all: a.out

a.out: .objects $(object-files) 
	$(CC) $(CFLAGS) $(object-files) -lm

.objects:
	mkdir .objects

.objects/%.o: src/%.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(dependencies)

clean:
	rm -rf .objects a.out
