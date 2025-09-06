#include <imgui.h>
#include <algorithm>
#include "System/Graphics.h"
#include "System/Misc.h"
#include "System/GpuResourceUtils.h"
#include "DushUI.h"
#include<cmath>


// コンストラクタ
DushUI::DushUI()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	HRESULT hr;

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(Vertex) * 24;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		// 頂点バッファオブジェクトの生成
		hr = device->CreateBuffer(&buffer_desc, nullptr, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 頂点シェーダー
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = GpuResourceUtils::LoadVertexShader(
			device,
			"Data/Shader/SpriteVS.cso",
			inputElementDesc,
			ARRAYSIZE(inputElementDesc),
			inputLayout.GetAddressOf(),
			vertexShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	}

	// ピクセルシェーダー
	{
		hr = GpuResourceUtils::LoadPixelShader(
			device,
			"Data/Shader/SpritePS.cso",
			pixelShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// テクスチャ
	{
		hr = GpuResourceUtils::LoadTexture(device, "Data/Sprite/dash.png", shaderResourceView.GetAddressOf(), &textureDesc);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

// 更新処理
void DushUI::Update(float elapsedTime, const bool dushflug)

{
	auto updateAmount = [&](float& amount)
	{
		amount += (1.0f / 40.0f) * elapsedTime; // 40秒で1.0になる
		if (amount > 1.0f)
		{
			amount = 0.0f; // ループさせる
		}
	};
	radialFillAmount == 1.0f;
	if (dushflug)
	{
	}
	if (!dushflug)
	{

		updateAmount(radialFillAmount);
	}
	{
	}
}


//描画処理
void DushUI::Render()
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	//	PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();

	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);

	// サンプラーステート
	ID3D11SamplerState* samplerStates[] =
	{
		renderState->GetSamplerState(SamplerState::LinearWrap)
	};
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	// レンダーステート設定
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));

	// 頂点バッファ設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetInputLayout(inputLayout.Get());

	// 画面サイズ＆テクスチャサイズ取得
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	float textureWidth = static_cast<float>(textureDesc.Width);
	float textureHeight = static_cast<float>(textureDesc.Height);

	// 描画

	DrawFillRadial(dc, 30, 400, 70, 70, 0, 0, textureWidth, textureHeight, radialFillAmount, screenWidth, screenHeight);
}

