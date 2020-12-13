#pragma once
#include "State/State.h"
#include "UI/UIElement.h"

class TileMap;
class Player;

class GamePlay : public State
{
public:
	friend class TileMap;
private:
	void update(const float& dt) override;
public:
	GamePlay(const std::string& level_path);
	virtual ~GamePlay() = default;

private:
	const float m_CameraBorderDistance = 0.4f;
	UIElement* m_Background;
	Player* m_Player = nullptr;
	TileMap* m_TileMap = nullptr;
};