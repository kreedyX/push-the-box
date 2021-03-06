#pragma once
#include "GamePlay.h"
#include "Core/Logger.h"
#include "Core/Window.h"
#include "State/StatesManager.h"
#include "Progress.h"
#include "LevelSelection.h"

using KB = sf::Keyboard;
constexpr float A_RATIO = 9.f / 16.f;
constexpr float CTRL_BTNS_SPACING = 0.02f;
const vec2f BTN_SCALE{ 4.2f, 4.2f };

void GamePlay::update(const float& dt)
{	
	m_ElapsedTime += dt;
	if (!m_LevelFinished) {
		if (m_Moves == 0)
			m_ElapsedTime = 0.f;

		vec2f player_pos = m_Player->get_position();
		vec2f player_size = m_Player->get_size();
		vec2f camera_offset = player_pos - m_Camera;

		if (!m_CameraInfo.locked.x) {
			// Right game screen side
			if (camera_offset.x + player_size.x > m_fovWidth - m_Border * A_RATIO)
				m_Camera.x = player_pos.x + player_size.x - m_fovWidth + m_Border * A_RATIO;

			// Left game screen side
			else if (player_pos.x - m_Camera.x < m_Border * A_RATIO)
				m_Camera.x = player_pos.x - m_Border * A_RATIO;
		}

		if (!m_CameraInfo.locked.y) {
			// Up game screen side
			if (camera_offset.y + player_size.y > 1.f - m_Border)
				m_Camera.y = player_pos.y + player_size.y - 1.f + m_Border;

			// Down game screen side
			else if (camera_offset.y < m_Border)
				m_Camera.y = player_pos.y - m_Border;
		}

		// Update timer
		u32 wts = (u32)m_ElapsedTime;
		u32 epds = wts % 60; wts -= epds;
		u32 epdm = wts / 60;
		if (epds != m_ElapsedSeconds || epdm != m_ElapsedMinutes) {
			m_ElapsedSeconds = epds;
			m_ElapsedMinutes = epdm;
			std::wstring m = std::to_wstring(epdm);
			std::wstring s = std::to_wstring(epds);
			if (epdm / 10 == 0u) m = L"0" + m;
			if (epds / 10 == 0u) s = L"0" + s;
			m_Timer->set_text(L"Czas: " + m + L":" + s);
			m_Timer->center_x();
		}

		// Player moves counter
		if (m_PlayerMoves != m_Moves) {
			m_Moves = m_PlayerMoves;
			m_MovesText->set_text(L"Ruchy: " + std::to_wstring(m_Moves));
			m_MovesText->center_x();
		}

		// Undos counter
		if (m_UndoRegister.size() != m_AvaliableUndos) {
			m_AvaliableUndos = (u32)m_UndoRegister.size();
			m_UndosText->set_text(std::to_wstring(m_AvaliableUndos) + L"/" + std::to_wstring(UNDOS_LIMIT));
			m_UndosText->center_x();
		}

		// Player movement control
		if (Window::is_focused()) {
			PlayerControl::get().go_up = KB::isKeyPressed(KB::Up) || m_MoveUp->is_pressed();
			PlayerControl::get().go_down = KB::isKeyPressed(KB::Down) || m_MoveDown->is_pressed();
			PlayerControl::get().go_right = KB::isKeyPressed(KB::Right) || m_MoveRight->is_pressed();
			PlayerControl::get().go_left = KB::isKeyPressed(KB::Left) || m_MoveLeft->is_pressed();
		}

		// Chek if level finished
		if (m_TileMap->m_Storages.size() == m_TileMap->m_StoragesFilled) {
			if (m_LevelNumber != -1) {
				LevelInfo* info = GameProgress::get().get_level_info(m_LevelNumber);
				u32 et = (u32)m_ElapsedTime;
				if (info->completed) {
					if (et < info->time)
						info->time = et;
					if (m_Moves < info->moves)
						info->moves = m_Moves;
				}
				else {
					info->completed = true;
					info->moves = m_Moves;
					info->time = et;
				}
				GameProgress::get().save();
				LevelSelection::m_Levels.at(m_LevelNumber - 1)->refresh();
			}
			m_ElapsedTime = 0.f;
			m_LevelFinished = true;
		}

		// Undo button handle
		if (m_UndoButton->was_pressed()) {
			if (m_Player->get_velocity_px().is_zero() && m_UndoRegister.size() > 0) {

				Undo undo = m_UndoRegister.back();
				vec2f undo_move = (vec2f)undo.m_LastMove * m_TileMap->m_TileSize;
				float speed = 1000.f * m_TileMap->m_TileScale.x;
				m_Player->start_movement(undo_move, speed);
				m_Player->m_TilePosition += undo.m_LastMove;
				m_Player->m_Animation->play_animation(undo.m_Animation, 1);
				m_Player->m_Animation->stop();

				if (undo.m_Box != nullptr) {
					undo.m_Box->start_movement(undo_move, speed);
					vec2u box_pos = undo.m_Box->m_TilePos;
					m_TileMap->m_Map[box_pos.x][box_pos.y] = FLOOR_TILE;
					box_pos += undo.m_LastMove;
					m_TileMap->m_Map[box_pos.x][box_pos.y] = BOX_TILE;
					undo.m_Box->m_TilePos = box_pos;
					undo.m_Box->m_CheckForStorage = true;
				}
				m_PlayerMoves--;
				m_UndoRegister.pop_back();
			}
		}

		// Restart button
		if (m_Restart->was_pressed()) {
			destroy_state();
			StatesManager::get().create_active_state(new GamePlay(m_LevelPath, m_LevelNumber, m_LevelName));
		}
	}
	else {
		// When level finished
		if (m_ElapsedTime / .15f > 1) {
			for (auto& box : m_TileMap->m_Boxes) {
				if (m_FlickState)
					box->set_sprite("box-gold");
				else
					box->set_sprite("box");
			}
			m_Flicks++;
			m_ElapsedTime -= .15f;
			m_FlickState = !m_FlickState;
		}
		if (m_Flicks == 16)
			destroy_state();
	}

	if (m_Exit->was_pressed())
		destroy_state();
}

