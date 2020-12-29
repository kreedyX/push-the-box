#include "LevelEditorMenu.h"
#include "State/StatesManager.h"
#include "Editor.h"

#include <locale>
#include <codecvt>

const sf::Color HEADER_COLOR{ 229, 198, 0, 255 };
const sf::Color TB_TEXT_COLOR{ 229, 198, 0, 255 };
const vec2f BUTTON_SCALE{ 4.5f, 4.5f };
const TextBoxProps TB_PROPS {
	"joystix", 36, TB_TEXT_COLOR, "cursor"
};

void LevelEditorMenu::update(const float& dt)
{
	uint16_t active_layer = layer_id();
	if (active_layer == LE_MENU) {
		if (m_bNew->was_pressed())
			set_main_layer(LE_NEW);
		if (m_bLoad->was_pressed())
			set_main_layer(LE_LOAD);
	}
	else if (active_layer == LE_NEW) {
		//if (m_bCreate->was_pressed() && create_level())
			//destroy_state();
		if (m_bCreate->was_pressed()) {
			StatesManager::get().create_active_state(new Editor("test", {12, 12}));
			destroy_state();
		}
	}

	if (m_bBack->was_pressed())
		if (active_layer != LE_MENU) {
			set_main_layer(LE_MENU);
			m_tbLevelName->clear();
		}
		else destroy_state();
}

bool LevelEditorMenu::create_level()
{
	std::wstring ln_content = m_tbLevelName->get_content();
	std::wstring w_content = m_tbLevelWidth->get_content();
	std::wstring h_content = m_tbLevelHeight->get_content();

	if (ln_content.size() == 0u) return false;
	if (w_content.size() == 0u) return false;
	if (h_content.size() == 0u) return false;

	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	std::string level_name = converter.to_bytes(ln_content);
	vec2u size = { (unsigned)std::stoi(w_content), (unsigned)std::stoi(h_content) };

	if (size.x < 5u || size.y < 5u) return false;

	StatesManager::get().create_active_state(new Editor(level_name, size));
	return true;
}

LevelEditorMenu::LevelEditorMenu()
{
	// Menu layer
	m_Background = new UIElement("header-state", { 1.5f, 1.5f });
	m_HeaderText = new UIText(L"EDYTOR POZIOM�W", "joystix", 80);
	m_bNew = new UIButton(L"NOWY", BUTTON_SCALE, 60);
	m_bLoad = new UIButton(L"WCZYTAJ", BUTTON_SCALE, 60);
	m_bBack = new UIButton(L"WR��", BUTTON_SCALE, 60);

	m_HeaderText->set_tcolor(HEADER_COLOR);
	m_HeaderText->set_position({ 0.f, .017f }).center_x();
	m_bNew->set_position({ 0.f, .35f }).center_x();
	m_bLoad->set_position({ 0.f, .5f }).center_x();
	m_bBack->set_position({ 0.f, 0.85f }).center_x();

	make_entity(m_Background);
	make_entity(m_HeaderText);
	make_entity(m_bNew, LE_MENU);
	make_entity(m_bLoad, LE_MENU);
	make_entity(m_bBack);

	// New level layer
	m_tbLevelName = new UITextBox("level-name", BUTTON_SCALE, TB_PROPS);
	m_tbLevelWidth = new UITextBox("level-size", BUTTON_SCALE, TB_PROPS);
	m_tbLevelHeight = new UITextBox("level-size", BUTTON_SCALE, TB_PROPS);
	m_NewCaption1 = new UIText(L"NAZWA POZIOMU", "joystix", 36);
	m_NewCaption2 = new UIText(L"SZEROKO�� POZIOMU (5-99)", "joystix", 36);
	m_NewCaption3 = new UIText(L"WYSOKO�� POZIOMU (5-99)", "joystix", 36);
	m_bCreate = new UIButton(L"STW�RZ", BUTTON_SCALE, 60);

	m_tbLevelName->set_max_chars(14);
	m_tbLevelWidth->only_numbers(); m_tbLevelHeight->only_numbers();
	m_tbLevelWidth->set_max_chars(2); m_tbLevelHeight->set_max_chars(2);
	m_NewCaption1->set_position({ 0.f, .2f }).center_x();
	m_tbLevelName->set_position({ 0.f, .25f }).center_x();
	m_NewCaption2->set_position({ 0.f, .35f }).center_x();
	m_tbLevelWidth->set_position({ 0.f, .4f }).center_x();
	m_NewCaption3->set_position({ 0.f, .5f }).center_x();
	m_tbLevelHeight->set_position({ 0.f, .55f }).center_x();
	m_bCreate->set_position({ 0.f, .65f }).center_x();

	make_entity(m_tbLevelName, LE_NEW);
	make_entity(m_tbLevelWidth, LE_NEW);
	make_entity(m_tbLevelHeight, LE_NEW);
	make_entity(m_NewCaption1, LE_NEW);
	make_entity(m_NewCaption2, LE_NEW);
	make_entity(m_NewCaption3, LE_NEW);
	make_entity(m_bCreate, LE_NEW);

	set_main_layer(LE_NEW);
	set_main_layer(LE_MENU);
}