#ifndef _CRYSTAL_H
#define _CRYSTAL_H

#include "gameObject.h"
#include "progress.h"
#include "gameObjectAudio.h"
using namespace MG;
using namespace MG::Collision;

class Crystal : public GameObject {
protected:
	Model* model;
	bool isSeeThrough = false;
	Progress seeThrough{ 500.0f, false };
	Progress rotateT{ 3000.0f, true };
	Progress destoryT{ 1000.0f, false };
	bool destory = false;
	GameObjectAudio* sound;
public:
	Crystal(
		const F3& position = {},
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
	~Crystal();
	void Update() override;
	void Draw() override;
	void OnCollision(GameObject* gameObject, const std::list<COLLISION_PAIR>& pairs) override;
	bool IsDestory();
};

#endif