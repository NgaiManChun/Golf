#include "water.h"
#include "ball.h"

static constexpr const char* WATER_MODEL = "asset\\model\\water.mgm";
static constexpr const char* WATER_COLLISION = "asset\\model\\water_collision.mgi";
static constexpr const char* WATER_SOUND = "asset\\sound\\water.wav";

Water::Water(const F3& position, const F3& size, const Quaternion& rotate, const std::string& scope) : Ground(position, size, rotate)
{
	model = LoadModel(WATER_MODEL, scope);

	// “–‚½‚è”»’è’Ç‰Á
	MGObject mgi = LoadMGO(WATER_COLLISION);
	ARRANGEMENT* arrangement = GetArrangementByMGObject(mgi);
	AddCollisionUnits(arrangement);
	mgi.Release();

	sound = new GameObjectAudio(LoadAudio(WATER_SOUND), false);

	UpdateWorldCollisionUnits();
	color.w = 0.5f;
	
}

void Water::Update()
{
	soundT.IncreaseValue(GetDeltaTime());
}

void Water::OnCollision(GameObject* gameObject, const std::list<COLLISION_PAIR>& pairs)
{
	if (gameObject) {
		if (typeid(*gameObject) == typeid(Ball) && soundT == 1.0f) {
			sound->Play();
			soundT = 0.0f;
		}
	}
	
}
