#pragma once

#include <DirectXMath.h>
#include "RenderState.h"

struct RenderContext
{
	ID3D11DeviceContext* deviceContext;               // デバイスコンテキスト。描画コマンドの発行に使用。
											         
	const RenderState* renderState = nullptr;         // 現在のレンダーステートへのポインタ（ブレンド・ラスタライザ・デプスステートなど）
											         
	DirectX::XMFLOAT4X4 view;                         // カメラのビュー行列（ワールド空間→カメラ空間）
	DirectX::XMFLOAT4X4 projection;                   // カメラの射影行列（カメラ空間→クリップ空間）

	DirectX::XMFLOAT3 lightDirection = { 0, -1, 0 };  // 平行光源の方向ベクトル（上から照らす）
	DirectX::XMFLOAT4 cameraPosition;                 // カメラのワールド座標（ピクセルシェーダ等で使用）

	// シャドウマップ関連
	DirectX::XMFLOAT4X4 lightViewProjection;          // ライト視点からのビュー射影行列（シャドウマッピング用）
	DirectX::XMFLOAT3 shadowColor;                    // 影の色（乗算的に使用）
	float shadowBias;                                 // 深度バイアス（シャドウアクネを防ぐための微調整値）

	// フォグおよび環境光
	DirectX::XMFLOAT4 ambientColor;                   // アンビエントライト（環境光）の色
	DirectX::XMFLOAT4 fogColor;                       // フォグの色
	DirectX::XMFLOAT4 fogRange;                       // フォグの開始・終了距離（x: start, y: end, z/w: 未使用またはカスタム用途）

	// 点光源設定
	float lightPower;                                 // 全体的な光源の強度（エネルギー係数）

	struct PointLightConstants
	{
		DirectX::XMFLOAT4 position;                   // 点光源の位置（wは未使用）
		DirectX::XMFLOAT4 color;                      // 光の色（強度も含めて調整可能）
		float range;                                  // 有効距離（減衰の終了距離）
		DirectX::XMFLOAT3 dummy = { 0,0,0 };          // アラインメント調整用（16バイト境界）
	};

	PointLightConstants pointLights[47];               // 点光源を最大8つまで定義可能

	// 線光源設定
	struct LineLightConstants
	{
		DirectX::XMFLOAT4 start;                      // 線光源の始点
		DirectX::XMFLOAT4 end;                        // 線光源の終点
		DirectX::XMFLOAT4 color;                      // 光の色
		float range;                                  // 有効距離（線からの距離で減衰）
		DirectX::XMFLOAT3 dummy = { 0,0,0 };          // アラインメント調整用
	};

	LineLightConstants lineLights[42];                 // 線光源を最大8つまで定義可能
};
