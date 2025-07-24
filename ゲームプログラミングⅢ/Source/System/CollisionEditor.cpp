#include <fstream>

#include "imgui.h"
#include "../External/Json/json.hpp"
using json = nlohmann::json;

#include "CollisionEditor.h"
#include "Collision.h"

void to_json(json& j, const AABB& aabb)
{
    j = {
        {"type", "AABB"},
        {"position", {aabb.position.x, aabb.position.y, aabb.position.z}},
        {"size", {aabb.size.x, aabb.size.y, aabb.size.z}}
    };
}

void from_json(const json& j, AABB& aabb)
{
    auto pos = j.at("position");
    aabb.position.x = pos[0].get<float>(); aabb.position.y = pos[1].get<float>(); aabb.position.z = pos[2].get<float>();
    auto size = j.at("size");
    aabb.size.x = size[0].get<float>(); aabb.size.y = size[1].get<float>(); aabb.size.z = size[2].get<float>();
}

void _Named(char* name, CollisionType type);

/**
* @brief 
*/
CollisionDataLoader::CollisionDataLoader()
{
}

/**
* @brief
*/
CollisionDataLoader::~CollisionDataLoader()
{
}

void CollisionDataLoader::Save(std::vector<AABB>& aabb)
{
    json j = aabb;

    std::ofstream ofs("./Data/Settings/Collision/hitBoxes.json");
    if (!ofs.is_open())
    {
        OutputDebugStringA("not found file name. So can't save file");
        return;
    }
    ofs << j.dump(4);
    ofs.close();
}

/**
* @brief
*/
template <typename T>
void CollisionDataLoader::Load(std::vector<T>& array)
{

}

#pragma region テンプレートの種類別読み込み
template <>
void CollisionDataLoader::Load<AABB>(std::vector<AABB>& array)
{
    std::ifstream ifs("./Data/Settings/Collision/hitBoxes.json");
    if (!ifs.is_open())
    {
        OutputDebugStringA("not found file name. So can't Load file");
        return;
    }
    json j;
    ifs >> j;
    ifs.close();

    array = j.get<std::vector<AABB>>();
}

template <>
void CollisionDataLoader::Load<Sphere>(std::vector<Sphere>& array)
{

}
#pragma endregion 

/**
* @brief
*/
void CollisionEditor::Initialize()
{
    loader.Load(volumes);
}

/**
* @brief
*/
bool CollisionEditor::Collision(const DirectX::XMFLOAT3& targetPosition, float range, DirectX::XMFLOAT3& outPos)
{
    bool flag = false;
    for (auto& box : volumes)
    {
        DirectX::XMFLOAT3 size = box.size;
        DirectX::XMFLOAT3 position = box.position;
        DirectX::XMFLOAT3 minPos = { position.x - size.x / 2, position.y / 2 - size.y, position.z - size.z / 2 };
        DirectX::XMFLOAT3 maxPos = { position.x + size.x / 2, position.y / 2 + size.y, position.z + size.z / 2 };
        if (Collision::AABBVsSphere(minPos, maxPos, targetPosition, range, outPos))
            flag = true;
    }
    return flag;
}


