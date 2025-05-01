#include "rail.h"
#include "scene.h"
#include "renderer.h"

static constexpr const char* RAIL_TEXTRUE = "asset\\texture\\white.png";
static constexpr const int RAIL_LENGTH = 160;
static constexpr const float RAIL_WIDTH = 0.1f;
static constexpr const F3 GRAVITY = { 0.0f, -15.0f, 0.0f };
static constexpr const float AIR_FRICTTION = 0.3f;
static constexpr const float SHOOT_MAX_FORCE = 100.0f;
static constexpr const float SIMPLING_TIME = 10.0f;

Rail::Rail(const GameObject* target, const F3& position, const F3& size, const Quaternion& rotate, const std::string& scope) 
	: target(target), GameObject(size, position, rotate), vertexes(std::vector<VERTEX>(RAIL_LENGTH * 2))
{
	texture = LoadTexture(RAIL_TEXTRUE, scope);
}

void Rail::Update()
{
	
}

void Rail::Draw()
{
	GetRenderer()->SetDepthState(DEPTH_STATE_NO_WRITE);
	GetRenderer()->SetRasterizerState(RASTERIZER_STATE_CULL_NONE);

	DrawPolygon(texture, vertexes.data(), RAIL_LENGTH * 2, TOPOLOGY_TRIANGLESTRIP, position, size, rotate, color);
	
	GetRenderer()->SetRasterizerState(RASTERIZER_STATE_CULL_NONE);
	GetRenderer()->SetDepthState(DEPTH_STATE_ENABLE);
}

void Rail::SetDirection(const F3& d)
{
	direction = d;
}

void Rail::UpdatePath()
{
	float sampleDeltaTime = SIMPLING_TIME / RAIL_LENGTH;
	F3 velocity = direction * power;
	if (power == 0.0f) {
		velocity *= SHOOT_MAX_FORCE;
	}
	F3 current = target->position;
	F3 lastpPosition = current - velocity * sampleDeltaTime;
	F3 offset = Rotate({ RAIL_WIDTH }, scene->GetCurrentCamera()->GetRotate());
	for (int i = 0; i < RAIL_LENGTH; i++) {
		float scalar = 1.0f - min((float)i / (RAIL_LENGTH - 1.0f), 1.0f);

		vertexes[i * 2].position = current - offset;
		vertexes[i * 2 + 1].position = current + offset;

		vertexes[i * 2].texCoord = { 0.0f, scalar };
		vertexes[i * 2 + 1].texCoord = { 1.0f, scalar };

		vertexes[i * 2].diffuse = { 1.0f, 8.0f, 4.0, scalar * 0.7f };
		vertexes[i * 2 + 1].diffuse = { 1.0f, 8.0f, 4.0f, scalar * 0.7f };

		lastpPosition = current;
		current += velocity * sampleDeltaTime;
		velocity += GRAVITY * sampleDeltaTime;
		velocity -= velocity * max(min(AIR_FRICTTION * sampleDeltaTime, 1.0f), 0.0f);
	}
}

void Rail::SetPower(const float p)
{
	power = p;
}
