#pragma once

#include "Objects/Object.h"
#include "System/RenderContext.h"
#include "System/Model.h"
#include "System/ModelRenderer.h"
#include "System/ShapeRenderer.h"
#include "System/LoadTextures.h"

#include <vector>


class ObjectManager
{
public:
	// インスタンス取得
	static ObjectManager& Instance()
	{
		static ObjectManager instance;
		return instance;
	}

	ObjectManager() = default;
	~ObjectManager() = default;

	void Initialize();
	void update();

	void Render(RenderContext& rc, ModelRenderer* renderer);
	void RenderDebugPrimitive(RenderContext& rc, ShapeRenderer* renderer);

private:
	std::vector<Object> smallChair;
	std::vector<Object> largeChair;
	std::vector<Object> counter;
	std::vector<Object> table;

	std::unique_ptr<Model> models[4];
	std::unique_ptr<LoadTextures> textures[3];
};

