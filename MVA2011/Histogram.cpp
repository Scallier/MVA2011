#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "Histogram.hpp"

Histogram::Histogram(int blue, int green, int red, int size):
m_height(size),
m_current_value(0)
{
	m_histo_values = new int[m_height]();
	m_initial_color = new Color(blue,green,red);
}

Histogram::~Histogram()
{
	if (m_initial_color)
		delete m_initial_color;
	if (m_histo_values)
		delete m_histo_values;
}

void
Histogram::add_value(int blue, int green, int red)
{
	m_histo_values[m_current_value] = m_initial_color->difference(blue,green,red);
	if (m_current_value<m_height)
		++m_current_value;
	else
		printf("Histogram already full, last value has been replaced\n");
}

int
Histogram::get_max_value()
{
	int max=0;
	for (int i=0;i<m_current_value;i++)
		if (m_histo_values[i]>max)
			max = m_histo_values[i];
	return max;
}

void
Histogram::show_histogram()
{
	IplImage* histogram_img = cvCreateImage(cvSize(get_max_value()+1,m_height),8,1);
	cvZero(histogram_img);
	for(int i=0;i<m_height;i++)
	{
		int next_value = m_histo_values[i];
		int j=m_height-1;
		while (CV_IMAGE_ELEM(histogram_img,uchar, j, next_value)==255)
			--j;
		CV_IMAGE_ELEM(histogram_img,uchar, j, next_value) = 255;
	}
	cvNamedWindow("Histogram",0);
	cvResizeWindow("Histogram",5*get_max_value(),5*m_height);
	cvShowImage("Histogram", histogram_img );
	cvReleaseImage(&histogram_img);
}

void
Histogram::clear()
{
	m_current_value = 0;
}
