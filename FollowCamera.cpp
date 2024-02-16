﻿#include "FollowCamera.h"
#include "MathUtilityForText.h"

void FollowCamera::Initialize() {
	viewProjection_.Initialize();
}

void FollowCamera::Update() {
	if (target_) {
		// 追従対象からカメラまでのオフセット
		Vector3 offset = {0.0f, 2.0f, -10.0f};

		Matrix4x4 matRotX = MakeRotateXMatrix(viewProjection_.rotation_.x);
		Matrix4x4 matRotY = MakeRotateYMatrix(viewProjection_.rotation_.y);
		Matrix4x4 matRotZ = MakeRotateZMatrix(viewProjection_.rotation_.z);
		// 回転行列の合成
		Matrix4x4 matRot = matRotZ * matRotX * matRotY;

		// オフセットをカメラの回転に合わせて回転させる
		offset = TransformNormal(offset, matRot);
		// 座標をコピーしてオフセット分からず
		viewProjection_.translation_ = target_->translation_ + offset;
	}
	//ビュー行列の更新
	viewProjection_.farZ = 2000.0f;
	viewProjection_.UpdateMatrix();
}