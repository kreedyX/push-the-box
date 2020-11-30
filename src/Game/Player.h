#pragma once
#include "Entity/Entity.h"
#include "TileMap.h"
#include "Box.h"
#include "Entity/Animation.h"
#include <unordered_map>

using vec2i = sf::Vector2i;

enum class Direction {
	Up, Right, Down, Left
};

enum class AnState {
	Standing, MovingUp, MovingRight, MovingDown, MovingLeft
};

class Player : public Entity
{
public:
	friend class TileMap;
	Player(TileMap* tile_map);
	virtual ~Player();
private:
	void update(const float& dt) override;
	void walk(Direction direction);
	void animate(AnState an_state);
	bool can_walk(vec2i offset);

	bool m_StopWalking = true;
	std::unordered_map<AnState, std::string> m_AnName;
	AnState m_AnimationState = AnState::Standing;
	Animation* m_Animation = nullptr;
	TileMap* m_TileMap = nullptr;
	Box* m_PushedBox = nullptr;
	vec2f m_DestinationPos;
	vec2u m_TilePosition;
	float m_MovementSpeed = 300.f;
};