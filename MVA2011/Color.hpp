#ifndef COLOR_HPP_
#define COLOR_HPP_

class Color
{
private:
	int m_num;
    int m_B, m_G, m_R;

public:
    const int get_B();
    const int get_G();
    const int get_R();

public:
    Color();
    Color(const Color& col);
    Color(int blue, int green, int red);
    void add_to_mean( int blue, int green, int red);
    int to_grey_scale();
    int to_grey_scale(int blue,int green, int red);
    int difference(int blue,int green,int red);
    void increment();
    int get_num();
    void set_num(int val);
    const bool operator==(const Color& col);

};

#endif
