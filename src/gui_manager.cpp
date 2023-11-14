#include "gui_manager.h"

namespace gui {

	Manager::Manager()
	{}

	void Manager::Update(olc::PixelGameEngine* pge)
	{
		for (auto& element : m_elements)
		{
			element->Update(pge);
		}
	}

	void Manager::Draw(olc::PixelGameEngine* pge)
	{
		for (auto& element : m_elements)
		{
			element->Draw(pge);
		}
	}

	void Manager::AddElement(Element* element)
	{
		m_elements.push_back(element);
	}

	void Manager::SetFont(olc::Font* font)
	{
		Font = font;
	}

} // namespace gui