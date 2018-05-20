schoolcoin: main.o sha256.o
	g++ -o schoolcoin main.o sha256.o

main.o: main.cpp sha256.h
	g++ -std=c++0x -c main.cpp

sha256.o: sha256.cpp sha256.h
	g++ -std=c++0x -c sha256.cpp

clean:
	rm *.o schoolcoin