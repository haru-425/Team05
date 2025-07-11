#pragma once

struct GameSettings
{
    float sensitivity   = 1.00f; // ���x
    float masterVolume  = 1.00f; // �}�X�^�[����
    float bgmVolume     = 1.00f; // BGM����
    float seVolume      = 1.00f; // SE����
};

/**
 * @brief �Q�[���̐ݒ����ێ��E�Ǘ�����V���O���g���}�l�[�W��
 *
 * GameSettings �\���̂̎擾�E�ۑ��E�ǂݍ��݁E���Z�b�g�Ȃǂ̏�����񋟂��܂�
 * 
 * @date 2025/07/04
 */
class SettingsManager
{
private:
    SettingsManager() {}
    ~SettingsManager() {}

public:
    static SettingsManager& Instance()
    {
        static SettingsManager instance;
        return instance;
    }

    /**
    * @brief �ݒ�����擾
    * @date 2025/07/04
    */
    const GameSettings& GetGameSettings() const { return settings; }

    /**
    * @brief �ݒ��ύX����
    * @date 2025/07/04
    */
    void SetGameSettings(const GameSettings& gameSettings) { settings = gameSettings; }

    /**
    * @brief �t�@�C������ݒ�������o���܂�
    * 
    * �ۑ���ɖ�肪����ꍇ�A���������܂���
    * 
    * @date 2025/07/04
    */
    void Save();

    /**
    * @brief �t�@�C������ݒ��ǂݍ��݂܂�
    * 
    * �ǂݍ��݌��ɖ�肪����ꍇ�A���������܂���
    * 
    * @date 2025/07/04
    */
    void Load();

    /**
    * @brief �ݒ�����������܂�
    * 
    * GameSettings �\���̂̃f�t�H���g�R���X�g���N�^���g�p���A
    * �S���ڂ�������Ԃɖ߂�
    * 
    * @date 2025/07/04
    */
    void ResetToDefault() {
        settings = GameSettings();
    }

private:
    GameSettings settings; // ���݂̐ݒ��ۑ����郁���o�ϐ�
};