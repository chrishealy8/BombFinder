#include "gui_button.h"
#include "gui_manager.h"

namespace gui {

	Button::Button(olc::vf2d pos, olc::vf2d size, std::string text, Manager& manager)
		: Element(pos, size, manager)
		, Text(text)
	{
		Text32 = std::u32string(Text.begin(), Text.end());
	}

	Button::~Button()
	{}

	void Button::Update(olc::PixelGameEngine* pge)
	{
		IsHovered = false;
		IsPressed = false;
		IsReleased = false;

		if (IsSelected)
		{
			m_State = State::Selected;
			return;
		}

		if (m_State == State::Hidden)
			return;
		
		olc::vf2d mousePos = pge->GetMousePos();

		if (mousePos.x >= Position.x && mousePos.x <= Position.x + Size.x &&
			mousePos.y >= Position.y && mousePos.y <= Position.y + Size.y)
		{
			if (pge->GetMouse(olc::Mouse::LEFT).bPressed || pge->GetMouse(olc::Mouse::LEFT).bHeld)
			{
				m_State = State::Pressed;
				IsPressed = true;
			}
			else if (pge->GetMouse(0).bReleased)
			{
				m_State = State::Released;
				IsReleased = true;
			}
			else
			{
				m_State = State::Hovered;
				IsHovered = true;
			}
		}
		else
		{
			m_State = State::Normal;
		}
	}

	void Button::Draw(olc::PixelGameEngine* pge)
	{
		olc::Pixel fillColour = GuiManager.NormalColour;
		olc::Pixel textColour = GuiManager.TextColour;

		switch (m_State)
		{
		case State::Hovered:
		case State::Selected:
			fillColour = GuiManager.HoveredColour;
			textColour = GuiManager.HoveredTextColour;
			break;
		case State::Pressed:
			fillColour = GuiManager.PressedColour;
			break;
		}

		pge->FillRect(Position, Size, fillColour);
		pge->DrawRect(Position, Size, GuiManager.BorderColour);

		if (GuiManager.Font == nullptr)
		{
			pge->DrawString(Position, Text, textColour);
		}
		else
		{
			olc::vf2d btnSize = Size;
			olc::FontRect text = GuiManager.Font->GetStringBounds(Text32);
			olc::vf2d offset = olc::vf2d{ btnSize.x - text.size.x, btnSize.y - text.size.y } / 2 - text.offset;
			GuiManager.Font->DrawString(Text32, Position + offset, textColour);
		}
	}
}
