#pragma once

#include <memory>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Model.h"
#include "Shader.h"

enum class ShaderId
{
	Basic,
	Lambert,
	Custom,

	EnumCount
};

class ModelRenderer
{
public:
	ModelRenderer(ID3D11Device* device);
	~ModelRenderer() {}

	// 描画実行
	void Render(const RenderContext& rc, const DirectX::XMFLOAT4X4& worldTransform, const Model* model, ShaderId shaderId);

private:
	// シーン全体に関する定数バッファ
	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;     // ビュー射影行列（カメラからの視点）
		DirectX::XMFLOAT4		lightDirection;     // 平行光源の方向ベクトル
		DirectX::XMFLOAT4       cameraPosition;     // カメラのワールド座標
	};

	// フォグやアンビエントライティングに関する定数
	struct fogConstants
	{
		DirectX::XMFLOAT4       ambientColor;       // アンビエント光の色
		DirectX::XMFLOAT4       fogColor;           // フォグの色
		DirectX::XMFLOAT4       fogRange;           // フォグの開始距離・終了距離など（使用方法による）
	};

	// 点光源の情報を格納する構造体
	struct PointLightConstants
	{
		DirectX::XMFLOAT4 position;                // 点光源の位置
		DirectX::XMFLOAT4 color;                   // 光の色
		float range;                               // 有効距離（減衰範囲）
		DirectX::XMFLOAT3 dummy;                   // アラインメント調整用（16バイト境界維持）
	};

	struct TorusLightConstants
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 direction;
		DirectX::XMFLOAT4 color;
		float majorRadius;   // ドーナツの中心円の半径
		float minorRadius;   // ドーナツの太さ（断面円の半径）
		float range;
		float dummy;
	};
	// 線光源（Line Light）の情報を格納する構造体
	struct LineLightConstants
	{
		DirectX::XMFLOAT4 start;                   // 線光源の始点
		DirectX::XMFLOAT4 end;                     // 線光源の終点
		DirectX::XMFLOAT4 color;                   // 光の色
		float range;                               // 有効距離
		DirectX::XMFLOAT3 dummy = { 0,0,0 };       // アラインメント調整用
	};

	// 全体のライティング定数（点光源・線光源などまとめて管理）
	struct LightConstants
	{
		PointLightConstants pointLights[256];        // 点光源の配列（最大8個）
		TorusLightConstants torusLights[256];        // 円光源の配列（最大8個）
		LineLightConstants lineLights[256];          // 線光源の配列（最大8個）
		float power;                               // 照明全体の強度（グローバル係数）
		DirectX::XMFLOAT3 dummy;                   // アラインメント調整用
	};

	// シャドウマッピング用の定数
	struct ShadowConstants
	{
		DirectX::XMFLOAT4X4     lightViewProjection; // ライト視点からのビュー射影行列
		DirectX::XMFLOAT3	    shadowColor;         // 影の色
		float				    shadowBias;          // シャドウバイアス（アーティファクト回避）
		DirectX::XMFLOAT4       edgeColor;
	};

	// スケルタルアニメーション用のボーン変換行列配列
	struct CbSkeleton
	{
		DirectX::XMFLOAT4X4		boneTransforms[256]; // 最大256個のボーン行列
	};

	// 各種シェーダーのポインタ（EnumCount 分用意）
	std::unique_ptr<Shader>					shaders[static_cast<int>(ShaderId::EnumCount)];

	// 各定数バッファ（GPUに送るデータ）
	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;     // シーン情報（ビュー・光源方向など）
	Microsoft::WRL::ComPtr<ID3D11Buffer> skeletonConstantBuffer;  // ボーン変換情報
	Microsoft::WRL::ComPtr<ID3D11Buffer> fogConstantBuffer;       // フォグ・環境光情報
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowConstantBuffer;    // シャドウ情報
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightConstantBuffer;     // ライティング情報
};
