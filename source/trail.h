#ifndef _TRAIL_H
#define _TRAIL_H

#include "gameObject.h"
#include "progress.h"
#include <queue>
#include "gameObjectAudio.h"
using namespace MG;
using namespace MG::Collision;

class Trail : public GameObject {
private:
	const GameObject* target;
	Texture* texture;
	std::queue<F3> points;
	std::vector<VERTEX> vertexes;
	Model* beam;
	GameObjectAudio* chargeSound;
	GameObjectAudio* beamSound;
	bool chargePlayed = false;
	bool beamPlayed = false;
public:
	bool inSuper = false;
	bool useBeam = false;
	Progress beamT{ 300.0f , false };
	
	Trail(
		const GameObject* target,
		const F3& position = {},
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
	~Trail();
	void Update() override;
	void Draw() override;
};

#endif
