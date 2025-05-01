#include "button.h"


constexpr const char* BUTTON_MODEL = "asset\\model\\button.mgm";
constexpr const char* BUTTON_COLLISION = "asset\\model\\button_collision.mgi";

Button::Button(const F3& position, const F3& size, const Quaternion& rotate, const std::string& scope) : Ground(position, size,rotate, scope)
{
	
	model = LoadModel(BUTTON_MODEL, scope);

	// “–‚½‚è”»’è’Ç‰Á
	MGObject mgi = LoadMGO(BUTTON_COLLISION);
	ARRANGEMENT* arrangement = GetArrangementByMGObject(mgi);
	AddCollisionUnits(arrangement);
	mgi.Release();

	UpdateWorldCollisionUnits();
}
