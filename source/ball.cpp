#include "ball.h"
#include "ground.h"
#include "water.h"
#include "crystal.h"
#include "keyboard.h"
#include "drawToolDX.h"
#include "scene.h"
#include "grass.h"

static constexpr const char* BALL_MODEL = "asset\\model\\ball.mgm";
static constexpr const char* BALL_COLLISION = "asset\\model\\ball_collision.mgi";
static constexpr const char* SHOOT_SOUND = "asset\\sound\\shoot.wav";
static constexpr const char* COLLISION_SOUND = "asset\\sound\\collision.wav";

static constexpr const char* CRYSTAL_SOUND = "asset\\sound\\collision.wav";

static constexpr const F3 GRAVITY = { 0.0f, -15.0f, 0.0f };
static constexpr const float AIR_FRICTTION = 0.3f;
static constexpr const float VELOCITY_CUT = 13.0f / 60.0f;
static constexpr const int LAYER_GROUND = 2;
static constexpr const int LAYER_WATER = 3;
static constexpr const int LAYER_ITEM = 4;

Ball::Ball(const F3& position, const F3& size, const Quaternion& rotate, const std::string& scope) : GameObject(size, position, rotate)
{
	model = LoadModel(BALL_MODEL, scope);

	// 当たり判定追加
	MGObject mgi = LoadMGO(BALL_COLLISION);
	ARRANGEMENT* arrangement = GetArrangementByMGObject(mgi);
	AddCollisionUnits(arrangement);
	mgi.Release();

	shootSound = new GameObjectAudio(LoadAudio(SHOOT_SOUND), false);
	collisionSound = new GameObjectAudio(LoadAudio(COLLISION_SOUND), false);

	friction = AIR_FRICTTION;

	UpdateWorldCollisionUnits();
}

Ball::~Ball()
{
	if (shootSound) {
		delete shootSound;
		shootSound = 0;
	}
	if (collisionSound) {
		delete collisionSound;
		collisionSound = 0;
	}
	
}

void Ball::Update()
{
	float deltaTimeSec = GetDeltaTime() * 0.001f;

	if (shootFinish) {
		static F3 target = { 0.0f, 250.0f, 0.0f };
		float distance = Distance(position, target);
		if (distance < 50.0f) {
			inSuper = true;
		}
		if (inSuper) {
			if (superChrage == 1.0f) {
				position = F3{ 0.0f, 250.0f, 0.0f } + F3{ ((float)rand()) / RAND_MAX * 5.0f , ((float)rand()) / RAND_MAX * 5.0f, ((float)rand()) / RAND_MAX * 5.0f };
				beamChrage.IncreaseValue(GetDeltaTime());
				if (beamChrage == 1.0f) {
					trail->useBeam = true;
				}
			}
			else {
				if (Distance(position, {}) > 1000.0f) {
					target = F3{ ((float)rand()) / RAND_MAX, ((float)rand()) / RAND_MAX, ((float)rand()) / RAND_MAX };
					target *= F3{ 500.0f, 30.0f, 500.0f };
					target *= F3{ (rand() % 2) ? 1.0f : -1.0f, (rand() % 2) ? 1.0f : -1.0f, (rand() % 2) ? 1.0f : -1.0f };
					target.y += 250.0f;
					velocity = Normalize(target - position) * 5000.0f;
				}

				// 位置更新
				position += velocity * deltaTimeSec;

				// 当たり判定更新
				UpdateWorldCollisionUnits();

				color = { 1.0f, 2.0f, 2.0f, 1.0f };
				if (trail) trail->inSuper = true;

				superChrage.IncreaseValue(GetDeltaTime());
			}
		}
	}
	if (superChrage == 1.0f) return;
	if (DistanceSquare(velocity, {}) < VELOCITY_CUT * VELOCITY_CUT) {
		velocity = {};

	}



	if (rideOn) {
		velocity = rideOn->velocity;
	}

	// 位置更新
	position += velocity * deltaTimeSec;

	// 当たり判定更新
	UpdateWorldCollisionUnits();

	if (!rideOn) {
		// 摩擦
		velocity -= velocity * max(min(friction * deltaTimeSec, 1.0f), 0.0f);

		// 重力
		velocity += GRAVITY * deltaTimeSec;
		friction = AIR_FRICTTION;
	}
}

void Ball::Draw()
{
	DrawModel(model, position, size, rotate, color);
}

void Ball::OnCollision(GameObject* gameobject, const std::list<COLLISION_PAIR>& pairs)
{
	
	if (gameobject->layer == LAYER_GROUND) {
		for (const auto& pair : pairs) {
			if (typeid(*pair.b) == typeid(Box)) {
				Box* box = (Box*)pair.b;
				F3 normal = box->GetNormal(position);
				float dot = -Dot(velocity, normal);
				if (dot > 0) {
					velocity += normal * dot * 1.5f;
					if (dot > 30.0f) {
						collisionSound->Play();
					}
				}
				if (Dot(gameobject->velocity, normal) >= 0.0f) {
					if (DistanceSquare(velocity, {}) < DistanceSquare(gameobject->velocity, {}) * 0.3f) {
						velocity = gameobject->velocity;
						rideOn = gameobject;
					}
					else {
						velocity += gameobject->velocity * GetDeltaTime() * 0.001f;
					}
				}
			}
		}
		friction = max(friction, 1.5f);
	}
	else if (gameobject->layer == LAYER_WATER) {
		if (!false) {
			outCourse.IncreaseValue(GetDeltaTime());
		}
		
	}
	else if (typeid(*gameobject) == typeid(Crystal)) {
		if (!((Crystal*)gameobject)->IsDestory()) {
			AddCrystal();
		}
	}
}

bool Ball::IsStop()
{
	return rideOn || DistanceSquare(velocity, {}) * 0.5f < VELOCITY_CUT * VELOCITY_CUT;
}

void Ball::AddForce(const F3& force)
{
	if (rideOn) {
		velocity = {};
		rideOn = nullptr;
	}
	velocity += force;
	shootSound->Play();
	
}

bool Ball::IsOutCourse() const
{
	return outCourse == 1.0f;
}

void Ball::SetOutCourse(float value)
{
	outCourse = value;
}

void Ball::SavePosition()
{
	lastPosition = position;
}

void Ball::RestorePosition()
{
	position = lastPosition + F3{ 0.0f, 0.5f };
	velocity = {};
}

void Ball::ShootFinish()
{
	shootFinish = true;
	AddForce((F3{ 0.0f, 250.0f, 0.0f } - position) * 2.0f);
}
