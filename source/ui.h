#ifndef _UI_H
#define _UI_H

#include "gameObject.h"
#include "progress.h"
#include "gameObjectText.h"
using namespace MG;
using namespace MG::Collision;

class UI : public GameObject {
protected:
	GameObjectText* strokesLabel;
	GameObjectText* superLabel;
	GameObjectText* controlLabel;
	int crystalNum = 0;
	Model* crystalModel;
	Progress crystalRotateT{ 3000.0f, true };
public:
	UI(
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
	~UI();
	void Update() override;
	void Draw() override;
	void SetStrokeNum(const int num);
	void SetCrystalNum(const int num);
	void SetSuper(bool v);
};

#endif