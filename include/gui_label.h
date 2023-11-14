#pragma once

#include "gui_element.h"

#include <string>

namespace gui {

    class Label : public Element
    {
    public:
        enum class Alignment
        {
			Left,
			Centre,
			Right
		} TextAlignment = Alignment::Left;

        Label(olc::vf2d pos, olc::vf2d size, std::string text, Manager& manager, Alignment alignment = Alignment::Left);
        virtual ~Label();

        void Update(olc::PixelGameEngine* pge) override;
        void Draw(olc::PixelGameEngine* pge) override;
        std::string GetText();
        void SetText(std::string text);

        std::string Text;
        std::u32string Text32;
    };

} // namespace gui
