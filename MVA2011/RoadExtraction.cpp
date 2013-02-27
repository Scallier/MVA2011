#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "RoadExtraction.hpp"

void
SetMatElem(CvMat *mat, int row, int col, int channel, int val)
{
	((float*)(mat->data.ptr+mat->step*row))[col*mat->step+channel]= val;
}

RoadExtraction::RoadExtraction():
m_source(0),
m_modified(0),
m_expanded(0)
{
}

RoadExtraction::~RoadExtraction()
{
	if (m_source)
		cvReleaseImage(&m_source);
	if (m_modified)
		cvReleaseImage(&m_modified);
	if (m_expanded)
		cvReleaseImage(&m_expanded);
}

void
RoadExtraction::load_image(const char * filename)
{
	m_source = cvLoadImage (filename, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	if(!m_source)
		std::cout<<"Failed to load image";
}

void
RoadExtraction::show_source()
{
	cvNamedWindow("Source Image", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("Source Image", 100, 100);
	cvShowImage("Source Image", m_source );
}

void
RoadExtraction::show_modified()
{
	cvNamedWindow("Modified Image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Modified Image", m_modified );
}

void
RoadExtraction::init_modified()
{
	if (m_modified)
		cvReleaseImage(&m_modified);
	m_modified = cvCloneImage (m_source);
	if(!m_modified)
		std::cout<<"Failed to clone";
}

void
RoadExtraction::create_histogram()
{
	IplImage* Proba = cvCreateImage(cvGetSize(m_modified),8,1);
	cvZero(Proba);
	IplImage* Apercu = cvCreateImage(cvSize(15,15),8,3);

	for(int i=7;i<m_modified->width-7;i=i+1)
	{
		for(int j=7;j<m_modified->height-7;j=j+1)
		{
			cvZero(Apercu);

			for (int i2=i-7;i2<i-7+15;i2++)
			{
				for (int j2=j-7;j2<j-7+15;j2++)
				{
					CV_IMAGE_ELEM(Apercu,uchar, j2-j+7, (i2-i+7)*3) = CV_IMAGE_ELEM(m_modified,uchar, j2, i2*3);
					CV_IMAGE_ELEM(Apercu,uchar, j2-j+7, (i2-i+7)*3+1) = CV_IMAGE_ELEM(m_modified,uchar, j2, i2*3+1);
					CV_IMAGE_ELEM(Apercu,uchar, j2-j+7, (i2-i+7)*3+2) = CV_IMAGE_ELEM(m_modified,uchar, j2, i2*3+2);
				}
			}

			for(int xVar=-7;xVar<7;xVar++)
			{
				Histogram* histo = new Histogram(CV_IMAGE_ELEM(m_modified,uchar, j, i*3),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+1),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+2),15);
				send_pixels(histo,15,j,i,xVar,7);
				if (histo->get_max_value()<20 && CV_IMAGE_ELEM(Proba,uchar, j, i)<250)
					CV_IMAGE_ELEM(Proba,uchar, j, i)+=5;
				delete histo;
			}

			for(int yVar=-7;yVar<7;yVar++)
			{
				Histogram* histo = new Histogram(CV_IMAGE_ELEM(m_modified,uchar, j, i*3),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+1),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+2),15);
				send_pixels(histo,15,j,i,7,yVar);
				if (histo->get_max_value()<20 && CV_IMAGE_ELEM(Proba,uchar, j, i)<250)
					CV_IMAGE_ELEM(Proba,uchar, j, i)+=5;
				delete histo;
			}
		}
	}


	cvSaveImage("Proba.bmp",Proba);
	cvNamedWindow("Proba",0);
	cvShowImage("Proba", Proba );
	printf("Probas\n");

	//trouve les couleur de fond
	m_background_colors = new std::vector<Color*>();

	ColorHistogram* erasedColorHisto = new ColorHistogram();
	for(int i=7;i<Proba->width-7;i++)
		for(int j=7;j<Proba->height-7;j++)
			if (CV_IMAGE_ELEM(Proba,uchar, j, i)==140)
				erasedColorHisto->add(CV_IMAGE_ELEM(m_modified,uchar, j, i*3),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+1),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+2));

	int background_quantity = m_source->width*m_source->height;
	while(background_quantity > m_source->width*m_source->height / 50)
	{
		Color* background_color = erasedColorHisto->get_most_represented_color_N();
		background_quantity = erasedColorHisto->get_quantity(background_color->get_B(),background_color->get_G(),background_color->get_R(),30);
		background_color->set_num(background_quantity);
		if(background_quantity > m_source->width*m_source->height / 50)
		{
			printf("bg color += %d %d %d\n",background_color->get_B(),background_color->get_G(),background_color->get_R());
			m_background_colors->push_back(background_color);
			erasedColorHisto->remove_color(background_color->get_B(),background_color->get_G(),background_color->get_R(),30);
		}
	}

	delete erasedColorHisto;

	//Effacfe les pixels 4 connectees
	for(int i=7;i<Proba->width-7;i++)
		for(int j=7;j<Proba->height-7;j++)
			if (CV_IMAGE_ELEM(Proba,uchar, j, i)==140)
				erase_area(Proba,j,i);

	cvNamedWindow("Proba",0);
	cvShowImage("Proba", Proba );
	printf("Probas dans une direction\n");
	cvWaitKey(0);

	m_expanded = cvCloneImage(m_source);
	cvZero(m_expanded);
	for(int i=7;i<Proba->width-7;i++)
		for(int j=7;j<Proba->height-7;j++)
			if (CV_IMAGE_ELEM(Proba,uchar, j, i)>30 && CV_IMAGE_ELEM(Proba,uchar, j, i)!=255)
				expand_from_seed(Proba,j,i,35);

	cvNamedWindow("Expanded",0);
	cvShowImage("Expanded", m_expanded);
	cvSaveImage("Extractedwocc.png",m_expanded);
	printf("expanded\n");
	cvWaitKey(0);

	color_check();
	cvSaveImage("Extracted.bmp",m_expanded);

	cvWaitKey(0);
}

