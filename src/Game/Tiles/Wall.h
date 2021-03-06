#pragma once
#include "Entity/Entity.h"

class Wall : public Entity
{
public:
	Wall(vec2f place_pos, vec2u tile_pos, u8 v);
	virtual ~Wall() = default;
private:
	void update(const float& dt) override;
	vec2u m_TilePos;
};