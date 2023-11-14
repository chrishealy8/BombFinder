#include "gui_label.h"
#include "gui_manager.h"

namespace gui {

	Label::Label(olc::vf2d pos, olc::vf2d size, std::string text, Manager& manager, Alignment alignment)
		: Element(pos, size, manager)
		, Text(text)
		, TextAlignment(alignment)
	{
		Text32 = std::u32string(Text.begin(), Text.end());
	}

	Label::~Label()
	{}

	void Label::Update(olc::PixelGameEngine* pge)
	{}

	void Label::Draw(olc::PixelGameEngine* pge)
	{
		if (IsHidden)
			return;

		if (GuiManager.Font == nullptr)
		{
			pge->DrawString(Position, Text, GuiManager.TextColour);
		}
		else
		{
			olc::FontRect text = GuiManager.Font->GetStringBounds(Text32);
			olc::vi2d alignOffset{ 0, 0 };

			if (TextAlignment == Alignment::Centre)
			{
				alignOffset.x = (Size.x - text.size.x) / 2;
			}
			else if (TextAlignment == Alignment::Right)
			{
				alignOffset.x = Size.x - text.size.x;
			}
			GuiManager.Font->DrawString(Text32, Position - text.offset + alignOffset, GuiManager.TextColour);
		}
		//pge->DrawRect(Position, Size, GuiManager.BorderColour);
	}

	std::string Label::GetText()
	{
		return Text;
	}

	void Label::SetText(std::string text)
	{
		Text = text;
		Text32 = std::u32string(Text.begin(), Text.end());
	}

} // namespace gui
