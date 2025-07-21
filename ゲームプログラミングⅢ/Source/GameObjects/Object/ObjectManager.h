#pragma once

#include "Object/Object.h"
#include "System/RenderContext.h"
#include "System/Model.h"
#include "System/ModelRenderer.h"
#include "System/ShapeRenderer.h"
#include "System/LoadTextures.h"

#include <vector>
#include <memory>

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
	void Update(float elapsedTime);

	void Render(RenderContext& rc, ModelRenderer* renderer);
	void RenderDebugPrimitive(RenderContext& rc, ShapeRenderer* renderer);

	void DebugGUI();

	enum class ObjectLavel {
		SmallChair = 0,
		LargeChair,
		Counter,
		Table,
		TV_A,
		TV_B,
		Robot_Arm,
	};

private:
	static const int OBJ_MAX = 7;
	inline static const float defaultScale = 0.01f;

	std::vector<std::unique_ptr<Object>> objects[OBJ_MAX];

	std::unique_ptr<Model> models[OBJ_MAX];
	std::unique_ptr<LoadTextures> textures[OBJ_MAX];
};

