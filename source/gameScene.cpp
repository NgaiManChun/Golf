#include "scene.h"
#include "ball.h"

#include "trail.h"
#include "rail.h"
#include "keyboard.h"
#include "renderer.h"
#include "button.h"
#include "grass.h"
#include "water.h"
#include "crystal.h"
#include "curtain.h"
#include "ui.h"
#include "gameObjectText.h"
#include "gameObjectAudio.h"

#include <iostream>
using namespace MG;

namespace GameScene {
	static constexpr const char* MAP_ARRAGNEMENT = "asset\\model\\map.mgi";
	static constexpr const char* BACKGROUND_MODEL = "asset\\model\\background.mgm";
	static constexpr const char* TEXTURE_WHITE = "asset\\texture\\white.png";
	static constexpr const char* TEXTURE_MOYA = "asset\\texture\\moya.png";
	static constexpr const char* TEXTURE_TITLE = "asset\\texture\\title.png";
	static constexpr const char* BALL_MODEL = "asset\\model\\ball.mgm";
	static constexpr const char* TITLE_SOUND = "asset\\sound\\title.wav";
	static constexpr const int LAYER_BALL = 1;
	static constexpr const int LAYER_GROUND =	2;
	static constexpr const int LAYER_WATER =	3;
	static constexpr const int LAYER_ITEM =		4;
	static constexpr const int LAYER_EFFECT =	5;
	static constexpr const int LAYER_SEE_THROUGH = 5;
	static constexpr const int LAYER_CURTAIN = LAYER_2D + 1;;
	static constexpr const float SLEEP_DISTANCE = 500.0f * 500.0f; // 休眠状態距離（２乗）
	static constexpr const F3 IDLE_OFFSET = { 0.0f, 3.0f, -10.0f };
	static constexpr const F3 HIGHVIEW_OFFSET = { -30.0f, 40.0f, -70.0f };
	static constexpr const F3 FOCUS_OFFSET = { -1.0f, 1.0f, -1.0f };
	static constexpr const F3 SHOOT_DIRECTION = { 0.0f,  0.447213590f, 0.894427180f };
	static constexpr const float SHOOT_MAX_FORCE = 100.0f;
	static constexpr const float CAMERA_ROTATE_AMOUNT = 1.0f;
	static constexpr const float CAMERA_MOVE_AMOUNT = 50.0f;
	
	
	enum STATE {
		STATE_TITLE,
		STATE_IDLE,
		STATE_FOCUS,
		STATE_FLYING,
		STATE_FREE_VIEW,
		STATE_OUT_COURSE,
		STATE_FINISH
	};

	// =======================================================
	// クラス定義
	// =======================================================
	class GameScene : public Scene {
	private:
		STATE state = STATE_FREE_VIEW;
		Ball* ball;
		Rail* rail;
		Model* background;
		Camera3D lowCamera;
		Camera3D goalCamera;
		Camera3D miniCamera;

		Progress focusProgress{ 500.0f, false };
		F3 shootDirection = {};

		Progress cameraTransit{ 500.0f, false };
		F3 cameraTransitStartPosition = {};
		float cameraTransitStartAngle = 60.0f;
		Quaternion cameraTransitStartRotate = Quaternion::Identity();

		Progress shootPower{ 800.0f, false };
		F3 lastPosition = {};
		Texture* whiteTexture;
		Texture* moyaTexture;
		Texture* titleTexture;
		Model* ballModel;
		//GameObjectText* strokesLabel;
		UI* ui;
		int currentCrystalNum = 0;
		int strokesNum = 0;

		GameObjectText* pressStartLabel;
		GameObjectQuad* titleQuad;
		bool isTitleFade = false;
		Progress titleFade{ 1000.0f, false };

		Curtain* curtain;
		RenderTarget* miniCameraRenderTarget;
		Progress miniCameraT{ 500.0f, false };

		GameObjectAudio* titleSound;

		Progress beamFade{ 2000.0f , false };
	public:
		void Init() override;
		//void Uninit() override;
		void Update() override;
		void Draw() override;
		//LAYER_TYPE GetLayerType(int layer) override;
		void SortGameObjects() override;
	};


	// =======================================================
	// シーン登録
	// =======================================================
	static SceneName sceneName = REGISTER_SCENE("game", GameScene);