// 放射塗りつぶし描画
void DushUI::DrawFillRadial(
	ID3D11DeviceContext* dc,
	float dx, float dy, float dw, float dh,
	float sx, float sy, float sw, float sh,
	float amount,
	float screenWidth,
	float screenHeight) const
{
	//  +----+----+
	//  |0   |1   |2
	//  |    |    |
	//  +----+----+
	//  |3   |4   |5
	//  |    |    |
	//  +----+----+
	//   6    7    8
	Vertex p[9];
	p[0].position.x = p[3].position.x = p[6].position.x = dx;
	p[1].position.x = p[4].position.x = p[7].position.x = dx + dw * 0.5f;
	p[2].position.x = p[5].position.x = p[8].position.x = dx + dw;

	p[0].position.y = p[1].position.y = p[2].position.y = dy;
	p[3].position.y = p[4].position.y = p[5].position.y = dy + dh * 0.5f;
	p[6].position.y = p[7].position.y = p[8].position.y = dy + dh;

	p[0].texcoord.x = p[3].texcoord.x = p[6].texcoord.x = sx;
	p[1].texcoord.x = p[4].texcoord.x = p[7].texcoord.x = sx + sw * 0.5f;
	p[2].texcoord.x = p[5].texcoord.x = p[8].texcoord.x = sx + sw;

	p[0].texcoord.y = p[1].texcoord.y = p[2].texcoord.y = sy;
	p[3].texcoord.y = p[4].texcoord.y = p[5].texcoord.y = sy + sh * 0.5f;
	p[6].texcoord.y = p[7].texcoord.y = p[8].texcoord.y = sy + sh;

	// 頂点編集開始
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	Vertex* v = reinterpret_cast<Vertex*>(mappedSubresource.pData);

	// 進行度(amount)に応じてスプライトが円を描くように表示されるようにする
	{
		// 0    1    2
		// +----+----+
		// |＼7 | 0／|
		// | 6＼|／1 |
		// 3	4    5
		// +----+----+
		// | 5／|＼2 |
		// |／4 | 3＼|
		// 6	7    8
		// +----+----+
		// 全体の進行度(amount)に応じた8つの三角形毎の進行度(t)を算出
		float t0 = std::clamp(amount * 8.0f - 0.0f, 0.0f, 1.0f);
		float t1 = std::clamp(amount * 8.0f - 1.0f, 0.0f, 1.0f);
		float t2 = std::clamp(amount * 8.0f - 2.0f, 0.0f, 1.0f);
		float t3 = std::clamp(amount * 8.0f - 3.0f, 0.0f, 1.0f);
		float t4 = std::clamp(amount * 8.0f - 4.0f, 0.0f, 1.0f);
		float t5 = std::clamp(amount * 8.0f - 5.0f, 0.0f, 1.0f);
		float t6 = std::clamp(amount * 8.0f - 6.0f, 0.0f, 1.0f);
		float t7 = std::clamp(amount * 8.0f - 7.0f, 0.0f, 1.0f);
		// 頂点の線形補完関数
		auto lerp = [&](const Vertex& v1, const Vertex& v2, float t) -> Vertex
		{
			DirectX::XMVECTOR P1 = DirectX::XMLoadFloat3(&v1.position);
			DirectX::XMVECTOR P2 = DirectX::XMLoadFloat3(&v2.position);
			DirectX::XMVECTOR P3 = DirectX::XMVectorLerp(P1, P2, t);
			DirectX::XMVECTOR T1 = DirectX::XMLoadFloat2(&v1.texcoord);
			DirectX::XMVECTOR T2 = DirectX::XMLoadFloat2(&v2.texcoord);
			DirectX::XMVECTOR T3 = DirectX::XMVectorLerp(T1, T2, t);
			Vertex out;
			DirectX::XMStoreFloat3(&out.position, P3);
			DirectX::XMStoreFloat2(&out.texcoord, T3);
			return out;
		};
		// 三角形0
		v[0] = lerp(p[1], p[2], t0); // 左上
		v[1] = p[4]; // 左下
		v[2] = p[1]; // 右下
		// 三角形1
		v[3] = lerp(p[2], p[5], t1); // 右上
		v[4] = p[4]; // 左下
		v[5] = p[2]; // 右下
		// 三角形2
		v[6] = p[4]; // 左上
		v[7] = p[5]; // 左上
		v[8] = lerp(p[5], p[8], t2); // 右上
		// 三角形3
		v[9] = p[4];
		v[10] = p[8];
		v[11] = lerp(p[8], p[7], t3); // 右下
		// 三角形4
		v[12] = p[4];
		v[13] = lerp(p[7], p[6], t4); // 左下
		v[14] = p[7];
		// 三角形5
		v[15] = lerp(p[6], p[3], t5); // 左上
		v[16] = p[4];
		v[17] = p[6];
		// 三角形6
		v[18] = lerp(p[3], p[0], t6); // 左上
		v[19] = p[3];
		v[20] = p[4];
		// 三角形7
		v[21] = p[0];
		v[22] = lerp(p[0], p[1], t7); // 右上
		v[23] = p[4];
	}

	// 0    1    2
	// +----+----+
	// |＼7 | 0／|
	// | 6＼|／1 |
	// 3	4    5
	// +----+----+
	// | 5／|＼2 |
	// |／4 | 3＼|
	// 6	7    8
	// +----+----+




		// NDC座標変換＆UV座標変換
	for (int i = 0; i < 24; ++i)
	{
		v[i].color = { 1, 1, 1, 1 };
		v[i].position.x = 2.0f * v[i].position.x / screenWidth - 1.0f;
		v[i].position.y = 1.0f - 2.0f * v[i].position.y / screenHeight;
		v[i].position.z = 0.0f;
		v[i].texcoord.x = v[i].texcoord.x / textureDesc.Width;
		v[i].texcoord.y = v[i].texcoord.y / textureDesc.Height;
	}

	// 頂点編集終了
	dc->Unmap(vertexBuffer.Get(), 0);

	// シェーダーリソースビュー設定
	dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	// プリミティブトポロジー設定
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 描画
	dc->Draw(24, 0);
}
