CC = g++
CFLAGS = -g -Wall -O0
SRC_TCP = abmof_client_TCP.cpp motion_field.cpp
SRC_UDP = abmof_client_UDP.cpp motion_field.cpp
PROG_TCP = abmof_client_TCP
PROG_UDP = abmof_client_UDP

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

ALL: $(PROG_TCP) $(PROG_UDP)

$(PROG_TCP):$(SRC_TCP)
		$(CC) $(CFLAGS) -o $(PROG_TCP) $(SRC_TCP) $(LIBS)

$(PROG_UDP):$(SRC_UDP)
		$(CC) $(CFLAGS) -o $(PROG_UDP) $(SRC_UDP) $(LIBS)
