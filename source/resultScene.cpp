#include "scene.h"
#include "gameObjectQuad.h"
#include "progress.h"
#include "renderer.h"
#include "gameObjectText.h"
using namespace MG;

namespace ResultScene {
	static constexpr const char* TEXTURE_WHITE = "asset\\texture\\white.png";
	static constexpr const char* MODEL_BOM = "asset\\model\\bom.mgm";

	// =======================================================
	// クラス定義
	// =======================================================
	class ResultScene : public Scene {
	private:
		Quaternion rotate0;
		Quaternion rotate1;
		Quaternion rotate2;
		Model* bomModel;
		Progress fade{ 2000.0f, false };
		Progress rotateT{ 700000.0f, true };
		Progress transitT{ 1500.0f, false };
		GameObjectText* thnaksLabel;
		GameObjectText* scoreLabel;
	public:
		void Init() override;
		//void Uninit() override;
		void Update() override;
		void Draw() override;
		//LAYER_TYPE GetLayerType(int layer) override;
	};


	// =======================================================
	// シーン登録
	// =======================================================
	static SceneName sceneName = REGISTER_SCENE("result", ResultScene);

	// =======================================================
	// 初期化
	// =======================================================
	void ResultScene::Init()
	{
		Scene::Init();

		// スクリーンの中心座標を取得
		F2 screenCenter = GetScreenCenter();

		// スクリーンのサイズを取得
		F2 screenSize = GetScreenSize();

		AddGameObject(
			GameObjectQuad(LoadTexture(TEXTURE_WHITE), { screenSize.x, screenSize.y }), -2
		);

		bomModel = LoadModel(MODEL_BOM);

		rotate0 = Quaternion(F3{ ((float)rand()) / RAND_MAX, ((float)rand()) / RAND_MAX, ((float)rand()) / RAND_MAX });
		rotate1 = Quaternion(F3{ ((float)rand()) / RAND_MAX, ((float)rand()) / RAND_MAX, ((float)rand()) / RAND_MAX });
		rotate2 = Quaternion(F3{ ((float)rand()) / RAND_MAX, ((float)rand()) / RAND_MAX, ((float)rand()) / RAND_MAX });


		AddGameObject(
			GameObjectText(
				std::wstring(L"Thank your playing!"),
				FONT{
					screenSize.y * 0.1f,
					"HG創英角ﾎﾟｯﾌﾟ体"
				},
				TEXT_RESIZE_KEEP_RATIO_BY_WIDTH,
				TEXT_ORIGIN_HORIZONTAL_CENTER,
				TEXT_ORIGIN_VERTICAL_CENTER,
				{ 2.0f, 2.0f, 1.0f }, {  }
			)
		);
		scoreLabel;
	}


	// =======================================================
	// 終了処理
	// =======================================================
	/*void ResultScene::Uninit()
	{
		Scene::Uninit();
	}*/


	// =======================================================
	// 更新
	// =======================================================
	void ResultScene::Update()
	{
		Scene::Update();
		rotate0 = Lerp(rotate0, Normalize(rotate1 * rotate0), rotateT);
		rotate1 = Lerp(rotate1, Normalize(rotate2 * rotate1), rotateT);
		rotate2 = Lerp(rotate2, Normalize(rotate0 * rotate2), rotateT);
		fade.IncreaseValue(GetDeltaTime());
		rotateT.IncreaseValue(GetDeltaTime());
		if (fade == 1.0f) {
			transitT.IncreaseValue(GetDeltaTime());
		}
	}


	// =======================================================
	// 描画
	// =======================================================
	void ResultScene::Draw()
	{
		Scene::Draw();
		Renderer* renderer = GetRenderer();

		renderer->SetDepthState(DEPTH_STATE_NO_WRITE_DRAW_ALL);
		//renderer->SetBlendState(BLEND_STATE_ADD);
		DrawModel(bomModel, { 3.0f * transitT , -1.7f * transitT }, F3{ 1.0f, 1.0f, 1.0f }, rotate0, { 1.0f, 1.0f, 1.0f, fade });
		DrawModel(bomModel, { 3.0f * transitT , -1.7f * transitT }, F3{ 1.0f, 1.0f, 1.0f } *0.98f, rotate1, { 1.0f, 1.0f, 1.0f, fade });
		DrawModel(bomModel, { 3.0f * transitT , -1.7f * transitT }, F3{ 1.0f, 1.0f, 1.0f } *0.96f, rotate2, { 1.0f, 1.0f, 1.0f, fade });

		renderer->SetDepthState(DEPTH_STATE_ENABLE);

	}

	// =======================================================
	// レイヤータイプの定義
	// =======================================================
	/*LAYER_TYPE ResultScene::GetLayerType(int layer)
	{
		return Scene::GetLayerType(layer);
	}*/
}
