#ifndef _GROUND_H
#define _GROUND_H

#include "gameObject.h"
#include "progress.h"
using namespace MG;
using namespace MG::Collision;

class Ground : public GameObject {
protected:
	Model* model;
	bool isSeeThrough = false;
	Progress seeThrough{ 500.0f, false };
public:
	Ground(
		const F3& position = {},
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
	void Update() override;
	void Draw() override;
	void OnCollision(GameObject* gameObject, const std::list<COLLISION_PAIR>& pairs) override;
};

#endif