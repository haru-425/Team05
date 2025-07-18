#include <algorithm>
#include "Misc.h"
#include "GpuResourceUtils.h"
#include "ModelRenderer.h"
#include "BasicShader.h"
#include "LambertShader.h"
#include "CustomShader.h"

// コンストラクタ
ModelRenderer::ModelRenderer(ID3D11Device* device)
{
	// シーン用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbScene),
		sceneConstantBuffer.GetAddressOf());

	// スケルトン用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbSkeleton),
		skeletonConstantBuffer.GetAddressOf());

	// fog用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(fogConstants),
		fogConstantBuffer.GetAddressOf());

	// Shadow用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(ShadowConstants),
		shadowConstantBuffer.GetAddressOf());

	// PointLight用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(LightConstants),
		lightConstantBuffer.GetAddressOf());

	// シェーダー生成
	shaders[static_cast<int>(ShaderId::Basic)] = std::make_unique<BasicShader>(device);
	shaders[static_cast<int>(ShaderId::Lambert)] = std::make_unique<LambertShader>(device);
	shaders[static_cast<int>(ShaderId::Custom)] = std::make_unique<CustomShader>(device);
}

// 描画実行
void ModelRenderer::Render(const RenderContext& rc, const DirectX::XMFLOAT4X4& worldTransform, const Model* model, ShaderId shaderId)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シーン用定数バッファ更新
	{
		CbScene cbScene{};
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
		DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);

		cbScene.lightDirection.x = rc.lightDirection.x;
		cbScene.lightDirection.y = rc.lightDirection.y;
		cbScene.lightDirection.z = rc.lightDirection.z;

		cbScene.cameraPosition.x = rc.cameraPosition.x;
		cbScene.cameraPosition.y = rc.cameraPosition.y;
		cbScene.cameraPosition.z = rc.cameraPosition.z;
		cbScene.cameraPosition.w = 1.0f;

		dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
	}

	// fog
	{
		fogConstants data = {};
		data.ambientColor = rc.ambientColor;
		data.fogColor = rc.fogColor;
		data.fogRange = rc.fogRange;

		dc->UpdateSubresource(fogConstantBuffer.Get(), 0, 0, &data, 0, 0);
	}

	// Shadow
	{
		ShadowConstants data = {};
		data.lightViewProjection = rc.lightViewProjection;
		data.shadowColor = rc.shadowColor;
		data.shadowBias = rc.shadowBias;
		data.edgeColor = rc.edgeColor;

		dc->UpdateSubresource(shadowConstantBuffer.Get(), 0, 0, &data, 0, 0);
	};

	// PointLight
	{
		LightConstants data = {};
		memcpy_s(data.pointLights, sizeof(data.pointLights), rc.pointLights, sizeof(rc.pointLights));
		memcpy_s(data.torusLights, sizeof(data.torusLights), rc.torusLights, sizeof(rc.torusLights));
		memcpy_s(data.lineLights, sizeof(data.lineLights), rc.lineLights, sizeof(rc.lineLights));
		data.power = rc.lightPower;
		data.dummy = { 0,0,0 };

		dc->UpdateSubresource(lightConstantBuffer.Get(), 0, 0, &data, 0, 0);
	};

	// 定数バッファ設定
	ID3D11Buffer* vsConstantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		skeletonConstantBuffer.Get(),
		fogConstantBuffer.Get(),
		shadowConstantBuffer.Get(),
		lightConstantBuffer.Get(),
	};
	dc->VSSetConstantBuffers(0, _countof(vsConstantBuffers), vsConstantBuffers);

	ID3D11Buffer* psConstantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		skeletonConstantBuffer.Get(),
		fogConstantBuffer.Get(),
		shadowConstantBuffer.Get(),
		lightConstantBuffer.Get(),
	};
	dc->PSSetConstantBuffers(0, _countof(psConstantBuffers), psConstantBuffers);

	// サンプラステート設定
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::LinearWrap),
		rc.renderState->GetSamplerState(SamplerState::LinearClamp),
		rc.renderState->GetSamplerState(SamplerState::Shadow),
	};
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	// レンダーステート設定
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));

	// ブレンドステート設定
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), nullptr, 0xFFFFFFFF);

	// 描画処理
	Shader* shader = shaders[static_cast<int>(shaderId)].get();
	shader->Begin(rc);

	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// 頂点バッファ設定
		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// スケルトン用定数バッファ更新
		CbSkeleton cbSkeleton{};
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				int nodeIndex = mesh.nodeIndices.at(i);
				const Model::Node& node = model->GetNodes().at(nodeIndex);
				DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
				DirectX::XMMATRIX OffsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
				DirectX::XMMATRIX BoneTransform = OffsetTransform * GlobalTransform * WorldTransform;
				DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[i], BoneTransform);
			}
		}
		else
		{
			const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
			DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
			DirectX::XMMATRIX BoneTransform = GlobalTransform * WorldTransform;
			DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[0], BoneTransform);
		}
		dc->UpdateSubresource(skeletonConstantBuffer.Get(), 0, 0, &cbSkeleton, 0, 0);

		// 描画
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			shader->Update(rc, *subset.material);

			dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
		}
	}

	shader->End(rc);

	// 定数バッファ設定解除
	for (ID3D11Buffer*& vsConstantBuffer : vsConstantBuffers) { vsConstantBuffer = nullptr; }
	for (ID3D11Buffer*& psConstantBuffer : psConstantBuffers) { psConstantBuffer = nullptr; }
	dc->VSSetConstantBuffers(0, _countof(vsConstantBuffers), vsConstantBuffers);
	dc->PSSetConstantBuffers(0, _countof(psConstantBuffers), psConstantBuffers);

	// サンプラステート設定解除
	/*for (ID3D11SamplerState*& samplerState : samplerStates) { samplerState = nullptr; }
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);*/
}
