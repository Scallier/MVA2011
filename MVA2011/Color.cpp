#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Color.hpp"

Color::Color():
m_num(1),
m_B(0),
m_G(0),
m_R(0)
{

}

Color::Color(const Color& col):
m_num(col.m_num),
m_B(col.m_B),
m_G(col.m_G),
m_R(col.m_R)
{

}

Color::Color(int blue, int green, int red):
m_num(1),
m_B(blue),
m_G(green),
m_R(red)
{

}

const int
Color::get_B()
{
	return m_B;
}

const int
Color::get_G()
{
	return m_G;
}

const int
Color::get_R()
{
	return m_R;
}

void
Color::add_to_mean( int blue, int green, int red)
{
	m_B=( m_B*m_num+blue)/(m_num+1);
	m_G=( m_G*m_num+green)/(m_num+1);
	m_R=( m_R*m_num+red)/(m_num+1);
    ++m_num;
}

int
Color::difference(int blue,int green,int red)
{
	return abs(blue-m_B)+abs(green-m_G)+abs(red-m_R);
}

int
Color::to_grey_scale()
{
	return (int)(m_B*0.11+m_G*0.59+m_R*0.3);
}

int
Color::to_grey_scale(int blue,int green, int red)
{
	return (int)(blue*0.11+green*0.59+red*0.3);
}

int
Color::get_num()
{
	return m_num;
}

void
Color::set_num(int val)
{
	m_num=val;
}

void
Color::increment()
{
	m_num++;
}

const bool
Color::operator==(const Color& col)
{
	if(col.m_B==m_B && col.m_G==m_G && col.m_R==m_R)
		return true;
	else
		return false;
}
