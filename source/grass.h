#ifndef _GRASS_H
#define _GRASS_H
#include "ground.h"


class Grass : public Ground {
public:
	Grass(
		const F3& position = {},
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
};

class GrassZ : public Grass {
private:
	Progress t{ 1000.0f, false };
	int sign = 1;
public:
	GrassZ(
		const F3& position = {},
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
	void Update() override;
};

#endif
