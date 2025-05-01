#include "ground.h"
#include "scene.h"

Ground::Ground(const F3& position, const F3& size, const Quaternion& rotate, const std::string& scope) : GameObject(size, position, rotate)
{
}

void Ground::Update()
{
	if (isSeeThrough) {
		seeThrough.IncreaseValue(GetDeltaTime());
	}
	else {
		seeThrough.IncreaseValue(-GetDeltaTime());
	}
	isSeeThrough = false;

	color.w = (1.0f - seeThrough * 0.5f);

	// 位置更新
	position += velocity * GetDeltaTime() * 0.001f;

	// 当たり判定更新
	UpdateWorldCollisionUnits();
}

void Ground::Draw()
{
	DrawModel(model, position, size, rotate, color);
}

void Ground::OnCollision(GameObject* gameObject, const std::list<COLLISION_PAIR>& pairs)
{
	if (!gameObject) {
		for (const auto& pair : pairs) {
			if (pair.b->GetName() == "seeThrough") {
				isSeeThrough = true;
				break;
			}
		}
	}

}
