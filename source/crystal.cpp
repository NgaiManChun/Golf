#include "crystal.h"
#include "ball.h"
#include "scene.h"

constexpr const char* CRYSTAL_MODEL = "asset\\model\\crystal.mgm";
constexpr const char* CRYSTAL_COLLISION = "asset\\model\\crystal_collision.mgi";
constexpr const char* CRYSTAL_SOUND = "asset\\sound\\crystal.wav";

Crystal::Crystal(const F3& position, const F3& size, const Quaternion& rotate, const std::string& scope) : GameObject(size, position, rotate)
{
	model = LoadModel(CRYSTAL_MODEL, scope);

	// “–‚½‚è”»’è’Ç‰Á
	MGObject mgi = LoadMGO(CRYSTAL_COLLISION);
	ARRANGEMENT* arrangement = GetArrangementByMGObject(mgi);
	AddCollisionUnits(arrangement);
	mgi.Release();
	sound = new GameObjectAudio(LoadAudio(CRYSTAL_SOUND), false);


	UpdateWorldCollisionUnits();

	color.w = 0.7f;
}

Crystal::~Crystal() {
	if (sound) {
		delete sound;
		sound = 0;
	}
}

void Crystal::Update()
{
	rotate = Quaternion::AxisYRadian(rotateT * 2.0f * PI);


	color.w = 0.7f * (1.0f - destoryT);
	if (destoryT == 1.0f) {
		scene->DeleteGameObject(this);
	}
	if (destory) {
		destoryT.IncreaseValue(GetDeltaTime());
	}
	rotateT.IncreaseValue(GetDeltaTime());
}

void Crystal::Draw()
{
	
	DrawModel(model, position + F3{ 0.0f, 2.0f, 0.0f } * destoryT, size * (1.0f + destoryT), rotate, color);
}

void Crystal::OnCollision(GameObject* gameObject, const std::list<COLLISION_PAIR>& pairs)
{
	if (!gameObject) {
		for (const auto& pair : pairs) {
			if (pair.b->GetName() == "seeThrough") {
				isSeeThrough = true;
				break;
			}
		}
	}
	else if (typeid(*gameObject) == typeid(Ball) && !destory) {
		destory = true;
		sound->Play();
	}
}

bool Crystal::IsDestory()
{
	return destory;
}
