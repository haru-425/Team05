#include "Player.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "imgui.h"

Player::Player()
{
    //model = std::make_unique<Model>("")

    // プレイヤーのパラメータ初期設定
    position.y = 1.5; // カメラの目線を設定するため
    radius = 2;     // デバッグ用
}

Player::~Player()
{
}

void Player::Update(float dt)
{
    // カメラ切り替え処理
    ChangeCamera();

    // 移動処理
    Move(dt);

    // 行列更新処理
    UpdateTransform();
}

void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    if(model)
        renderer->Render(rc, world, model.get(), ShaderId::Lambert);
}

void Player::DrawDebug()
{
    if (ImGui::Begin("Player", nullptr))
    {
        ImGui::InputFloat3("saveDirection", &saveDirection.x);
    }
    ImGui::End();
}

void Player::Move(float dt)
{
    Camera& cam = Camera::Instance();

    DirectX::XMFLOAT3 forward;
    // カメラが切り替わっていないときだけカメラの方向を取る
    if (isChangeCamera == false)
    {
        forward = cam.GetFront();
        saveDirection = forward;
    }
    else
        forward = saveDirection;


    // カメラの方向をXZ面に固定
    forward.y = 0;
    float len = sqrtf(forward.x * forward.x + forward.z * forward.z);
    if (len > 0.0f)
    {
        forward.x /= len;
        forward.z /= len;
    }

    position.x += 5 * forward.x * dt;
    position.z += 5 * forward.z * dt;
}

// カメラ切り替え処理、実際のカメラ切り替えは外部でする
void Player::ChangeCamera()
{
    Mouse& mouse = Input::Instance().GetMouse();

    // カメラ切り替え
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        isChangeCamera = !isChangeCamera;

        if (isChangeCamera == false)
            Camera::Instance().SetFront(saveDirection);
    }
}
