/**
 * OpenCV video streaming over UDP
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
#include <fstream>
#include <string>
#include <sstream>


using namespace cv;

#define PI 3.14159265

class RGB
{
public:
	unsigned char R;
	unsigned char G;
	unsigned char B;

	RGB(unsigned char r, unsigned char g, unsigned char b)
	{
		R = r;
		G = g;
		B = b;
	}

	bool Equals(RGB rgb)
	{
		return (R == rgb.R) && (G == rgb.G) && (B == rgb.B);
	}
};

class HSV
{
public:
	double H;
	double S;
	double V;

	HSV(double h, double s, double v)
	{
		H = h;
		S = s;
		V = v;
	}

	bool Equals(HSV hsv)
	{
		return (H == hsv.H) && (S == hsv.S) && (V == hsv.V);
	}
};

static RGB HSVToRGB(HSV hsv) {
	double r = 0, g = 0, b = 0;

	if (hsv.S == 0)
	{
		r = hsv.V;
		g = hsv.V;
		b = hsv.V;
	}
	else
	{
		int i;
		double f, p, q, t;

		if (hsv.H == 360)
			hsv.H = 0;
		else
			hsv.H = hsv.H / 60;

		i = (int)trunc(hsv.H);
		f = hsv.H - i;

		p = hsv.V * (1.0 - hsv.S);
		q = hsv.V * (1.0 - (hsv.S * f));
		t = hsv.V * (1.0 - (hsv.S * (1.0 - f)));

		switch (i)
		{
		case 0:
			r = hsv.V;
			g = t;
			b = p;
			break;

		case 1:
			r = q;
			g = hsv.V;
			b = p;
			break;

		case 2:
			r = p;
			g = hsv.V;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = hsv.V;
			break;

		case 4:
			r = t;
			g = p;
			b = hsv.V;
			break;

		default:
			r = hsv.V;
			g = p;
			b = q;
			break;
		}

	}

	return RGB((unsigned char)(r * 255), (unsigned char)(g * 255), (unsigned char)(b * 255));
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

    if ((sokt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        std::cerr << "socket() failed" << std::endl;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port = htons(serverPort);

    // if (connect(sokt, (sockaddr*)&serverAddr, addrLen) < 0) {
    //     std::cerr << "connect() failed!" << std::endl;
    //}



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

    //make img continuos
    if ( ! img.isContinuous() ) { 
          img = img.clone();
    }
        
    std::cout << "Image Size:" << imgSize << std::endl;


    namedWindow("Event slice Client", CV_WINDOW_NORMAL);
    // resizeWindow("Event sice Client", img.rows * 3,  img.cols * 3);

    // Send an simple message to start the connection.
    // char message[] = "OK.";
    // if ((bytes = sendto(sokt, message, sizeof(message) , MSG_WAITALL, (struct sockaddr *) &serverAddr, addrLen)) == -1) {
    //     std::cerr << "send failed, received bytes = " << bytes << std::endl;
    // }

    std::ifstream file("/home/kairx/kairx/INI/ABMOF/abmof_server/pig-withOFResult_areaThr_1000-OFResult.txt");
    std::string str; 

    char recvBuf[imgSize];
    int checknum = 0;
    while (key != 'q') {

        double minIntensity, maxIntensity;

        // if ((bytes = recvfrom(sokt, recvBuf, imgSize , MSG_WAITALL, (struct sockaddr *) &serverAddr, &addrLen)) == -1) {
        //     std::cerr << "recv failed, received bytes = " << bytes << std::endl;
        // }

        printf("Received %d data from the server.\n", bytes);
        printf("The first four bytes are %x, %x, %x, %x.\n", (uchar)recvBuf[0], (uchar)recvBuf[1], (uchar)recvBuf[2], (uchar)recvBuf[3]);
        minMaxLoc(img, &minIntensity, &maxIntensity);
        printf("The maximum intensity is %f.\n", maxIntensity);

        // Reset image
        // img = Mat::zeros(img.size(), img.type());

        cvtColor(img, img_color, COLOR_GRAY2BGR);
        cv::resize(img_color, img_resize, cv::Size(), scalsz, scalsz);

        //for reading the file
        bytes = 20000;

        // start reading from the fourth bytes. The first bytes are used to store some debug information from the server.
        for(int bufIndex = 4; bufIndex  < bytes; bufIndex = bufIndex + 4)
        {

            getline(file, str);
            checknum = checknum+1;
            std::cout<<checknum<<std::endl;
            std::stringstream stream(str);

            /*
            uchar x = recvBuf[bufIndex];
            uchar y = recvBuf[bufIndex + 1];
            uchar pol = recvBuf[bufIndex + 2] & 0x01; // The last bit of the third bytes is polarity.
            char OF_x = 3 - ((recvBuf[bufIndex + 2] & 0x0e) >> 1); // Notice the direction should start from t-2 slice to t-1 slice.
            char OF_y = 3 - ((recvBuf[bufIndex + 2] & 0x70) >> 4);
            */

            uint64_t ts;
            int x;
            int y;
            int pol;
            int OF_x;
            int OF_y;

            stream >> ts;
            stream >> x;
            stream >> y;
            stream >> pol;
            stream >> OF_x;
            stream >> OF_y;

            // Only print once
            if (bufIndex == 4) printf("OF_x is  %d, OF_y is %d.\n", OF_x, OF_y);

            Point startPt = Point(x*scalsz, y*scalsz);
            Point endPt = Point(x*scalsz + OF_x * 5,  y*scalsz + OF_y * 5);

            //conver to motion color
            double param, result;
            param = double(OF_y)/double(OF_x);
            result = atan (param) * 180 / PI;
            HSV data = HSV(result, 0.9, 0.9);
            RGB value = HSVToRGB(data);

            // If (OF_x, OF_y) = (-4, -4) means it's invalid OF.
	    if(OF_x != 3 && OF_y != 3)
	    {
		    if(OF_x != -4 && OF_y != -4)
		    {
			    cv::arrowedLine(img_resize, startPt, endPt, cv::Scalar(value.B, value.G, value.R), 1);
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