void
RoadExtraction::send_pixels(Histogram* histo, int size, int X_center, int Y_center, int X_radius, int Y_radius)
{
	for(int i=0;i<size;i++)
	{
		int x= X_center-X_radius*(1-2.0*i/size);
		int y= (Y_center-Y_radius*(1-2.0*i/size));
		histo->add_value(CV_IMAGE_ELEM(m_modified,uchar,x ,y*3 ),
					CV_IMAGE_ELEM(m_modified,uchar, x, y*3+1),
					CV_IMAGE_ELEM(m_modified,uchar, x, y*3+2));
	}
}

void
RoadExtraction::erase_area(IplImage* image, int x, int y)
{
	CV_IMAGE_ELEM(image,uchar, x, y)=255;

	std::list<int> xList;
	std::list<int> yList;

	for(int i=-1;i<2;i++)
		for(int j=-1;j<2;j++)
			if(x+i>-1 && y+j>-1 && x+i<image->height && y+j<image->width && (i!=0 || j!=0 ) && CV_IMAGE_ELEM(image,uchar, x+i, y+j)>0 && CV_IMAGE_ELEM(image,uchar, x+i, y+j)!=255)
			{
				xList.push_front(x+i);
				yList.push_front(y+j);
			}
	erase_area(image, xList, yList);
}

void
RoadExtraction::erase_area(IplImage* image, std::list<int>& xList, std::list<int>& yList)
{
	while(!xList.empty())
	{
		int x,y;
		int max = xList.size();
		while (max!=0)
		{
			--max;
			x = xList.front();
			xList.pop_front();
			y = yList.front();
			yList.pop_front();

			if (CV_IMAGE_ELEM(image,uchar, x, y)!=0 && CV_IMAGE_ELEM(image,uchar, x, y)!=255)
			{
				CV_IMAGE_ELEM(image,uchar, x, y)=255;

				for(int i=-1;i<2;i++)
					for(int j=-1;j<2;j++)
						if(x+i>-1 && y+j>-1 && x+i<image->height && y+j<image->width && (i!=0 || j!=0 ) &&
								CV_IMAGE_ELEM(image,uchar, x+i, y+j)>30 && CV_IMAGE_ELEM(image,uchar, x+i, y+j)!=255)
						{
							xList.push_back(x+i);
							yList.push_back(y+j);
						}
			}
		}
	}
}

