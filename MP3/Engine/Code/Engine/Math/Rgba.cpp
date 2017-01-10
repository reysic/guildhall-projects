#include "Engine/Math/Rgba.hpp"
#include "Engine/MathUtils.hpp"
const Rgba Rgba::WHITE(255, 255, 255, 255);

const Rgba Rgba::BLACK(0, 0, 0, 255);

const Rgba Rgba::RED(255, 0, 0, 255);

const Rgba Rgba::GREEN(0, 255, 0, 255);

const Rgba Rgba::BLUE(0, 0, 255, 255);

const Rgba Rgba::PINK(255, 0, 255, 255);

const Rgba Rgba::YELLOW(255, 255, 0, 255);

const Rgba Rgba::LTGRAY(190, 190, 190, 255);

Rgba::Rgba()
{
	m_red = 0;
	m_green = 0;
	m_blue = 0;
	m_alpha = 0;
}

Rgba::Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	m_red = red;
	m_green = green;
	m_blue = blue;
	m_alpha = alpha;
}


void Rgba::SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	m_red = red;
	m_green = green;
	m_blue = blue;
	m_alpha = alpha;
}

bool operator==(const Rgba& lhs, const Rgba& rhs)
{
	if (lhs.m_red == rhs.m_red && 
		lhs.m_green == rhs.m_green &&
		lhs.m_blue == rhs.m_blue &&
		lhs.m_alpha == rhs.m_alpha)
		return true;
	return false;
}
