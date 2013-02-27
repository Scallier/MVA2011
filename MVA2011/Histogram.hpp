#ifndef HISTOGRAM_HPP_
#define HISTOGRAM_HPP_

#include "Color.hpp"

class Histogram
{
public:
	Histogram(int blue, int green, int red, int size);
	~Histogram();
	void add_value(int blue, int green, int red);
	int get_max_value();
	void show_histogram();
	void clear();


private:
	int m_height, m_current_value;
	int* m_histo_values;
	Color* m_initial_color;
};

#endif
