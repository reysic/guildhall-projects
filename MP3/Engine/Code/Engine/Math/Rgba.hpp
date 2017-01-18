#pragma once
class Rgba
{
public:
	static const Rgba WHITE;
	static const Rgba BLACK;
	static const Rgba RED;
	static const Rgba GREEN;
	static const Rgba BLUE;
	static const Rgba PINK;
	static const Rgba YELLOW;
	static const Rgba LTGRAY;

	Rgba();
	Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	unsigned char m_red;
	unsigned char m_green;
	unsigned char m_blue;
	unsigned char m_alpha;

	void Rgba::SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	friend bool operator==(const Rgba& lhs, const Rgba& rhs);
};