	// =======================================================
	// 初期化
	// =======================================================
	void GameScene::Init()
	{
		Scene::Init();

		ambientLight = { 1.0f, 1.0f, 1.0f, 0.8f };
		driectLight = { 1.0f, 1.0f, 1.0f, 0.2f };
		driectLightPosition = { 400.0f, 2000.0f, -300.0f };
		currentCamera->SetFar(3000.0f);

		whiteTexture = LoadTexture(TEXTURE_WHITE);
		moyaTexture = LoadTexture(TEXTURE_MOYA);
		ballModel = LoadModel(BALL_MODEL);
		titleTexture = LoadTexture(TEXTURE_TITLE);
		 

		// UI
		ui = AddGameObject(UI(), LAYER_2D);

		// ボール
		ball = AddGameObject(Ball({ 0.0f, 3.0f, 5.0f }), LAYER_BALL);
		ball->AddCrystal = [this](){
			ui->SetCrystalNum(++currentCrystalNum);
		};

		// ボール軌跡
		ball->trail =  AddGameObject(Trail(ball), LAYER_EFFECT);

		// 照準軌跡
		rail = AddGameObject(Rail(ball), LAYER_EFFECT);
		rail->enable = false;

		F2 screenSize = GetScreenSize();

		// タイトル
		titleQuad = AddGameObject(
			GameObjectQuad(
				LoadTexture(TEXTURE_TITLE),
				{ screenSize.x * 0.7f, screenSize.x * 0.7f / titleTexture->GetWidth() * titleTexture->GetHeight() }, 
				{}, {}, { 1.0f, 1.0f, 1.0f, 0.0f }
			), LAYER_2D
		);
		pressStartLabel = AddGameObject(GameObjectText(
			L"press space",
			FONT{
				screenSize.y * 0.1f,
				"HG創英角ﾎﾟｯﾌﾟ体"
			},
			TEXT_RESIZE_KEEP_RATIO_BY_HEIGHT,
			TEXT_ORIGIN_HORIZONTAL_CENTER,
			TEXT_ORIGIN_VERTICAL_CENTER,
			{ 100.0f, screenSize.y * 0.1f, 1.0f }, { 0.0f, screenSize.y * -0.4f }, {}, { 1.0f, 1.0f, 1.0f, 0.0f }
		), LAYER_2D);
		titleSound = AddGameObject(GameObjectAudio(LoadAudio(TITLE_SOUND), false));


		// 復帰カーテン
		curtain = AddGameObject(Curtain(), LAYER_CURTAIN);
		curtain->enable = false;
		curtain->Reset();

		// ミニカメラ
		miniCameraRenderTarget = GetRenderer()->CreateRenderTarget();
		
		// マップ読み込み
		MGObject mgi = LoadMGO(MAP_ARRAGNEMENT);
		ARRANGEMENT* arrangement = GetArrangementByMGObject(mgi);
		for (int i = 0; i < arrangement->instanceNum; i++) {
			MODEL_INSTANCE& modelInstance = arrangement->instances[i];
			const char* instance = arrangement->instances[i].instance;
			if (!strcmp(instance, "grass")) {
				AddGameObject(Grass(modelInstance.position, modelInstance.scale, modelInstance.rotate, sceneName), LAYER_GROUND);
			}
			else if (!strcmp(instance, "grassZ")) {
				AddGameObject(GrassZ(modelInstance.position, modelInstance.scale, modelInstance.rotate, sceneName), LAYER_GROUND);
			}
			else if (!strcmp(instance, "crystal")) {
				AddGameObject(Crystal(modelInstance.position, modelInstance.scale, modelInstance.rotate, sceneName), LAYER_ITEM);
			}
			else if (!strcmp(instance, "water")) {
				AddGameObject(Water(modelInstance.position, modelInstance.scale, modelInstance.rotate, sceneName), LAYER_WATER);
			}
			else if (!strcmp(instance, "button")) {
				AddGameObject(Button(modelInstance.position, modelInstance.scale, modelInstance.rotate, sceneName), LAYER_GROUND);
			}
			else if (!strcmp(instance, "ball")) {
				ball->position = modelInstance.position;
				currentCamera->position = ball->position + F3{ 0.0f, 0.0f, -10.0f };
			}
		}
		mgi.Release();

		background = LoadModel(BACKGROUND_MODEL);

		focusProgress = 0.0f;
		shootDirection = {};
		cameraTransit = 0.0f;
		currentCrystalNum = 0;
		strokesNum = 0;
		isTitleFade = false;
		titleFade = { 1000.0f, false };
		state = STATE_FLYING;
		state = STATE_TITLE;
		//state = STATE_FREE_VIEW;
	}


