#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "RenderContext.h"

// スプライト
class Sprite
{
public:
	Sprite();
	Sprite(const char* filename);

	// 頂点データ
	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

	struct MiniMap
	{
		int Flag;
		float radius;
		float paramatar;
		float sisex;
		float sisey;
		float dummy[3];
	};

	struct Noise
	{
		float time;
		float strength;
		DirectX::XMFLOAT2 dummy;
	};

	// 描画実行
	void Render(const RenderContext& rc,
		float dx, float dy,					// 左上位置
		float dz,							// 奥行
		float dw, float dh,					// 幅、高さ
		float sx, float sy,					// 画像切り抜き位置
		float sw, float sh,					// 画像切り抜きサイズ
		float angle,						// 角度
		float r, float g, float b, float a	// 色
		, bool minimapFlg = false,
		float radius = 0.0f,//半径
		float parameter = 360.0f
	);

	// 描画実行（テクスチャ切り抜き指定なし）
	void Render(const RenderContext& rc,
		float dx, float dy,					// 左上位置
		float dz,							// 奥行
		float dw, float dh,					// 幅、高さ
		float angle,						// 角度
		float r, float g, float b, float a	// 色
	);

	//void DrawGui()const;

	DirectX::XMFLOAT2 GetTextureSize() const { return textureSize; }

	ID3D11ShaderResourceView* GetSRV() const { return shaderResourceView.Get(); }

	void SetNoise(int noise) { this->noise_flag = noise; }
	void SetStrength(int strength) { this->strength = strength; }

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			NoiseShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				minimap_constant_Buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				noise_constant_Buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	float textureWidth = 0;
	float textureHeight = 0;

	float radius = 200.0f;

	float parametar = 360.0f;

	DirectX::XMFLOAT2 textureSize;

	int noise_flag = 0;

	float time;
	float strength;
};
