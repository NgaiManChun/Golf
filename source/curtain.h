#ifndef _CURTAIN_H
#define _CURTAIN_H

#include "gameObject.h"
#include "progress.h"
#include "renderer.h"
using namespace MG;
using namespace MG::Collision;

class Curtain : public GameObject {
private:
	RenderTarget* renderTarget;
	Texture* tex;
	Texture* texStar;
	F3 starSize = {};
	Progress t = { 1000.0f, false };
	bool reverse = false;
public:
	Curtain();
	~Curtain();
	void Update() override;
	void Draw() override;
	void Reset();
	void SetReverse(bool reverse);
	bool GetReverse() const;
	float GetTime() const;
};

#endif