	// =======================================================
	// 終了処理
	// =======================================================
	/*void GameScene::Uninit()
	{
		Scene::Uninit();
	}*/


	// =======================================================
	// 更新
	// =======================================================
	void GameScene::Update()
	{
		
		// ボールのカメラの間に当たり判定を作る
		// 当たったオブジェクトを透視にする
		Box seeThroughRay = Box(
			M4x4::ScalingMatrix({ 1.0f, 0.5f, Distance(currentCamera->GetPosition(), ball->position) }) *
			//M4x4::RotatingMatrix(currentCamera->GetRotate()) *
			M4x4::RotatingMatrix(Quaternion(Normalize(ball->position - currentCamera->GetPosition()))) *
			M4x4::TranslatingMatrix((currentCamera->GetPosition() + ball->position) / 2.0f),
			"seeThrough"
		);

		
		for (GameObject* gameObject : gameObjects) {
			if (state == STATE_FINISH) break;
			// 2Dレイヤー除外
			if (GetLayerType(gameObject->layer) == LAYER_TYPE_2D) continue;

			// ボール自身を除外
			if (ball == gameObject) continue;

			// 休眠状態オブジェクト除外
			if (!gameObject->enable) continue;

			// 当たり判定
			ball->Collision(gameObject);

			
			if (gameObject->layer == LAYER_GROUND || gameObject->layer == LAYER_WATER) {
				// 透視判定
				gameObject->Collision(&seeThroughRay);
			}
			
		}
		bool UseMiniCamera = false;
		static Progress HighViewT = { 500.0f, false };
		if (state == STATE_TITLE) {
			static Progress cameraRotateT{30000.0f, true};
			ui->enable = false;
			if (!isTitleFade) {
				titleQuad->color.w = titleFade;
				pressStartLabel->color.w = titleFade;
			}
			else {
				titleQuad->color.w = 1.0f - titleFade;
				pressStartLabel->color.w = 1.0f - titleFade;
			}

			if (!isTitleFade && Keyboard_IsKeyDownTrigger(KK_SPACE)) {
				isTitleFade = true;
				titleFade = 0.0f;
				titleSound->Play();
			}
			if (isTitleFade && titleFade == 1.0f) {
				state = STATE_IDLE;
				cameraTransitStartPosition = currentCamera->GetPosition();
				cameraTransitStartRotate = currentCamera->GetRotate();
				cameraTransit = 0.0f;
				titleQuad->enable = false;
				pressStartLabel->enable = false;
			}
			titleFade.IncreaseValue(GetDeltaTime());


			currentCamera->SetPosition(Rotate(F3{ 0.0f, 150.0f, -800.0f}, Quaternion::AxisYRadian(cameraRotateT * 2.0f * PI)));
			currentCamera->SetFront(currentCamera->position * -1.0f);
			cameraRotateT.IncreaseValue(GetDeltaTime());
		}
		else if (state == STATE_IDLE) {
			ui->enable = true;
			float rotateAmount = CAMERA_ROTATE_AMOUNT * GetDeltaTime() * 0.001f;

			
			if (Keyboard_IsKeyDown(KK_H)) {
				HighViewT.IncreaseValue(GetDeltaTime());
			}
			else {
				HighViewT.IncreaseValue(-GetDeltaTime());
			}

			if (Keyboard_IsKeyDown(KK_W)) {
				currentCamera->SetRotate(currentCamera->GetRotate() * Quaternion::AxisXRadian(-rotateAmount));
			}
			if (Keyboard_IsKeyDown(KK_S)) {
				currentCamera->SetRotate(currentCamera->GetRotate() * Quaternion::AxisXRadian(rotateAmount));
			}
			if (Keyboard_IsKeyDown(KK_A)) {
				currentCamera->SetRotate(Quaternion::AxisYRadian(-rotateAmount) * currentCamera->GetRotate());
			}
			if (Keyboard_IsKeyDown(KK_D)) {
				currentCamera->SetRotate(Quaternion::AxisYRadian(rotateAmount) * currentCamera->GetRotate());
			}
			F3 cameraPoistion = ball->position + Rotate(IDLE_OFFSET + HIGHVIEW_OFFSET * HighViewT, currentCamera->GetRotate());

			if (cameraTransit < 1.0f) {
				currentCamera->SetPosition(Lerp(cameraTransitStartPosition, cameraPoistion, cameraTransit));
				currentCamera->SetAngle(Lerp(cameraTransitStartAngle, 60.0f, cameraTransit));
				if (isTitleFade) {
					currentCamera->SetRotate(Lerp(cameraTransitStartRotate, Quaternion::Identity(), cameraTransit));
				}
			}
			else {
				currentCamera->SetPosition(cameraPoistion);
				currentCamera->SetAngle(60.0f);
				isTitleFade = false;
			}

			if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
				state = STATE_FOCUS;
				cameraTransitStartPosition = currentCamera->GetPosition();
				cameraTransit = 0.0f;
			}
			if (Keyboard_IsKeyDown(KK_L) && Keyboard_IsKeyDown(KK_R) && currentCrystalNum > 1) {
				state = STATE_FINISH;
				cameraTransitStartRotate = currentCamera->GetRotate();
				cameraTransit = 0.0f;
				ui->SetSuper(false);
			}
			if (Keyboard_IsKeyDown(KK_P)) {
				ball->AddCrystal();
			}

		}
		else if(state == STATE_FOCUS) {
			float rotateAmount = CAMERA_ROTATE_AMOUNT * GetDeltaTime() * 0.001f;

			if (Keyboard_IsKeyDown(KK_H)) {
				HighViewT.IncreaseValue(GetDeltaTime());
			}
			else {
				HighViewT.IncreaseValue(-GetDeltaTime());
			}

			if (Keyboard_IsKeyDown(KK_W)) {
				currentCamera->SetRotate(currentCamera->GetRotate() * Quaternion::AxisXRadian(-rotateAmount));
			}
			if (Keyboard_IsKeyDown(KK_S)) {
				currentCamera->SetRotate(currentCamera->GetRotate() * Quaternion::AxisXRadian(rotateAmount));
			}
			if (Keyboard_IsKeyDown(KK_A)) {
				currentCamera->SetRotate(Quaternion::AxisYRadian(-rotateAmount) * currentCamera->GetRotate());
			}
			if (Keyboard_IsKeyDown(KK_D)) {
				currentCamera->SetRotate(Quaternion::AxisYRadian(rotateAmount) * currentCamera->GetRotate());
			}

			F3 cameraPoistion = ball->position + Rotate(FOCUS_OFFSET + HIGHVIEW_OFFSET * HighViewT, currentCamera->GetRotate());

			shootDirection = Rotate(SHOOT_DIRECTION, currentCamera->GetRotate());
			rail->enable = true;
			rail->SetDirection(shootDirection);
			rail->UpdatePath();

			if (cameraTransit < 1.0f) {
				currentCamera->SetPosition(Lerp(cameraTransitStartPosition, cameraPoistion, cameraTransit));
			}
			else {
				currentCamera->SetPosition(cameraPoistion);
			}

			float power = (1.0f - shootPower * 0.95f);
			if (Keyboard_IsKeyDown(KK_SPACE)) {
				static int sign = 1;
				if (shootPower == 1.0f || shootPower == 0.0f) {
					sign = -sign;
				}
				shootPower.IncreaseValue(GetDeltaTime() * sign);
				rail->SetPower(SHOOT_MAX_FORCE * power);
			}
			else {
				ball->SavePosition();
				ball->AddForce(shootDirection * SHOOT_MAX_FORCE * power);
				
				lastPosition = ball->position;
				shootPower = 0.0f;
				strokesNum++;
				rail->SetPower(0.0f);
				state = STATE_FLYING;
			}
			
			
		}
		else if (state == STATE_FLYING) {
			ui->enable = false;
			if (ball->IsStop()) {
				state = STATE_IDLE;
				cameraTransitStartPosition = currentCamera->GetPosition();
				cameraTransitStartAngle = currentCamera->GetAngle();
				cameraTransit = 0.0f;
			}
			if (DistanceSquare(ball->position, currentCamera->position) > 50.0f * 50.0f) {
				currentCamera->SetFront(Lerp(currentCamera->GetFront(), ball->position - currentCamera->position, GetDeltaTime() * 0.001f), {0.0f, 1.0f, 0.0f });
				currentCamera->SetAngle(Lerp(currentCamera->GetAngle(), 10.0f, GetDeltaTime() * 0.001f));
				UseMiniCamera = true;
			}
			
		}
		else if (state == STATE_FREE_VIEW) {
			float rotateAmount = CAMERA_ROTATE_AMOUNT * GetDeltaTime() * 0.001f;
			if (Keyboard_IsKeyDown(KK_U)) {
				currentCamera->SetRotate(currentCamera->GetRotate() * Quaternion::AxisXRadian(-rotateAmount));
			}
			if (Keyboard_IsKeyDown(KK_J)) {
				currentCamera->SetRotate(currentCamera->GetRotate() * Quaternion::AxisXRadian(rotateAmount));
			}
			if (Keyboard_IsKeyDown(KK_H)) {
				currentCamera->SetRotate(Quaternion::AxisYRadian(-rotateAmount) * currentCamera->GetRotate());
			}
			if (Keyboard_IsKeyDown(KK_K)) {
				currentCamera->SetRotate(Quaternion::AxisYRadian(rotateAmount) * currentCamera->GetRotate());
			}

			F3 cameraMove = {};
			float moveAmount = CAMERA_MOVE_AMOUNT * GetDeltaTime() * 0.001f;
			if (Keyboard_IsKeyDown(KK_W)) {
				cameraMove.z += moveAmount;
			}
			if (Keyboard_IsKeyDown(KK_S)) {
				cameraMove.z -= moveAmount;
			}
			if (Keyboard_IsKeyDown(KK_A)) {
				cameraMove.x -= moveAmount;
			}
			if (Keyboard_IsKeyDown(KK_D)) {
				cameraMove.x += moveAmount;
			}
			if (Keyboard_IsKeyDown(KK_E)) {
				cameraMove.y += moveAmount;
			}
			if (Keyboard_IsKeyDown(KK_Q)) {
				cameraMove.y -= moveAmount;
			}
			if (Keyboard_IsKeyDown(KK_C)) {
				currentCamera->SetPosition(ball->position);
			}

			currentCamera->SetPosition(currentCamera->position + Rotate(cameraMove, currentCamera->GetRotate()));
		}
		else if (state == STATE_OUT_COURSE) {
			curtain->enable = true;
			if (curtain->GetTime() == 1.0f) {
				curtain->SetReverse(true);
				ball->RestorePosition();
				ball->SetOutCourse(0.0f);
				state = STATE_IDLE;

				currentCamera->SetPosition(ball->position + Rotate(IDLE_OFFSET, currentCamera->GetRotate()));
			}
			if (curtain->GetReverse() && curtain->GetTime() == 0.0f) {
				curtain->enable = false;
				curtain->Reset();
			}
		}
		else if (state == STATE_FINISH) {
			
			if (cameraTransit < 1.0f) {
				currentCamera->SetRotate(Lerp(cameraTransitStartRotate, Quaternion(F3{ 0.0f, 250.0f, 0.0f } - currentCamera->position), cameraTransit));
			}
			else {
				currentCamera->SetFront(F3{ 0.0f, 250.0f, 0.0f } - currentCamera->position);
				if (!ball->shootFinish) {
					ball->ShootFinish();
				}
				
			}
			if (ball->trail->beamT == 1.0f) {

				beamFade.IncreaseValue(GetDeltaTime());
			}
			if (beamFade == 1.0f) {
				SceneTransit("result", "sample_transition");
			}
		}

