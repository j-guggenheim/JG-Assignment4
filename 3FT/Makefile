# Dependency rules for non-file targets
all: ft
clobber: clean
	rm -f *~ \#*\#
clean:
	rm -f ft *.o meminfo*

# Dependency rules for file targets
ft: ft_client.o ft.o dynarray.o path.o d_nodeFT.o
	gcc217 -g ft_client.o ft.o dynarray.o path.o d_nodeFT.o -o ft
ft_client.o: ft_client.c ft.h a4def.h 
	gcc217 -c -g ft_client.c
ft.o: ft.c ft.h d_nodeFT.h dynarray.h path.h a4def.h
	gcc217 -c -g ft.c
dynarray.o: dynarray.c dynarray.h
	gcc217 -c -g dynarray.c
path.o: path.c path.h a4def.h dynarray.h
	gcc217 -c -g path.c
d_nodeFT.o: d_nodeFT.c d_nodeFT.h dynarray.h path.h a4def.h
	gcc217 -c -g d_nodeFT.c