bool CollisionEditor::Collision(const DirectX::XMFLOAT3& targetPosition, float range, DirectX::XMFLOAT3& outPos, DirectX::XMFLOAT3& pushDir)
{
#if 0
    bool flag = false;
    DirectX::XMVECTOR totalPush = DirectX::XMVectorZero();
    DirectX::XMVECTOR totalPos = DirectX::XMLoadFloat3(&targetPosition);
    int hitCount = 0;

    for (auto& box : volumes)
    {
        DirectX::XMFLOAT3 size = box.size;
        DirectX::XMFLOAT3 position = box.position;
        DirectX::XMFLOAT3 minPos = { position.x - size.x / 2, position.y / 2 - size.y, position.z - size.z / 2 };
        DirectX::XMFLOAT3 maxPos = { position.x + size.x / 2, position.y / 2 + size.y, position.z + size.z / 2 };

        DirectX::XMFLOAT3 tempOutPos;
        DirectX::XMFLOAT3 tempPushDir;

        if (Collision::AABBVsSphere(minPos, maxPos, targetPosition, range, tempOutPos, tempPushDir))
        {
            DirectX::XMVECTOR pushVec = DirectX::XMLoadFloat3(&tempPushDir);
            totalPush = DirectX::XMVectorAdd(totalPush, pushVec);
            hitCount++;
            flag = true;
        }
    }

    if (flag && hitCount > 0)
    {
        // 合成された押し出し方向を正規化
        DirectX::XMVECTOR normalizedPush = DirectX::XMVector3Normalize(totalPush);

        // 押し出し距離（例：0.1f）
        const float pushDistance = 0.1f;
        DirectX::XMVECTOR finalPos = DirectX::XMVectorAdd(totalPos, DirectX::XMVectorScale(normalizedPush, pushDistance));

        DirectX::XMStoreFloat3(&outPos, finalPos);
        DirectX::XMStoreFloat3(&pushDir, normalizedPush);
    }
    else
    {
        outPos = targetPosition;
        pushDir = { 0.0f, 0.0f, 0.0f };
    }

    return flag;
#else


    bool flag = false;
    float maxDepth = 0.0f;
    DirectX::XMVECTOR bestPush = DirectX::XMVectorZero();
    DirectX::XMVECTOR bestPos = DirectX::XMLoadFloat3(&targetPosition);

    for (auto& box : volumes)
    {
        DirectX::XMFLOAT3 size = box.size;
        DirectX::XMFLOAT3 position = box.position;
        DirectX::XMFLOAT3 minPos = { position.x - size.x / 2, position.y / 2 - size.y, position.z - size.z / 2 };
        DirectX::XMFLOAT3 maxPos = { position.x + size.x / 2, position.y / 2 + size.y, position.z + size.z / 2 };

        DirectX::XMFLOAT3 tempOutPos;
        DirectX::XMFLOAT3 tempPushDir;

        if (Collision::AABBVsSphere(minPos, maxPos, targetPosition, range, tempOutPos, tempPushDir))
        {
            DirectX::XMVECTOR centerVec = DirectX::XMLoadFloat3(&targetPosition);
            DirectX::XMVECTOR outVec = DirectX::XMLoadFloat3(&tempOutPos);
            float depth = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(outVec, centerVec)));

            if (depth > maxDepth)
            {
                maxDepth = depth;
                bestPush = DirectX::XMLoadFloat3(&tempPushDir);
                bestPos = outVec;
                flag = true;
            }
        }
    }

    if (flag)
    {
        // 押し出し方向を正規化
        DirectX::XMVECTOR normalizedPush = DirectX::XMVector3Normalize(bestPush);

        // 押し出し位置を補間して滑らかにする
        DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat3(&targetPosition);
        DirectX::XMVECTOR smoothedPos = DirectX::XMVectorLerp(currentPos, bestPos, 0.5f); // 補間係数は調整可能

        DirectX::XMStoreFloat3(&outPos, smoothedPos);
        DirectX::XMStoreFloat3(&pushDir, normalizedPush);
    }
    else
    {
        outPos = targetPosition;
        pushDir = { 0.0f, 0.0f, 0.0f };
    }

    return flag;


#endif
}



/**
* @brief
*/
void CollisionEditor::Render(const RenderContext& rc, ShapeRenderer* renderer)
{
    if (!isVisible)return;

    for (auto& boxes : volumes)
    {
        renderer->RenderBox(rc, { boxes.position.x, boxes.position.y , boxes.position.z },
            { 0.0f, 0.0f, 0.0f },
            { boxes.size.x / 2, boxes.size.y / 2 ,boxes.size.z / 2 },
            { 1,0,0,1 });
    }
}

/**
* @brief
*/
void CollisionEditor::DrawDebug()
{
    /// 親ImGuiウィンドウ
    if (ImGui::Begin("CollisionEditor"))
    {
        ImVec2 size = ImGui::GetWindowSize();
        /// 保存機能
        if (ImGui::Button("Save"))
            loader.Save(volumes);

        /// 読み込み機能
        if (ImGui::Button("Load"))
        {
            loader.Load(volumes);
        }
        
        ImGui::Checkbox("isVisible", &isVisible);

        static int type = 0;
        /// 当たり判定の種類を選択
        ImGui::SetNextItemWidth(size.x / 4);
        ImGui::InputInt("collisionType", &type , 0, 2);
        ImGui::SameLine();

        /// typeの値をCollisionType型に変換
        colType = static_cast<CollisionType>(type);
        /// ImGuiのButtonのラベル用
        char buf[256] = "Add AABB";
        _Named(buf, colType);

        /// 当たり判定の追加
        if (ImGui::Button(buf))
            AddCollision(colType);

        /// 箱当たり判定群
        if(ImGui::TreeNode("BoxCollisions"))
        {
            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImGui::BeginChild("##", avail, true, ImGuiWindowFlags_HorizontalScrollbar);

            int i = 0;
            for (auto& boxes : volumes)
            {
                char buf[256];
                sprintf_s(buf, sizeof(buf), "box_%d", i);

                ImGui::Separator();
                ImGui::PushID(i);
                ImGui::Text(buf);
                ImGui::DragFloat3("Position", &boxes.position.x, 0.01f);
                ImGui::DragFloat3("Size", &boxes.size.x, 0.01f);
                ImGui::PopID();

                ++i;
            }
            ImGui::EndChild();
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

/**
* @brief コリジョンの追加
*/
void CollisionEditor::AddCollision(CollisionType type)
{

    switch (type)
    {
    case CollisionType::AABB:
        volumes.emplace_back(); ///< AABBの当たり判定の追加
        OutputDebugStringA("Add box collider\n");

        break;
    case CollisionType::Sphere:
#if 0
        volumes.emplace_back();
        OutputDebugStringA("Add box collider\n");
#endif
        break;
    default:
        break;
    }
}

void _Named(char* name, CollisionType type)
{
    static CollisionType prevType = type;

    if (prevType == type)return;

    switch (type)
    {
    case CollisionType::AABB:   name = "Add AABB";      break;
    case CollisionType::Sphere: name = "Add Sphere";    break;
    default:
        break;
    }

    prevType = type;
}
