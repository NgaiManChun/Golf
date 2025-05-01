#ifndef _BUTTON_H
#define _BUTTON_H

#include "ground.h"

class Button : public Ground {
public:
	Button(
		const F3& position = {},
		const F3& size = { 1.0f, 1.0f, 1.0f },
		const Quaternion& rotate = { 0.0f, 0.0f, 0.0f, 1.0f },
		const std::string& scope = RESOURCE_SCOPE_GOBAL
	);
	/*void Update() override;
	void Draw() override;
	void OnCollision(GameObject* gameObject, const std::list<COLLISION_PAIR>& pairs) override;*/
};

#endif
