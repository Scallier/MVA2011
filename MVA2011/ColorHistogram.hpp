#ifndef COLORHISTOGRAM_H_
#define COLORHISTOGRAM_H_

#include <vector>
#include "Color.hpp"

class ColorHistogram
{
private:
	std::vector<Color>* m_vect;

public:
	ColorHistogram();
	~ColorHistogram();
	int size();
	void add(int b, int g, int r);
	Color* get_most_represented_color_N();
	Color get_at(int id);
	int get_quantity();
	int get_quantity(int b, int g, int r, int dist=0);
	void remove_color(int b, int g, int r, int dist);
};

#endif
