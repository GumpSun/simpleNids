CC=gcc
COPT=-Wall -g -O0

all: main.o nids.o output.o base64.o hash.o http_parser.o stream.o
	$(CC) $(COPT) -o simpleNids main.o nids.o output.o base64.o hash.o http_parser.o stream.o  -Wl,-rpath=./lib/ -lnids -lpcap -lpthread -ljson -lcrypto -lrdkafka -lz -lrt

nids.o: nids.c nids.h
	$(CC) $(COPT) -c nids.c

output.o: output.c output.h
	$(CC) $(COPT) -c output.c

base64.o: base64.c base64.h
	$(CC) $(COPT) -c base64.c

hash.o: hash.c hash.h
	$(CC) $(COPT) -c hash.c

http_parser.o: http_parser.c http_parser.h
	$(CC) $(COPT) -c http_parser.c

stream.o: stream.c stream.h
	$(CC) $(COPT) -c stream.c

main.o: main.c config.h
	$(CC) $(COPT) -c main.c

clean:
	rm -f *.o *~ simpleNids
