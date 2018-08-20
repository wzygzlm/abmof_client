CC = g++
CFLAGS = -g -Wall
SRCS = abmof_client.cpp
PROG = abmof_client

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
		$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
