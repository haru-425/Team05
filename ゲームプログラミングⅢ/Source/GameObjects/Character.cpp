#include"Character.h"

//行列更新処理
void Character::UpdateTransform()
{
    //スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
    //回転行列
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);
    //位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //３つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    //計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}

//デバッグ用の描画
void Character::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    //衝突判定用
    renderer->RenderSphere(rc, position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
    //衝突判定用のデバッグ円柱を描画
    renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

    //実験
    //renderer->RenderBox(rc, position, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(1, 1, 1), DirectX::XMFLOAT4(1, 1, 1, 1));
}

bool Character::ApplyDamage(int damage, float invincibleTime)
{
    if (damage == 0) return false;
    if (health <= 0) return false;

    if (invincibleTimer > 0.0f)return false;

    invincibleTimer = invincibleTime;

    health -= damage;

    if (health <= 0)
    {
        OnDead();
    }
    else
    {
        OnDamage();
    }

    return true;
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;

    //進行ベクトルがゼロベクトルの場合は処理する必要なし
    float length = sqrtf(vx * vx + vz * vz);
    if (length < 0.001f)return;

    //進行ベクトルを単位ベクトル化
    vx /= length;
    vz /= length;

    //自身の回転値から前方向を求める
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    //回転角を求めるため、２つの単位ベクトルの内積を計算する
    float dot = (frontX * vx) + (frontZ * vz);

    //内積値は‐１.0～1.0で表現されており、２つの単位ベクトルの角度が
    //小さいほど1.0に近づくという性質を利用して回転速度を調整する
    float rot = 1.0f - dot;
    if (rot > speed)rot = speed;

    //左右判定を行うために２つの単位ベクトルの外積を計算する
    float cross = (frontZ * vx) - (frontX * vz);

    //2Dの外積値が正の場合か負の場合によって左右判定が行える
    //左右の判定を行うことによって左回転を選択する
    if (cross < 0.0f)
    {
        //angle.y -= speed;
        angle.y -= rot;
        //boltTransform.Angle.y -= rot;
        //bodyTransform.Angle.y -= rot;
    }
    else
    {
        //angle.y += speed;
        angle.y += rot;
        //boltTransform.Angle.y += rot;
        //bodyTransform.Angle.y += rot;
    }
}

void Character::Move(float elapsedTime, float vx, float vz, float speed)
{
    moveVecX = vx;
    moveVecZ = vz;

    maxMoveSpeed = speed;
}

void Character::UpdateVelocity(float elapsedTime)
{
    UpdateVerticalVelocity(elapsedTime);
    UpdateHorizontalVelocity(elapsedTime);
    UpdateVerticalMove(elapsedTime);
    UpdateHorizontalMove(elapsedTime);

    ////重力処理
    //velocity.y += gravity * elapsedTime;
    ////移動処理
    //position.y += velocity.y * elapsedTime;
    ////地面判定
    //if (position.y < 0.0f)
    //{
    //    position.y = 0.0f;
    //    velocity.y = 0.0f;

    //    //接地した
    //    if (!isGround)
    //    {
    //        OnLanding();
    //    }
    //    isGround = true;
    //}
    //else
    //{
    //    isGround = false;
    //}
}

void Character::UpdateInvicibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}

void Character::UpdateVerticalVelocity(float elapsedTime)
{
    velocity.y += gravity * elapsedTime;
}

void Character::UpdateVerticalMove(float elapsedTime)
{
    position.y += velocity.y * elapsedTime;

    if (position.y < 0.0f)
    {
        position.y = 0.0f;

        if (!isGround)
        {
            OnLanding();
        }
        IsGround();
        velocity.y = 0.0f;
    }
    else
    {
        isGround = false;
    }
}

void Character::UpdateHorizontalVelocity(float elapsedTime)
{
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        float friction = this->friction * elapsedTime;

        if (!isGround)friction *= airControl;

        if (length > friction)
        {
            float vx = velocity.x / length;
            float vz = velocity.z / length;
            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        else
        {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    if (length <= maxMoveSpeed)
    {
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
        if (moveVecLength > 0.0f)
        {
            float acceleration = this->acceleration * elapsedTime;
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;

            if (!isGround)acceleration *= airControl;

            float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                float vx = velocity.x / length;
                float vz = velocity.z / length;

                velocity.x = vx * maxMoveSpeed;
                velocity.z = vz * maxMoveSpeed;
            }
        }
    }

    moveVecX = 0.0f;
    moveVecZ = 0.0f;
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
    position.x += velocity.x * elapsedTime;
    position.z += velocity.z * elapsedTime;
}

