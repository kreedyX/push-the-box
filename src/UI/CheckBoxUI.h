#pragma once
#include "ElementUI.h"
#include <functional>

class CheckBoxUI : public ElementUI
{
public:
	CheckBoxUI(const std::string& sprite, const std::string& selection_glyph, vec2f scale = { 1.f, 1.f });
	virtual ~CheckBoxUI() = default;

	void on_change(std::function<void(bool)> on_change_function);
	void select(bool selected = true);
	void disable(bool disabled = true);
	inline bool is_selected() const { return m_IsSelected; }
	inline bool is_disabled() const { return m_IsDisabled; }
private:
	void update(const float& dt) override;

	ElementUI* m_SelectionGlyph;
	std::function<void(bool)> m_OnChangeFunction = nullptr;
	bool m_IsSelected = false;
	bool m_IsDisabled = false;
	bool m_Released = true;
public:
	bool m_CanBeUnchecked = true;
};