CC=gcc
OPTS=-g -std=c99 -Werror

all: main.o predictor.o gshare.o tournament.o
	$(CC) $(OPTS) -lm -o predictor main.o predictor.o gshare.o tournament.o

main.o: main.c predictor.h
	$(CC) $(OPTS) -c main.c

predictor.o: gshare.h tournament.h predictor.h predictor.c
	$(CC) $(OPTS) -c predictor.c

gshare.o: predictor.h gshare.h gshare.c
	$(CC) $(OPTS) -c gshare.c

tournament.o: predictor.h tournament.h tournament.c
	$(CC) $(OPTS) -c tournament.c


clean:
	rm -f *.o predictor;
