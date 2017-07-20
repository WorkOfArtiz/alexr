build : libalexr.a libalexr.so

clean :
	rm -rf objs libalexr.a test

test : libalexr.a test.c
	gcc test.c libalexr.a -o test
	./test

libalexr.a : objs/alexr.o
	$(AR) rcs $@ $^

libalexr.so : objs/alexr.o
	$(LD) -shared -o $@ $^ -fPIC 

# could have generatalised here, but why bother?
objs/alexr.o : src/alexr.c
	mkdir -p objs
	$(CC) -fPIC -I. -c $^ -o $@
