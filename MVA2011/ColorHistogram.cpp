#include <stdio.h>

#include "ColorHistogram.hpp"
#include "Color.hpp"

ColorHistogram::ColorHistogram()
{
	m_vect = new std::vector<Color>();
}

ColorHistogram::~ColorHistogram()
{
	delete m_vect;
}

int
ColorHistogram::size()
{
	return m_vect->size();
}

void
ColorHistogram::add(int b, int g, int r)
{
	Color newColor(b,g,r);

	bool incremented = false;
	for(int i=0;i<size();i++)
		if(newColor==m_vect->at(i))
		{
			incremented = true;
			m_vect->at(i).increment();
			i=size();
		}

	if (!incremented)
		m_vect->push_back(newColor);
}

Color*
ColorHistogram::get_most_represented_color_N()
{
	int id = 0;
	int max = -1;
	for(int i=0;i<size();i++)
	{
		if(m_vect->at(i).get_num()>max)
		{
			max = m_vect->at(i).get_num();
			id = i;
		}
	}
	return new Color(m_vect->at(id));
}

Color
ColorHistogram::get_at(int id)
{
	return m_vect->at(id);
}

int
ColorHistogram::get_quantity(int b, int g, int r, int dist)
{
	int nb = 0;
	for(int i=0;(unsigned int)i<m_vect->size();i++)
		if (m_vect->at(i).difference(b,g,r)<=dist)
			nb+=m_vect->at(i).get_num();
	return nb;
}

void
ColorHistogram::remove_color(int b, int g, int r, int dist)
{
	for(int i=0;(unsigned int)i<m_vect->size();i++)
		if (m_vect->at(i).difference(b,g,r)<=dist)
			m_vect->at(i).set_num(0);
}

