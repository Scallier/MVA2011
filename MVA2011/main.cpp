#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "RoadExtraction.hpp"


int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	RoadExtraction* CapturedMap = new RoadExtraction();

	CapturedMap->load_image("map1.bmp");
	CapturedMap->load_image("map2.png");

	CapturedMap->init_modified();
	CapturedMap->show_source();
	CapturedMap->mean_shift(3,20);
	cvWaitKey(0);
	CapturedMap->create_histogram();
	cvWaitKey(0);

	return 0;
}
