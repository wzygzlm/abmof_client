#ifndef MOTION_FLD
#define MOTION_FLD

#include <iostream>
#include <math.h>
#include <unistd.h>
#include <vector>


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

RGB HSVToRGB(HSV hsv);

#endif