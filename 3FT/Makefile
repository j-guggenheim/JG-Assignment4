# Dependency rules for non-file targets
all: ft
clobber: clean
	rm -f *~ \#*\#
clean:
	rm -f ft *.o meminfo*

# Dependency rules for file targets
ft: ft_client.o ft.o dynarray.o path.o nodeFT.o
	gcc217 -g ft_client.o ft.o dynarray.o path.o nodeFT.o -o ft
ft_client.o: ft_client.c ft.h a4def.h 
	gcc217 -c -g ft_client.c
ft.o: ft.c ft.h nodeFT.h dynarray.h path.h a4def.h
	gcc217 -c -g ft.c
dynarray.o: dynarray.c dynarray.h
	gcc217 -c -g dynarray.c
path.o: path.c path.h a4def.h dynarray.h
	gcc217 -c -g path.c
nodeFT.o: nodeFT.c nodeFT.h dynarray.h path.h a4def.h
	gcc217 -c -g nodeFT.c