		if (ball->IsOutCourse()) {
			state = STATE_OUT_COURSE;
		}

		ui->SetStrokeNum(strokesNum);
		
		rail->color.w = focusProgress;
		if (state == STATE_FOCUS) {
			focusProgress.IncreaseValue(GetDeltaTime());
		}
		else {
			focusProgress.IncreaseValue(-GetDeltaTime());
		}
		if (UseMiniCamera) {
			miniCameraT.IncreaseValue(GetDeltaTime());
		}
		else {
			miniCameraT.IncreaseValue(-GetDeltaTime());
		}
		cameraTransit.IncreaseValue(GetDeltaTime());
		
		Scene::Update();

	}

	void GameScene::SortGameObjects() {
		//Scene::SortGameObjects();
		
		gameObjects.sort([this](GameObject* a, GameObject* b) -> bool {
			if (a->layer == LAYER_GROUND && b->layer == LAYER_GROUND) {
				bool transpentGroundA = (a->layer == LAYER_GROUND && a->color.w < 1.0f);
				bool transpentGroundB = (b->layer == LAYER_GROUND && b->color.w < 1.0f);
				if (transpentGroundA && transpentGroundB) {
					return DistanceSquare(currentCamera->GetPosition(), a->position) < DistanceSquare(currentCamera->GetPosition(), b->position);
				}
				else if (transpentGroundA) {
					return false;
				}
				else if (transpentGroundB) {
					return true;
				}
			}
			
			return a->layer < b->layer;
		});
	}


	// =======================================================
	// 描画
	// =======================================================
	void GameScene::Draw()
	{
		// 背景
		Renderer* renderer = GetRenderer();
		if (state == STATE_FINISH) {
			ambientLight.w = 0.6f * (1.0f - cameraTransit) + 0.2f * cameraTransit;
			driectLight.w = 0.4f * (1.0f - cameraTransit) + 0.1f * cameraTransit;
		}
		else {
			ambientLight = { 1.0f, 1.0f, 1.0f, 0.6f };
			driectLight = { 1.0f, 1.0f, 1.0f, 0.4f };
		}
		
		renderer->SetAmbient(ambientLight);
		renderer->SetLightColor(driectLight);
		renderer->ApplyCamera(currentCamera);
		
		DrawModel(background, currentCamera->GetPosition() + F3{0.0f, 0.6f});
		renderer->SetDepthState(DEPTH_STATE_ENABLE);

		Scene::Draw();

		if (miniCameraT) {
			miniCamera.SetPosition(ball->position + F3{ 1.0f, 1.0f, 1.0f });
			miniCamera.SetFront(ball->position - miniCamera.position);

			// スクリーンの中心座標を取得
			F2 screenCenter = GetScreenCenter();

			// スクリーンのサイズを取得
			F2 screenSize = GetScreenSize();

			// サブ描画ターゲット設定
			renderer->SetRenderTarget(miniCameraRenderTarget);
			renderer->ClearRenderTargetView(miniCameraRenderTarget);

			renderer->ApplyCamera(&miniCamera);
			renderer->SetAmbient(ambientLight);
			renderer->SetLight(driectLightPosition, driectLight);
			for (auto gameObject : gameObjects) {
				if (!gameObject->enable) continue;
				if (GetLayerType(gameObject->layer) == LAYER_TYPE_2D) continue;
				gameObject->Draw();
			}
			
			// メイン描画ターゲットに切り替える
			renderer->SetRenderTarget(nullptr);

			renderer->ApplyCamera(currentCamera2D);
			DrawQuad(
				miniCameraRenderTarget->texture,
				{ screenSize.x * 0.375f - 40.0f, screenSize.y * -0.375f + 40.0f },
				{ screenSize.x * 0.25f, screenSize.y * 0.25f, 1.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 1.0f, miniCameraT }
			);
		}
		

		if (beamFade > 0.0f) {
			renderer->ApplyCamera(currentCamera);
			renderer->SetBlendState(BLEND_STATE_ADD);
			DrawModel(ballModel, currentCamera->GetPosition(), { 1.0f, 1.0f, 1.0f }, currentCamera->GetRotate(), { 1.0f, 1.0f, 1.0f, beamFade });
			renderer->SetBlendState(BLEND_STATE_ALPHA);
		}

	}

	// =======================================================
	// レイヤータイプの定義
	// =======================================================
	/*LAYER_TYPE GameScene::GetLayerType(int layer)
	{
		return Scene::GetLayerType(layer);
	}*/
}
