schoolcoin: main.o sha256.o
	g++ -o schoolcoin main.o sha256.o

main.o: main.cpp sha256.h
	g++ -c main.cpp

sha256.o: sha256.cpp sha256.h
	g++ -c sha256.cpp

clean:
	rm *.o schoolcoin