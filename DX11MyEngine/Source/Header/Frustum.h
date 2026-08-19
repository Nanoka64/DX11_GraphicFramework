#pragma once
#include <DirectXCollision.h>

class Frustum
{
private:

public:
	bool Intersects(const CollInData_AABB& _other);
};

