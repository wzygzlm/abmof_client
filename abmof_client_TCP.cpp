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

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

static rgb   hsv2rgb(hsv in);

rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}

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

    Mat img, img_color, img_resize;
    img = Mat::ones(180 , 240, CV_8UC1)*127;;    
    int imgSize = img.total() * img.elemSize();
    uchar *iptr = img.data;
    int bytes = 0;
    int key;
    int scalsz = 3;
    hsv hsvcode; 
    rgb rgbcode;
    float angle;

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
        cv::resize(img_color, img_resize, cv::Size(), scalsz, scalsz);

        // start reading from the fourth bytes. The first bytes are used to store some debug information from the server.
        for(int bufIndex = 4; bufIndex  < bytes; bufIndex = bufIndex + 4)
        {
            uchar x = recvBuf[bufIndex];
            uchar y = recvBuf[bufIndex + 1];
            uchar pol = recvBuf[bufIndex + 2] & 0x01; // The last bit of the third bytes is polarity.
            char OF_x = 3 - ((recvBuf[bufIndex + 2] & 0x0e) >> 1); // Notice the direction should start from t-2 slice to t-1 slice.
            char OF_y = 3 - ((recvBuf[bufIndex + 2] & 0x70) >> 4);

            // Only print once
            if (bufIndex == 4) printf("OF_x is  %d, OF_y is %d.\n", OF_x, OF_y);

            Point startPt = Point(x*scalsz, y*scalsz);
            Point endPt = Point(x*scalsz + OF_x * 5,  y*scalsz + OF_y * 5);
            /*
            angle = atan2 (OF_y,OF_x) * 180 / (3.14);
            hsvcode = {angle ,abs((OF_y+OF_x)/6) ,1};
            rgbcode = hsv2rgb(hsvcode);
            std::cout << "Vx Vy value is: " << (OF_x) << (OF_y) <<std::endl;
            std::cout << "RGB R value is: " << (rgbcode.r) << std::endl;
            std::cout << "RGB G value is: " << (rgbcode.g) << std::endl;
            std::cout << "RGB B value is: " << (rgbcode.b) << std::endl;
            */
            // If (OF_x, OF_y) = (-4, -4) means it's invalid OF.
	    if(OF_x != 3 && OF_y != 3)
	    {
		    if(OF_x != -4 && OF_y != -4)
		    {
			    cv::arrowedLine(img_resize, startPt, endPt, cv::Scalar(0, 0, 255), 1);
		    }
	    }

            if(pol == 1)
            {
                for(int i = 0; i < scalsz; i++)
                {
                    for(int j = 0; j < scalsz; j++)
                    {
                        img_resize.at<Vec3b>(y*scalsz+i, x*scalsz+j)[0] = 255;
                        img_resize.at<Vec3b>(y*scalsz+i, x*scalsz+j)[1] = 255;
                        img_resize.at<Vec3b>(y*scalsz+i, x*scalsz+j)[2] = 255;
                    }
                }
                //img_color.at<Vec3b>(y, x)[0] = 255;
                //img_color.at<Vec3b>(y, x)[1] = 255;
                //img_color.at<Vec3b>(y, x)[2] = 255;
            }
            else
            {
                for(int i = 0; i < scalsz; i++)
                {
                    for(int j = 0; j < scalsz; j++)
                    {
                        img_resize.at<Vec3b>(y*scalsz+i, x*scalsz+j)[0] = 0;
                        img_resize.at<Vec3b>(y*scalsz+i, x*scalsz+j)[1] = 0;
                        img_resize.at<Vec3b>(y*scalsz+i, x*scalsz+j)[2] = 0;
                    }
                }                
                //img_color.at<Vec3b>(y, x)[0] = 0;
                //img_color.at<Vec3b>(y, x)[1] = 0;
                //img_color.at<Vec3b>(y, x)[2] = 0;
            }
            /*
            if(OF_x != -3 && OF_y != -3) //cv::line(img_color, startPt, endPt, (0, 0, 255), 1);
            {
                img_color.at<Vec3b>(y, x)[0] = 127+OF_x*40;
                img_color.at<Vec3b>(y, x)[1] = 127+OF_y*40;
                img_color.at<Vec3b>(y, x)[2] = 0;
            }*/
            
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
        
        cv::imshow("Event slice Client", img_resize); 
      
        // if (key = cv::waitKey(10) >= 0) break;
        if (key = cv::waitKey(10) >= 0);
    }   

    close(sokt);

    return 0;
}	
