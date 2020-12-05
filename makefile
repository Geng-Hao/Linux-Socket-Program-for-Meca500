
P = ClientRobot
OBJECTS = Client_Robot.c
CFLAGS = -g -Wall
LDLIBS = -lpthread
CC = gcc


$(P): $(OBJECTS) 
	$(CC) $(OBJECTS) $(CFLAGS) $(LDLIBS) -o $(P)


clean:
	rm $(P)
