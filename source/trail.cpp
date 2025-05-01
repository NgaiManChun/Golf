#include "trail.h"
#include "scene.h"
#include "renderer.h"

static constexpr const char* TRAIL_TEXTRUE = "asset\\texture\\white.png";
static constexpr const char* BEAM_MODEL = "asset\\model\\beam.mgm";
static constexpr const char* CHARGE_SOUND = "asset\\sound\\chrage.wav";
static constexpr const char* BEAM_SOUND = "asset\\sound\\beam.wav";

static constexpr const int TRAIL_LENGTH = 60;
static constexpr const float TRAIL_WIDTH = 1.0f;

Trail::Trail(const GameObject* target, const F3& position, const F3& size, const Quaternion& rotate, const std::string& scope) 
	: target(target), GameObject(size, position, rotate), vertexes(std::vector<VERTEX>(TRAIL_LENGTH * 2))
{
	texture = LoadTexture(TRAIL_TEXTRUE, scope);

	beam = LoadModel(BEAM_MODEL, scope);

	chargeSound = new GameObjectAudio(LoadAudio(CHARGE_SOUND), false);
	beamSound = new GameObjectAudio(LoadAudio(BEAM_SOUND), false);
}

Trail::~Trail() {
	delete chargeSound;
	delete beamSound;
}

void Trail::Update()
{
	float deltaTimeSec = GetDeltaTime() * 0.001f;
	if (points.empty() || DistanceSquare(target->velocity, {}) > 20.0f * 20.0f) {
		points.push(target->position);
	}
	else if(!points.empty()) {
		points.pop();
	}
	
	while (points.size() > TRAIL_LENGTH) {
		points.pop();
	}

	F3 cameraPosition = scene->GetCurrentCamera()->GetPosition();
	F3 cameraFront = scene->GetCurrentCamera()->GetFront();
	std::queue<F3> pointsCopy = points;
	int index = 0;
	F3 lastpPosition = {};
	while (!pointsCopy.empty()) {


		float scalar = min((float)index / ((float)points.size() - 1.0f), 1.0f);
		F3 offset = Normalize(Cross(pointsCopy.front() - lastpPosition, cameraFront)) * TRAIL_WIDTH * 0.5f * scalar;

		if (inSuper) {
			offset *= 1.5f;
		}

		vertexes[index * 2].position = pointsCopy.front() - offset;
		vertexes[index * 2 + 1].position = pointsCopy.front() + offset;

		vertexes[index * 2].texCoord = { 0.0f, scalar };
		vertexes[index * 2 + 1].texCoord = { 1.0f, scalar };

		

		if (inSuper) {
			vertexes[index * 2].diffuse = { 1.0f, 0.2f, 0.0, scalar * 0.9f };
			vertexes[index * 2 + 1].diffuse = { 1.0f, 0.2f, 0.0f, scalar * 0.9f };
		}
		else {
			vertexes[index * 2].diffuse = { 1.0f, 1.0f, 1.0f, scalar };
			vertexes[index * 2 + 1].diffuse = { 1.0f, 1.0f, 1.0f, scalar };
		}

		lastpPosition = pointsCopy.front();
		pointsCopy.pop();
		index++;
	}
	if (inSuper && !chargePlayed) {
		chargeSound->Play();
		chargePlayed = true;
	}
	if (useBeam && !beamPlayed) {
		beamSound->Play();
		beamPlayed = true;
	}
	
}

void Trail::Draw()
{
	
	if (inSuper) {
		GetRenderer()->SetAmbient({1.0f, 1.0f, 1.0f, 1.0f });
		GetRenderer()->SetBlendState(BLEND_STATE_ADD);
	}
	if (useBeam) {
		DrawModel(beam, {}, { 700.0f * beamT, 2000.0f, 700.0f * beamT }, Quaternion::Identity(), { 1.0f, 1.0f, 1.0, 0.9f });
		DrawModel(beam, {}, { 800.0f * beamT, 2000.0f, 800.0f * beamT }, Quaternion::Identity(), { 1.0f, 0.0f, 0.0, 0.7f });
		beamT.IncreaseValue(GetDeltaTime());
	}
	else if (points.size() > 1) {
		GetRenderer()->SetDepthState(DEPTH_STATE_NO_WRITE);
		GetRenderer()->SetRasterizerState(RASTERIZER_STATE_CULL_NONE);

		DrawPolygon(texture, vertexes.data(), points.size() * 2, TOPOLOGY_TRIANGLESTRIP, {}, { 1.0f, 1.0f, 1.0f }, Quaternion::Identity(), { 1.0f, 1.0f, 1.0f, 0.7f });

		GetRenderer()->SetRasterizerState(RASTERIZER_STATE_CULL_NONE);
		GetRenderer()->SetDepthState(DEPTH_STATE_ENABLE);
	}
	GetRenderer()->SetBlendState(BLEND_STATE_ALPHA);
}
