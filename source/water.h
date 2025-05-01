#ifndef _WATER_H
#define _WATER_H
#include "ground.h"
#include "gameObjectAudio.h"


class Water : public Ground {
private:
	GameObjectAudio* sound;
	bool ballInside = false;
	Progress soundT{ 3000.0f, false };
public:
	Water(
		const F3& position = {},
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
	void Update() override;
	void OnCollision(GameObject* gameObject, const std::list<COLLISION_PAIR>& pairs) override;
};

#endif
