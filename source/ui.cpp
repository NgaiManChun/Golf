#include "ui.h"

constexpr const char* CRYSTAL_MODEL = "asset\\model\\crystal.mgm";

UI::UI(const std::string& scope)
{
	F2 screenSize = GetScreenSize();
	strokesLabel = new GameObjectText(
		std::wstring(L"‘Å”F") + std::to_wstring(0),
		FONT{
			screenSize.y * 0.1f,
			"HG‘n‰pŠpÎß¯Ìß‘Ì"
		},
		TEXT_RESIZE_KEEP_RATIO_BY_HEIGHT,
		TEXT_ORIGIN_HORIZONTAL_LEFT,
		TEXT_ORIGIN_VERTICAL_TOP,
		{ 100.0f, screenSize.y * 0.1f, 1.0f }, { screenSize.x * -0.5f + 40.0f, screenSize.y * 0.5f - 40.0f }
	);

	superLabel = new GameObjectText(
		std::wstring(L"R + L‚Å•KŽE‹ZI"),
		FONT{
			screenSize.y * 0.1f,
			"HG‘n‰pŠpÎß¯Ìß‘Ì"
		},
		TEXT_RESIZE_KEEP_RATIO_BY_HEIGHT,
		TEXT_ORIGIN_HORIZONTAL_CENTER,
		TEXT_ORIGIN_VERTICAL_BOTTOM,
		{ 100.0f, screenSize.y * 0.1f, 1.0f }, { 0, 0 }
	);

	controlLabel = new GameObjectText(
		std::wstring(L"WASD•ûŒü‘€ì\nƒXƒy[ƒX’·‰Ÿ‚µƒVƒ…[ƒg\nH‰“‚­Œ©‚é"),
		FONT{
			screenSize.y * 0.1f,
			"HG‘n‰pŠpÎß¯Ìß‘Ì"
		},
		TEXT_RESIZE_KEEP_RATIO_BY_HEIGHT,
		TEXT_ORIGIN_HORIZONTAL_LEFT,
		TEXT_ORIGIN_VERTICAL_BOTTOM,
		{ 100.0f, screenSize.y * 0.1f, 1.0f }, { screenSize.x * -0.5f + 40.0f,  100 }
	);

	crystalModel = LoadModel(CRYSTAL_MODEL);
}

UI::~UI()
{
	if (strokesLabel) {
		delete strokesLabel;
	}
	delete superLabel;
	delete controlLabel;
}

void UI::Update()
{
	if (strokesLabel) {
		strokesLabel->Update();
	}
	if (superLabel) {
		superLabel->Update();
	}
	if (controlLabel) {
		controlLabel->Update();
	}
	crystalRotateT.IncreaseValue(GetDeltaTime());
}

void UI::Draw()
{
	F2 screenSize = GetScreenSize();
	if (strokesLabel) {
		

		strokesLabel->position = { screenSize.x * -0.5f + 40.0f - 4.0f, screenSize.y * 0.5f - 40.0f - 4.0f };
		strokesLabel->color = { 0.0f, 0.0f, 0.0f, 0.5f };
		strokesLabel->Draw();

		strokesLabel->position = { screenSize.x * -0.5f + 40.0f, screenSize.y * 0.5f - 40.0f };
		strokesLabel->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		strokesLabel->Draw();
	}
	if (crystalNum > 1 && superLabel->enable) {
		superLabel->Draw();
	}
	controlLabel->Draw();

	F3 size = { 60.0f, 60.0f, 60.0f };
	for (int i = 0; i < crystalNum; i++) {
		F3 position = {  };
		DrawModel(
			crystalModel, 
			{ screenSize.x * -0.5f + i * 80.0f + 80.0f * 0.5f,  screenSize.y * -0.5f + 60.0f * 1.5f, 0.0f },
			size,
			Quaternion::AxisYRadian(2.0f * PI * crystalRotateT),
			{ 1.0f, 1.0f, 1.0f, 0.7f }
		);
	}
	
	
}

void UI::SetStrokeNum(const int num)
{
	strokesLabel->SetValue(std::wstring(L"‘Å”F") + std::to_wstring(num));
}

void UI::SetCrystalNum(const int num)
{
	crystalNum = num;
}

void UI::SetSuper(bool v)
{
	superLabel->enable = v;
}
