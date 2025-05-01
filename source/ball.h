#ifndef _BALL_H
#define _BALL_H

#include "gameObject.h"
#include "gameObjectAudio.h"
#include "progress.h"
#include "trail.h"
#include <functional>
using namespace MG;
using namespace MG::Collision;

class Ball : public GameObject {
private:
	Model* model;
	float friction = 0.0f;
	F3 extraVelocity = {};
	GameObject* rideOn = nullptr;
	F3 rideOffset = {};
	Progress outCourse = { 700.0f, false };
	F3 lastPosition = {};
	GameObjectAudio* shootSound;
	GameObjectAudio* collisionSound;
	Progress superChrage{ 5000.0f, false };
	Progress beamChrage{ 3000.0f, false };
public:
	bool shootFinish = false;
	bool inSuper = false;
	std::function<void()> AddCrystal;
	Trail* trail;

	Ball(
		const F3& position = {},
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
	~Ball();
	void Update() override;
	void Draw() override;
	void OnCollision(GameObject* gameobject, const std::list<COLLISION_PAIR>& pairs);
	bool IsStop();
	void AddForce(const F3& force);
	bool IsOutCourse() const;
	void SetOutCourse(float value);
	void SavePosition();
	void RestorePosition();
	void ShootFinish();
};

#endif
