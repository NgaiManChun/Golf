#include "curtain.h"

static constexpr const char* TEXTURE_WHITE = "asset\\texture\\white.png";
static constexpr const char* TEXTURE_STAR = "asset\\texture\\star.png";

Curtain::Curtain() {
	renderTarget = GetRenderer()->CreateRenderTarget();
	tex = LoadTexture(TEXTURE_WHITE);
	texStar = LoadTexture(TEXTURE_STAR);
	color = { 0.0f, 0.0f, 0.0f, 1.0f };
}

Curtain::~Curtain() {
	if (renderTarget) {
		GetRenderer()->ReleaseRenderTarget(renderTarget);
		renderTarget = nullptr;
	}
}

void Curtain::Update() {
	F2 screenSize = GetScreenSize();
	size = { screenSize.x, screenSize.y, 0.0f };
	float maxLength = std::max(screenSize.x, screenSize.y);
	starSize = F3{ maxLength, maxLength, 0.0f } *2.5f * (1.0f - t);
	if (reverse) {
		t.IncreaseValue(-GetDeltaTime());
	}
	else {
		t.IncreaseValue(GetDeltaTime());
	}
}

void Curtain::Draw() {
	GetRenderer()->SetRenderTarget(renderTarget);
	GetRenderer()->ClearRenderTargetView(renderTarget);

	DrawQuad(tex, {}, size, rotate, color);
	GetRenderer()->SetBlendState(BLEND_STATE_DEST_OUT);
	DrawQuad(texStar, {}, starSize);
	GetRenderer()->SetBlendState(BLEND_STATE_ALPHA);

	GetRenderer()->SetRenderTarget(nullptr);
	DrawQuad(renderTarget->texture, {}, size, rotate);

}

void Curtain::Reset() {
	reverse = false;
	t = 0.0f;
}

void Curtain::SetReverse(bool reverse) {
	this->reverse = reverse;
}

bool Curtain::GetReverse() const {
	return reverse;
}

float Curtain::GetTime() const {
	return t;
}