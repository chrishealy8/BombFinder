#include "gui_element.h"
#include "gui_manager.h"

namespace gui {

    Element::Element(olc::vf2d pos, olc::vf2d size, Manager& manager)
        : Position(pos), Size(size), GuiManager(manager)
    {
        manager.AddElement(this);
    }

    Element::~Element()
    {}

} // namespace gui
