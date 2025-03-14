binaries = readxyz genxyz
folder = worksheet-3

sources_readxyz = src/readxyz.cpp src/molecule.cpp src/cell.cpp src/molecule_system.cpp
sources_genxyz = src/genxyz.cpp src/box_generator.cpp

objects_readxyz = $(patsubst %.cpp,%.o,$(sources_readxyz))
objects_genxyz = $(patsubst %.cpp,%.o,$(sources_genxyz))

read-params-bin = 20.0 box20-density0.40-positions.xyz box20-density0.40-velocities.xyz
gen-params-bin = 20 0.4 box.xyz

# header file location
CPPFLAGS = -Iinclude -O2 -DNDEBUG

all: $(binaries) run

run: readxyz
	./readxyz $(read-params-bin)

#run: genxyz
#	./genxyz $(gen-params-bin)

# build readxyz
readxyz: $(objects_readxyz)
	g++ -g3 -o $@ $^

# build genxyz
genxyz: $(objects_genxyz)
	g++ -g3 -o $@ $^

%.o: %.cpp
	g++ -g3 $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f src/*.o

clear: clean
	rm -f *.zip $(binaries) *.bmp

zip: clean
	zip $(folder) Makefile *.cpp *.h *.pxl