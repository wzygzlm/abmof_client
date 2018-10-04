/**
 * OpenCV video streaming over TCP/IP
 * Client: Receives video from server and display it
 * by Steve Tuenkam
 */

#include "opencv2/opencv.hpp"
#include <iostream>
#include <math.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

using namespace cv; 
int main(int argc, char** argv)
{

    //--------------------------------------------------------
    //networking stuff: socket , connect
    //--------------------------------------------------------
    int         sokt;
    char*       serverIP;
    int         serverPort;

    if (argc < 3) {
           std::cerr << "Usage: cv_video_cli <serverIP> <serverPort> " << std::endl;
    }

    serverIP   = argv[1];
    serverPort = atoi(argv[2]);

    struct  sockaddr_in serverAddr;
    socklen_t           addrLen = sizeof(struct sockaddr_in);

    if ((sokt = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket() failed" << std::endl;
    }

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port = htons(serverPort);
if (connect(sokt, (sockaddr*)&serverAddr, addrLen) < 0) {
        std::cerr << "connect() failed!" << std::endl;
    }



    //----------------------------------------------------------
    //OpenCV Code
    //----------------------------------------------------------

    Mat img, img_color;
    img = Mat::zeros(180 , 240, CV_8UC1);    
    int imgSize = img.total() * img.elemSize();
    uchar *iptr = img.data;
    int bytes = 0;
    int key;

    //make img continuos
    if ( ! img.isContinuous() ) { 
          img = img.clone();
    }
        
    std::cout << "Image Size:" << imgSize << std::endl;


    namedWindow("Event slice Client", CV_WINDOW_NORMAL);
    // resizeWindow("Event sice Client", img.rows * 3,  img.cols * 3);

    char recvBuf[imgSize];

    while (key != 'q') {

        double minIntensity, maxIntensity;

        if ((bytes = recv(sokt, recvBuf, imgSize , MSG_WAITALL)) == -1) {
            std::cerr << "recv failed, received bytes = " << bytes << std::endl;
        }

        printf("Received %d data from the server.\n", bytes);
        printf("The first four bytes are %x, %x, %x, %x.\n", (uchar)recvBuf[0], (uchar)recvBuf[1], (uchar)recvBuf[2], (uchar)recvBuf[3]);
        minMaxLoc(img, &minIntensity, &maxIntensity);
        printf("The maximum intensity is %f.\n", maxIntensity);

        // Reset image
        // img = Mat::zeros(img.size(), img.type());

        cvtColor(img, img_color, COLOR_GRAY2BGR);

        // start reading from the fourth bytes. The first bytes are used to store some debug information from the server.
        for(int bufIndex = 4; bufIndex  < bytes; bufIndex = bufIndex + 4)
        {
            uchar x = recvBuf[bufIndex];
            uchar y = recvBuf[bufIndex + 1];
            uchar pol = recvBuf[bufIndex + 2] & 0x01; // The last bit of the third bytes is polarity.
            char OF_x = (recvBuf[bufIndex + 2] & 0x0e) - 3;
            char OF_y = ((recvBuf[bufIndex + 2] & 0x70) >> 4) - 3;

            // Only print once
            if (bufIndex == 40) printf("OF_x is  %d, OF_y is %d.\n", OF_x, OF_y);

            Point startPt = Point(x, y);
            Point endPt = Point(x + OF_x, y + OF_y);

            if(OF_x != -3 && OF_y != -3) cv::arrowedLine(img_color, startPt, endPt, (0, 0, 255), 1);

            if(pol == 1)
            {
                img_color.at<Vec3b>(y, x)[0] = 0;
                img_color.at<Vec3b>(y, x)[1] = 0 ;
                img_color.at<Vec3b>(y, x)[2] = 255;
            }
            else
            {
                img_color.at<Vec3b>(y, x)[0] = 0;
                img_color.at<Vec3b>(y, x)[1] = 255;
                img_color.at<Vec3b>(y, x)[2] = 0;
            }
            
        }

//        int counter = 0;
//        for (int  row = 0; row < 180; row++)
//        {
//            for (int col = 0; col < 240; col++)
//            {
//                int index = row * 240 + col;
//
//                int tmp = round(iptr[index]*255.0/maxIntensity);
//                // std::cout << "round value is: " << (unsigned int)tmp << std::endl;
//                iptr[index]= (uchar)(tmp);
//                if (iptr[index] != 0)
//                {
//                    // printf("The non-zero intensity is %d.\n", iptr[index]);
//                    img_color.at<Vec3b>(row, col)[0] = 0;
//                    img_color.at<Vec3b>(row, col)[1] = 0 ;
//                    img_color.at<Vec3b>(row, col)[2] = 255;
//                    counter++;
//                }
//            }
//        }
//        
//        printf("Non-zero pixel number is %d.\n", counter);
        
        cv::imshow("Event slice Client", img_color); 
      
        // if (key = cv::waitKey(10) >= 0) break;
        if (key = cv::waitKey(10) >= 0);
    }   

    close(sokt);

    return 0;
}	
