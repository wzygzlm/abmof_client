# abmof_client
A TCP/UDP client to display ABMOF result.


How to install?
1. install opencv library:

sudo add-apt-repository ppa:timsc/opencv-3.3

sudo apt-get update

sudo apt-get install libopencv-dev libopencv-shape-dev libopencv-dnn-dev

2. build

make

3. run the client, 4097 is the server port number. It's not fixed and could changed by the server.

./abmof_client_TCP 4097 10000 10000 1 for TCP
./abmof_client_UDP 4097 10000 10000 0 for UDP
