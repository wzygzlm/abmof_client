CC = g++
CFLAGS = -g -Wall
SRCS = abmof_server.cpp
PROG = abmof_server

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
		$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