GamePlay::GamePlay(const std::string& level_path, i32 number, const std::wstring name)
	: m_LevelPath(level_path), m_LevelNumber(number)
{
	m_TileMap = new TileMap();
	if (m_TileMap->load_level(m_LevelPath)) {
		m_LevelName = name;
		if (number > 0)
			m_LevelName = L"LEVEL " + std::to_wstring(number);

		// Initialize UI elements
		m_Background = new ElementUI("gameplay-background", { 1.f, 1.f });
		m_Menu = new ElementUI("gameplay-menu", { 1.5f, 1.5f });
		m_tLevelName = new TextUI(m_LevelName, "joystix", 32);
		m_Timer = new TextUI("Czas: 00:00", "joystix", 36);
		m_MovesText = new TextUI("Ruchy: 0", "joystix", 36);
		m_UndosText = new TextUI("0/" + std::to_string(UNDOS_LIMIT), "joystix", 36);
		m_UndoButton = new ButtonUI("COFNIJ", BTN_SCALE, 32);
		m_MoveUp = new ButtonUI("", BTN_SCALE);
		m_MoveDown = new ButtonUI("", BTN_SCALE);
		m_MoveRight = new ButtonUI("", BTN_SCALE);
		m_MoveLeft = new ButtonUI("", BTN_SCALE);
		m_Restart = new ButtonUI(L"RESTART", { 3.f, 3.f }, 32);
		m_Exit = new ButtonUI(L"WYJD�", { 3.f, 3.f }, 32);

		// Construct background
		make_entity(m_Background);

		// Construct tilemap
		construct_tilemap();

		// Construct UI menu
		m_fovWidth = 1.f - m_Menu->get_size().x;
		m_Menu->set_position({ m_fovWidth, 0.f });
		m_tLevelName->attach_position(m_Menu).center_x(.05f);
		m_Timer->attach_position(m_Menu).center_x(.13f);
		m_MovesText->attach_position(m_Menu).center_x(.18f);
		m_UndoButton->assign_button_sprite("btn-3x1", "btn-3x1-pressed");
		m_UndoButton->attach_position(m_Menu).center_x(.24f);
		m_UndosText->attach_position(m_Menu).center_x(.33f);

		ElementUI* arrow_up = new ElementUI("arrow_up", BTN_SCALE);
		ElementUI* arrow_down = new ElementUI("arrow_down", BTN_SCALE);
		ElementUI* arrow_right = new ElementUI("arrow_right", BTN_SCALE);
		ElementUI* arrow_left = new ElementUI("arrow_left", BTN_SCALE);

		m_MoveUp->assign_button_sprite("btn-1x1", "btn-1x1-pressed");
		m_MoveUp->attach_position(m_Menu).center_x(.41f);
		m_MoveUp->set_symbol(arrow_up);

		float down_y = m_MoveUp->get_position().y + m_MoveUp->get_size().y + CTRL_BTNS_SPACING * 3.f;
		float length_y = (m_MoveUp->get_size().y * 2.f + CTRL_BTNS_SPACING * 3.f) / 2.f;
		float side_y = m_MoveUp->get_position().y + length_y - m_MoveUp->get_size().y / 2.f;
		float left_x = m_MoveUp->get_position().x - m_MoveUp->get_size().x - CTRL_BTNS_SPACING * A_RATIO;
		float right_x = m_MoveUp->get_position().x + m_MoveUp->get_size().x + CTRL_BTNS_SPACING * A_RATIO;

		m_MoveDown->assign_button_sprite("btn-1x1", "btn-1x1-pressed");
		m_MoveDown->attach_position(m_Menu).set_position({ 0.f, down_y }).center_x();
		m_MoveDown->set_symbol(arrow_down);

		m_MoveRight->assign_button_sprite("btn-1x1", "btn-1x1-pressed");
		m_MoveRight->attach_position(m_Menu).set_position({ right_x, side_y });
		m_MoveRight->set_symbol(arrow_right);

		m_MoveLeft->assign_button_sprite("btn-1x1", "btn-1x1-pressed");
		m_MoveLeft->attach_position(m_Menu).set_position({ left_x, side_y });
		m_MoveLeft->set_symbol(arrow_left);

		m_Restart->attach_position(m_Menu).center_x(.75f);
		m_Exit->attach_position(m_Menu).center_x(.85f);

		make_entity(m_Menu);
		make_entity(m_tLevelName);
		make_entity(m_Timer);
		make_entity(m_MovesText);
		make_entity(m_UndoButton);
		make_entity(m_UndosText);
		make_entity(m_MoveUp);
		make_entity(m_MoveDown);
		make_entity(m_MoveRight);
		make_entity(m_MoveLeft);
		make_entity(m_Restart);
		make_entity(m_Exit);

		// Camera setup
		TileMap*& tm = m_TileMap;
		vec2f total_size = (vec2f)tm->m_LevelSize * tm->m_TileSize / Window::size();
		GameCamera::set_cam_info(total_size, { {0.f, 0.f}, {m_fovWidth, 1.f} });
		m_CameraInfo = GameCamera::get_cam_info();
		m_Camera = m_CameraInfo.pos;
		LOG_INFO("Game started on level:", m_LevelPath);
	}
	else {
		LOG_ERROR("Can't open game level:", level_path);
		destroy_state();
	}
}

void GamePlay::construct_tilemap()
{
	for (auto& tile : m_TileMap->m_Tiles)
		make_entity(tile);

	for (auto& storage : m_TileMap->m_Storages)
		make_entity(storage);

	for (auto& box : m_TileMap->m_Boxes)
		make_entity(box);

	m_Player = m_TileMap->m_Player;
	m_Player->init(m_TileMap, &m_UndoRegister, &m_PlayerMoves);
	make_entity(m_Player);
}
