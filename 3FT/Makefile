# Dependency rules for non-file targets
all: testFT
clobber: clean
	rm -f *~ \#*\#
clean:
	rm -f testFT *.o

# Dependency rules for file targets
testFT: ft_client.o ft.o
	gcc217 ft_client.o ft.o -o testFT
ft_client.o: ft_client.c ft.h
	gcc217 -c ft_client.c
ft.o: ft.c ft.h d_nodeFT.h dynarray.h
	gcc217 -c ft.c