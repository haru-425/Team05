#pragma once
#include "GameObject.h"
#include <DirectXMath.h>
#include <vector>

class battery:public GameObject
{
public:
	battery(){}
	~battery() {}
	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	DirectX::XMFLOAT3 getPos() { return position; }

	void setPos(DirectX::XMFLOAT3 pos) { position = pos; }
private:
};