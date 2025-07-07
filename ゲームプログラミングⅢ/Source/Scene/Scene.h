#pragma once

//�V�[��
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    //������
    virtual void Initialize() = 0;

    //�I����
    virtual void Finalize() = 0;

    //�X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    virtual void Render() = 0;

    //GUI�`��
    virtual void DrawGUI() = 0;

    //�����������Ă��邩
    bool IsReady()const { return ready; }

    //���������ݒ�
    void SetReady() { ready = true; }

protected:
    // �ǉ� 2025/07/04
    // �|�[�Y��ʂ��ǂ���
    bool isPause = false;

private:
    bool ready = false;

};