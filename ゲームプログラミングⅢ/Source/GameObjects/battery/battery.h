#pragma once
#include "GameObject.h"
#include <DirectXMath.h>
#include <vector>
#include "System/LoadTextures.h"

#define RATESPEED 5

class battery:public GameObject
{
public:
	battery();
	~battery();
	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	DirectX::XMFLOAT3 getPos() { return position; }

	void setPos(DirectX::XMFLOAT3 pos) { position = pos; }

	void setModel(std::shared_ptr<Model> model) { this->model = model; }
private:
	std::shared_ptr<Model> model = nullptr;                ///< モデルデータ
	std::shared_ptr<LoadTextures> textures;                /// テクスチャデータ


};