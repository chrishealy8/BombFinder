#pragma once

#include "gui_element.h"
#include "olcPGEX_TTF.h"

namespace gui {

	class Manager
	{
	public:
		Manager();

		void Update(olc::PixelGameEngine* pge);
		void Draw(olc::PixelGameEngine* pge);
		void AddElement(Element* element);
		void SetFont(olc::Font* font);

		olc::Pixel TextColour = olc::WHITE;
		olc::Pixel HoveredTextColour = olc::BLACK;
		olc::Pixel NormalColour = olc::VERY_DARK_GREY;
		olc::Pixel HoveredColour = olc::GREY;
		olc::Pixel PressedColour = olc::DARK_GREY;
		olc::Pixel BorderColour = olc::WHITE;

		olc::Font* Font = nullptr;

	private:
		std::vector<Element*> m_elements;
	};

} // namespace gui

