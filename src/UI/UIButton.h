#pragma once
#include "UI/UIElement.h"
#include "UI/UIText.h"
#include <SFML/Graphics.hpp>
#include <chrono>

class UIButton : public UIElement
{
	void update(const float& dt) override;
public:
	bool was_pressed();
	bool is_pressed() const;
	UIButton(const std::string& button_name, const vec2f& scale, uint8_t font_size = 0);
	UIButton(const std::wstring& button_name, const vec2f& scale, uint8_t font_size);
	virtual ~UIButton();

	static bool m_AnyPressed;
private:
	uint8_t m_FontSize = 0;
	bool m_Pressed = false;
	bool m_ButtonEventHandled = false;
	std::chrono::milliseconds m_PressTime;
	std::string m_ButtonAsset;
	std::wstring m_ButtonName;
	UIText* m_ButtonText = nullptr;
};