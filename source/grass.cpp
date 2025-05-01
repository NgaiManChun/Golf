#include "grass.h"
#include "scene.h"

constexpr const char* GRASS_MODEL = "asset\\model\\grass.mgm";
constexpr const char* GRASS_COLLISION = "asset\\model\\grass_collision.mgi";

Grass::Grass(const F3& position, const F3& size, const Quaternion& rotate, const std::string& scope) : Ground(position, size, rotate)
{
	model = LoadModel(GRASS_MODEL, scope);

	// “–‚½‚è”»’è’Ç‰Á
	MGObject mgi = LoadMGO(GRASS_COLLISION);
	ARRANGEMENT* arrangement = GetArrangementByMGObject(mgi);
	AddCollisionUnits(arrangement);
	mgi.Release();

	UpdateWorldCollisionUnits();
}



GrassZ::GrassZ(const F3& position, const F3& size, const Quaternion& rotate, const std::string& scope) : Grass(position, size, rotate)
{
	velocity = { 0.0f, 0.0f, 20.0f };
	t = { 4000.0f, false };
	t = 0.5f;
}

void GrassZ::Update()
{
	Grass::Update();

	if (t == 1.0f) {
		sign = -sign;
		t = 0.0f;
	}
	velocity = F3{ 0.0f, 0.0f, 20.0f * sign };
	t.IncreaseValue(GetDeltaTime());
}
