#include "Player.h"
#include "ImGuiManager.h"
#include <cassert>
#include "Input.h"
#include "MathUtilityForText.h"
#include "Vector3.h"

// 初期化
void Player::Initialize(Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm) {
	// NULLポインタチェック
	assert(modelBody);
	assert(modelHead);
	assert(modelL_arm);
	assert(modelR_arm);

	// メンバ変数に受け取った値を入力
	modelFighterBody_ = modelBody;
	modelFighterHead_ = modelHead;
	modelFighterL_arm_ = modelL_arm;
	modelFighterR_arm_ = modelR_arm;

	// ワールド変換の初期化
	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();

	// 親子構造
	worldTransformBody_.parent_ = &worldTransformBase_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	// 位置調整
	worldTransformHead_.translation_.y = 1.5038f;
	worldTransformL_arm_.translation_.x = -0.5275f;
	worldTransformL_arm_.translation_.y = 1.2619f;
	worldTransformR_arm_.translation_.x = 0.5275f;
	worldTransformR_arm_.translation_.y = 1.2619f;

	// インプット
	input_ = Input::GetInstance();

	InitializeFloatingGimmick();
}

// 更新
void Player::Update() {
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		const float speed = 0.3f;
		const float threshold = 0.5f;
		bool isMoving = false;

		// 移動量
		Vector3 move = {
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX};
		if (Length(move) > threshold) {
			isMoving = true;
		}
		move = Normalize(move) * speed;

		if (viewProjection_) {
			// カメラの回転行列
			Matrix4x4 matRotX = MakeRotateXMatrix(viewProjection_->rotation_.x);
			Matrix4x4 matRotY = MakeRotateYMatrix(viewProjection_->rotation_.y);
			Matrix4x4 matRotZ = MakeRotateZMatrix(viewProjection_->rotation_.z);
			// 回転行列の合成
			Matrix4x4 matRot = matRotZ * matRotX * matRotY;
			// 移動量をカメラの回転に合わせて回転させる
			move = TransformNormal(move, matRot);
		}
		// 移動
		worldTransformBase_.translation_ += move;

		// 移動ベクトルのY軸周り角度
		worldTransformBase_.rotation_.y = std::atan2(move.x, move.z);
	}

	ImGui::Begin("Player");
	ImGui::SliderFloat3(
	    "Head Translation", reinterpret_cast<float*>(&worldTransformHead_.translation_), -10.0f,
	    -10.0f);
	ImGui::SliderFloat3(
	    "Head Translation", reinterpret_cast<float*>(&worldTransformL_arm_.translation_), -10.0f,
	    10.0f);
	ImGui::SliderFloat3(
	    "Head Translation", reinterpret_cast<float*>(&worldTransformR_arm_.translation_), -10.0f,
	    10.0f);
	ImGui::SliderInt("floatintgCycie", reinterpret_cast<int*>(&floationgCycle_), 1, 200);
	ImGui::SliderFloat("floatingAmplitude", &floatingAmplitude_, 0.0f, 10.0f);
	ImGui::SliderFloat("idleArmAngleMax_", &idleArmAngleMax_, 0.0f, 180.0f);
	ImGui::End();

	const float speed = 0.3f;
	Vector3 move = {0.0f, 0.0f, 0.0f};
	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		move.x = 1.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		move.x = -1.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_UP)) {
		move.z = 1.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_DOWN)) {
		move.z = -1.0f;
	}

	move = Normalize(move) * speed;

	worldTransformBase_.translation_ += move;

	// 移動ベクトルのY軸周り角度
	worldTransformBase_.rotation_.y = std::atan2(move.x, move.z);
	
	// 浮遊ギミックの更新
	UpdateFloatingGimmick();

	// 変換行列を更新
	worldTransformBase_.matWorld_ = MakeAffineMatrix(
	    worldTransformBase_.scale_, worldTransformBase_.rotation_,
		worldTransformBase_.translation_);

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

// 描画
void Player::Draw(const ViewProjection& viewProjection) {
	// 3Dモデルを描画
	modelFighterBody_->Draw(worldTransformBody_, viewProjection);
	modelFighterHead_->Draw(worldTransformHead_, viewProjection);
	modelFighterL_arm_->Draw(worldTransformL_arm_, viewProjection);
	modelFighterR_arm_->Draw(worldTransformR_arm_, viewProjection);
}

void Player::InitializeFloatingGimmick() { 
	floatingParameter_ = 0.0f; }

void Player::UpdateFloatingGimmick() {
	floatingParameter_ += 0.1f; 
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * 0.2f;
	worldTransformL_arm_.rotation_.x = std::sin(floatingParameter_) * 0.5f;
	worldTransformR_arm_.rotation_.x = std::sin(floatingParameter_) * 0.5f;
}