void
RoadExtraction::expand_from_seed(IplImage* image, int x, int y, int colorDelta)
{
	IplImage* temp = cvCloneImage (m_modified);
	cvZero(temp);

	int b = CV_IMAGE_ELEM(m_modified,uchar, x, y*3);
	int g = CV_IMAGE_ELEM(m_modified,uchar, x, y*3+1);
	int r = CV_IMAGE_ELEM(m_modified,uchar, x, y*3+2);

	CV_IMAGE_ELEM(image,uchar, x, y)=0;
	CV_IMAGE_ELEM(temp,uchar, x, y*3)=255;

	std::list<int> xList;
	std::list<int> yList;

	for(int i=-1;i<2;i++)
		for(int j=-1;j<2;j++)
			if(x+i>-1 && y+j>-1 && x+i<image->height && y+j<image->width && (i!=0 || j!=0 ) &&
					abs(b-CV_IMAGE_ELEM(m_modified,uchar, x+i, y*3+j*3))
					+abs(g-CV_IMAGE_ELEM(m_modified,uchar, x+i, y*3+j*3+1))
					+abs(r-CV_IMAGE_ELEM(m_modified,uchar, x+i, y*3+j*3+2))<colorDelta)
			{
				xList.push_front(x+i);
				yList.push_front(y+j);
			}
	expand_from_seed(image, xList, yList, colorDelta, r, g, b, temp);
	cvReleaseImage(&temp);
}

void
RoadExtraction::expand_from_seed(IplImage* image, std::list<int>& xList, std::list<int>& yList, int colorDelta, int r, int g, int b, IplImage* temp)
{
	bool erase = false;
	while(!xList.empty() && !erase)
	{
		int x,y;
		int max = xList.size();
		while (max!=0 && ! erase)
		{
			--max;
			x = xList.front();
			xList.pop_front();
			y = yList.front();
			yList.pop_front();

			if (CV_IMAGE_ELEM(image,uchar, x, y)==255)
				erase = true;
			else
			if (CV_IMAGE_ELEM(temp,uchar, x, y*3)!=255)
			{
				CV_IMAGE_ELEM(image,uchar, x, y)=0;
				CV_IMAGE_ELEM(temp,uchar, x, y*3)=255;

				for(int i=-1;i<2;i++)
					for(int j=-1;j<2;j++)
						if(x+i>-1 && y+j>-1 && x+i<image->height && y+j<image->width && (i!=0 || j!=0 ) &&
								abs(b-CV_IMAGE_ELEM(m_modified,uchar, x+i, y*3+j*3))
								+abs(g-CV_IMAGE_ELEM(m_modified,uchar, x+i, y*3+j*3+1))
								+abs(r-CV_IMAGE_ELEM(m_modified,uchar, x+i, y*3+j*3+2))<colorDelta)
						{
							xList.push_back(x+i);
							yList.push_back(y+j);
						}
			}
		}
	}
	if(!erase)
	{
		cvAdd(m_expanded,temp,m_expanded);
		cvZero(temp);
	}
}

void
RoadExtraction::mean_shift(int spatialRadius, int colorRadius)
{
	int level = 1;
	IplImage *dst_img;

	//apply mean shift
	dst_img = cvCloneImage (m_modified);
	cvPyrMeanShiftFiltering(m_modified,dst_img,spatialRadius,colorRadius,level,cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,10,1));

	//show result image
	m_modified = cvCloneImage(dst_img);

	cvNamedWindow ("Mean shift", CV_WINDOW_AUTOSIZE);
	cvShowImage ("Mean shift", m_modified);

	cvReleaseImage (&dst_img);
}

