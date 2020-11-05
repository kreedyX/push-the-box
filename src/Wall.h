#pragma once
#include "Entity.h"

class Wall : public Entity
{
private:
	void update(const float& dt) override;
public:
	Wall(const vec2f& position_px);
	virtual ~Wall();
};