#ifndef ROADEXTRACTION_HPP_
#define ROADEXTRACTION_HPP_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <list>
#include <vector>
#include "Histogram.hpp"
#include "Color.hpp"
#include "ColorHistogram.hpp"

void SetMatElem(CvMat *mat, int row, int col, int channel, int val);

class RoadExtraction
{
public:
	RoadExtraction();
	~RoadExtraction();

public:
	void load_image(const char * filename);
	void show_source();
	void show_modified();
	void init_modified();
	void create_histogram();
	void mean_shift(int spatialRadius, int colorRadius);
	void send_pixels(Histogram* histo, int size, int X_center, int Y_center, int X_radius, int Y_radius);
	void erase_area(IplImage* image, int x, int y);
	void expand_from_seed(IplImage* image, int x, int y, int colorDelta);
	void color_check();

private:
	void thin_operator();
	bool is_delete_candidate(int x,int y);
	void hough_lines();

private:
	void erase_area(IplImage* image, std::list<int>& xList, std::list<int>& yList);
	void expand_from_seed(IplImage* image, std::list<int>& xList, std::list<int>& yList, int colorDelta, int r, int g, int b, IplImage* temp);


private:
	IplImage* m_source;
	IplImage* m_modified;
	IplImage* m_expanded;
	Color* m_background_color;
	std::vector<Color*>* m_background_colors;
	int m_background_quantity;
};

#endif
