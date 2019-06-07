CC = g++
CFLAGS = -g -Wall -O0
SRC_ABMOF_TCP = abmof_client_TCP.cpp motion_field.cpp
SRC_ABMOF_UDP = abmof_client_UDP.cpp motion_field.cpp
SRC_EFAST_TCP = abmof_client_TCP.cpp motion_field.cpp
SRC_EFAST_UDP = abmof_client_UDP.cpp motion_field.cpp
SRC_HARRIS_DEMO = cornerHarris_Demo.cpp
PROG_ABMOF_TCP = abmof_client_TCP
PROG_ABMOF_UDP = abmof_client_UDP
PROG_EFAST_TCP = efast_client_TCP
PROG_EFAST_UDP = efast_client_UDP
PROG_HARRIS_DEMO = demo_Harris

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

ALL: $(PROG_ABMOF_TCP) $(PROG_ABMOF_UDP) $(PROG_ABMOF_TCP) $(PROG_ABMOF_UDP) $(PROG_HARRIS_DEMO)

$(PROG_ABMOF_TCP):$(SRC_ABMOF_TCP)
		$(CC) $(CFLAGS) -o $(PROG_ABMOF_TCP) $(SRC_ABMOF_TCP) $(LIBS)

$(PROG_ABMOF_UDP):$(SRC_ABMOF_UDP)
		$(CC) $(CFLAGS) -o $(PROG_ABMOF_UDP) $(SRC_ABMOF_UDP) $(LIBS)

$(PROG_ABMOF_TCP):$(SRC_EFAST_TCP)
		$(CC) $(CFLAGS) -o $(PROG_ABMOF_TCP) $(SRC_EFAST_TCP) $(LIBS)

$(PROG_ABMOF_UDP):$(SRC_EFAST_UDP)
		$(CC) $(CFLAGS) -o $(PROG_ABMOF_UDP) $(SRC_EFAST_UDP) $(LIBS)

$(PROG_HARRIS_DEMO):$(SRC_HARRIS_DEMO)
		$(CC) $(CFLAGS) -o $(PROG_HARRIS_DEMO) $(SRC_HARRIS_DEMO) $(LIBS)

clean:
	rm -rf $(PROG_ABMOF_TCP) $(PROG_ABMOF_UDP) $(PROG_HARRIS_DEMO) *.o *.dSYM
	echo Clean done

