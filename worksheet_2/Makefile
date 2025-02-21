binary = readxyz
folder = book-index
objects = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
params-bin = 5molecules-positions.xyz 5molecules-velocities.xyz

run: $(binary)
	./$(binary) $(params-bin)

$(binary): $(objects)
	g++ -g3 -o $@ $^

%.o: %.cpp
	g++ -g3 -c -o $@ $<

clean:
	rm -f *.o

clear: clean
	rm -f *.zip $(binary) *.bmp

zip: clean
	zip $(folder) Makefile *.cpp *.h *.pxl