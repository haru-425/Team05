#include"Character.h"

//�s��X�V����
void Character::UpdateTransform()
{
    //�X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
    //��]�s��
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);
    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //�R�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}

//�f�o�b�O�p�̕`��
void Character::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    //�Փ˔���p
    renderer->RenderSphere(rc, position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
    //�Փ˔���p�̃f�o�b�O�~����`��
    renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

    //����
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

    //�i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
    float length = sqrtf(vx * vx + vz * vz);
    if (length < 0.001f)return;

    //�i�s�x�N�g����P�ʃx�N�g����
    vx /= length;
    vz /= length;

    //���g�̉�]�l����O���������߂�
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    //��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
    float dot = (frontX * vx) + (frontZ * vz);

    //���ϒl�́]�P.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
    //�������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
    float rot = 1.0f - dot;
    if (rot > speed)rot = speed;

    //���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
    float cross = (frontZ * vx) - (frontX * vz);

    //2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
    //���E�̔�����s�����Ƃɂ���č���]��I������
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

    ////�d�͏���
    //velocity.y += gravity * elapsedTime;
    ////�ړ�����
    //position.y += velocity.y * elapsedTime;
    ////�n�ʔ���
    //if (position.y < 0.0f)
    //{
    //    position.y = 0.0f;
    //    velocity.y = 0.0f;

    //    //�ڒn����
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

