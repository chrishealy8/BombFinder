#pragma once

#include "gui_element.h"
#include "olcPGEX_TTF.h"

namespace gui {

class Button : public Element
{
public:
	Button(olc::vf2d pos, olc::vf2d size, std::string text, Manager& manager);
	virtual ~Button();

	void Update(olc::PixelGameEngine* pge) override;
	void Draw(olc::PixelGameEngine* pge) override;

	std::string Text;
	std::u32string Text32;

	enum class Alignment
	{
		Left,
		Centre,
		Right
	} TextAlignment = Alignment::Centre;
};

} // namespace gui

