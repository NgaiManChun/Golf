#ifndef _RAIL_H
#define _RAIL_H

#include "gameObject.h"
#include "progress.h"
using namespace MG;
using namespace MG::Collision;

class Rail : public GameObject {
private:
	const GameObject* target;
	Texture* texture;
	std::vector<VERTEX> vertexes;
	F3 direction = {};
	float power = 0.0f;

public:
	Rail(
		const GameObject* target,
		const F3& position = {},
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
	void Update() override;
	void Draw() override;
	void SetDirection(const F3& d);
	void UpdatePath();
	void SetPower(const float p);
};

#endif