void
RoadExtraction::color_check()
{
	cvSaveImage("Extracted_sans_color_check.png",m_expanded);
	ColorHistogram* colorHisto = new ColorHistogram();
	IplImage* test = cvCloneImage(m_expanded);

	//histo des couleurs conservees
	for(int i=7;i<m_expanded->width-7;i++)
		for(int j=7;j<m_expanded->height-7;j++)
			if(CV_IMAGE_ELEM(m_expanded,uchar, j, i*3)!=0)
				colorHisto->add(CV_IMAGE_ELEM(m_modified,uchar, j, i*3),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+1),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+2));

	bool same_color=false;
	for(int ind=0;ind<m_background_colors->size() && !same_color;ind++)
	{
		int outlier_quantity = colorHisto->get_quantity(m_background_colors->at(ind)->get_B(),m_background_colors->at(ind)->get_G(),m_background_colors->at(ind)->get_R(),30);
		printf("road %d back %d\n",outlier_quantity,m_background_colors->at(ind)->get_num());
		if(outlier_quantity < m_modified->width * m_modified->height / 15 || outlier_quantity>m_background_colors->at(ind)->get_num())
		{
			printf("erasing bg\n");
			for(int i=0;i<m_expanded->width;i++)
				for(int j=0;j<m_expanded->height;j++)
					if(m_background_colors->at(ind)->difference(CV_IMAGE_ELEM(m_modified,uchar, j, i*3),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+1),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+2))<30)
					{
						CV_IMAGE_ELEM(test,uchar, j, i*3)=0;
						CV_IMAGE_ELEM(test,uchar, j, i*3+1)=0;
						CV_IMAGE_ELEM(test,uchar, j, i*3+2)=0;
					}
			colorHisto->remove_color(m_background_colors->at(ind)->get_B(),m_background_colors->at(ind)->get_G(),m_background_colors->at(ind)->get_R(),30);
			same_color = false;
		}
		else
		{
			colorHisto->remove_color(m_background_colors->at(ind)->get_B(),m_background_colors->at(ind)->get_G(),m_background_colors->at(ind)->get_R(),30);
			colorHisto->remove_color(m_background_colors->at(ind)->get_B(),m_background_colors->at(ind)->get_G(),m_background_colors->at(ind)->get_R(),30);
			printf("bg & road of same color\n");
		}
	}

	Color* mostPresent = colorHisto->get_most_represented_color_N();
	if(!same_color)
	{
		while(colorHisto->get_quantity(mostPresent->get_B(),mostPresent->get_G(),mostPresent->get_R(),30)>m_source->height*m_source->width/25)
		{
			for(int i=7;i<m_expanded->width-7;i++)
				for(int j=7;j<m_expanded->height-7;j++)
					if(mostPresent->difference(CV_IMAGE_ELEM(m_modified,uchar, j, i*3),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+1),CV_IMAGE_ELEM(m_modified,uchar, j, i*3+2))<30)
					{
						CV_IMAGE_ELEM(test,uchar, j, i*3)=255;
						CV_IMAGE_ELEM(test,uchar, j, i*3+1)=0;
						CV_IMAGE_ELEM(test,uchar, j, i*3+2)=0;
					}
			colorHisto->remove_color(mostPresent->get_B(),mostPresent->get_G(),mostPresent->get_R(),30);
			printf("road color += b %d g %d r %d\n",mostPresent->get_B(),mostPresent->get_G(),mostPresent->get_R());
		}
	}

	delete mostPresent;
	delete colorHisto;

	cvNamedWindow("test",0);
	cvShowImage("test",test);
	cvWaitKey(0);
	m_expanded = cvCloneImage(test);
}

void
RoadExtraction::thin_operator()
{
	for(int i=1;i<m_expanded->width-1;i++)
		for(int j=1;j<m_expanded->height-1;j++)
			if(is_delete_candidate(j,i))
				CV_IMAGE_ELEM(m_expanded,uchar, j,i*3)=0;
}

bool
RoadExtraction::is_delete_candidate(int x,int y)
{
	int Xorder[8] = {-1,-1,-1,0,1,1,1,0};
	int Yorder[8] = {-1,0,1,1,1,0,-1,-1};

	int numb = 0;
	bool isOver = false;
	if (CV_IMAGE_ELEM(m_expanded,uchar, x, y*3)!=0)
	{
		for(int i=0;i<8;i++)
			if(x+Yorder[i]>0 && x+Yorder[i]<m_expanded->height && y+Xorder[i]>0 && y+Xorder[i]<m_expanded->width)
			{
				if(CV_IMAGE_ELEM(m_expanded,uchar, x+Yorder[i], (y+Xorder[i])*3)!=0)
					++numb;
				if(numb!=0 && CV_IMAGE_ELEM(m_expanded,uchar, x+Yorder[i], (y+Xorder[i])*3)==0)
					isOver=true;
				if(isOver && CV_IMAGE_ELEM(m_expanded,uchar, x+Yorder[i], (y+Xorder[i])*3)!=0)
					return false;
			}
	}
	else
		return false;
	if (numb>1)
		return true;
	else
		return false;
}