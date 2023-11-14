#pragma once

#include "olcPixelGameEngine.h"

namespace gui {

    class Manager;

    class Element
    {
    public:
        Element(olc::vf2d pos, olc::vf2d size, Manager& manager);
        virtual ~Element();

        virtual void Update(olc::PixelGameEngine* pge) = 0;
        virtual void Draw(olc::PixelGameEngine* pge) = 0;

        olc::vf2d Position;
        olc::vf2d Size;
        Manager& GuiManager;

        bool IsHovered = false;
        bool IsPressed = false;
        bool IsReleased = false;
        bool IsSelected = false;
        bool IsHidden = false;

    protected:
        enum class State
        {
            Normal,
            Hovered,
            Pressed,
            Released,
            Selected,
            Hidden
        } m_State = State::Normal;
    };

} // namespace gui
