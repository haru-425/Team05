#pragma once

#include <xaudio2.h>
#include <x3daudio.h>
#include <vector>
#include <DirectXMath.h>
#include <windows.h>
#include <mmreg.h>
#include <fstream>

#include <thread>
#include <atomic>
#include <mutex>

using namespace DirectX;

/**
 * @brief 3D�I�[�f�B�I�V�X�e�����Ǘ�����N���X
 *
 * XAudio2��X3DAudio��p���ĕ����̉����i�G�~�b�^�[�j��3D��ԏ�ɔz�u���A
 * ���X�i�[�̈ʒu�E�����ɉ����ė��̉�������������B
 */
class Audio3DSystem
{
public:
	static Audio3DSystem& Instance()
	{
		static Audio3DSystem instance;
		return instance;
	}
	/**
	 * @brief �I�[�f�B�I�V�X�e���̏��������s��
	 *
	 * XAudio2�G���W���̏������A�}�X�^�����O�{�C�X�̐����AX3DAudio�̏��������s���B
	 * �o�̓f�o�C�X�̃`�����l�����i�X�e���I�Ȃǁj���擾�������ێ�����B
	 */
	void Initialize();


	float bgmVolume = 1;
	float seVolume = 1;
	float masterVolume = 1;
	enum class SoundType {
		BGM,
		SE,
		cnt,
	};
	enum class FadeState {
		None,
		FadeIn,
		FadeOut
	};

	struct FadeInfo {
		FadeState state = FadeState::None;
		float timer = 0.0f;
		float duration = 1.0f;
		float startVolume = 0.0f;
		float targetVolume = 1.0f;
	};
	/**
	 * @brief �G�~�b�^�[�i�����j��ǉ�����
	 *
	 * �w�肳�ꂽWAV�t�@�C����ǂݍ��݁A�w��ʒu��3D�����Ƃ��Ēǉ�����B
	 * ���[�v�Đ��ݒ��A���w�����ݒ�A���������Ȃ��iBGM�j���w��\�B
	 *
	 * @param wavPath �����t�@�C���̃p�X�iWAV�`���j
	 * @param pos 3D��ԏ�̉����̈ʒu�iXMFLOAT3�j
	 * @param tag �G�~�b�^�[���ʗp�̃^�O������
	 * @param loop ���[�v�Đ����邩�ǂ����̃t���O�itrue:���[�v�Đ��j
	 * @param isOmnidirectional ���w���������Ƃ��čĐ����邩�ǂ���
	 * @param constantVolume ���������𖳌��ɂ��邩�iBGM���Ɏg�p�j
	 * @param distanceScaler ���������̃X�P�[�����O�l�i�f�t�H���g��1.0f�j
	 */
	void AddEmitter(const char* wavPath, const XMFLOAT3& pos, const std::string& tag, SoundType soundType,
		float volume, bool loop, bool isOmnidirectional = false, bool constantVolume = false, float distanceScaler = 1.0f);

	/**
	 * @brief ���X�i�[�̈ʒu�ƌ������X�V����
	 *
	 * 3D��ԓ��̃��X�i�[�i������j�̈ʒu�ƌ����x�N�g����ݒ肵�A
	 * ����Ɋ�Â�3D�����̌v�Z���s������������B
	 *
	 * @param pos ���X�i�[��3D�ʒu
	 * @param front ���X�i�[�̐��ʕ����x�N�g��
	 * @param up ���X�i�[�̏�����x�N�g��
	 */
	void UpdateListener(const XMFLOAT3& pos, const XMFLOAT3& front, const XMFLOAT3& up);

	/**
	 * @brief ���ׂẴG�~�b�^�[��3D�����������X�V����
	 *
	 * �e�G�~�b�^�[�̈ʒu�Ɋ�Â��A���X�i�[����̋�����������v�Z���A
	 * �{�����[����p���A������3D�����p�����[�^��XAudio2��SourceVoice�ɔ��f������B
	 */
	void UpdateEmitters(float elapsedTime);

	/**
	 * @brief �w�肵���^�O�����G�~�b�^�[�̈ʒu��ύX����
	 *
	 * �o�^�ς݂̃G�~�b�^�[�̒�����A�w��^�O�ƈ�v������̂�T���A
	 * ����3D�ʒu��V�����ʒu�ɍX�V����B
	 *
	 * @param tag �ύX�Ώۂ̃G�~�b�^�[�����ʂ���^�O������
	 * @param newPos �V����3D�ʒu
	 */
	void SetEmitterPositionByTag(const std::string& tag, const XMFLOAT3& newPos);

	/**
	 * @brief WAV�t�@�C����ǂݍ��݁A�t�H�[�}�b�g���ƍĐ��o�b�t�@���Z�b�g�A�b�v����
	 *
	 * �w�肳�ꂽWAV�t�@�C�����特���f�[�^��ǂݍ��݁A
	 * WAVEFORMATEX�\���́AXAUDIO2_BUFFER�\���́A�����f�[�^�o�b�t�@��ݒ肷��B
	 *
	 * @param filename �ǂݍ���WAV�t�@�C���̃p�X
	 * @param outFormat WAV�̃t�H�[�}�b�g�����i�[����\���̂̃|�C���^
	 * @param outBuffer XAUDIO2_BUFFER�\���̂̃|�C���^�i�����f�[�^�ƍĐ����j
	 * @param waveData �����f�[�^���i�[����o�C�g�z��i�Ăяo�����ŊǗ������j
	 * @return �ǂݍ��ݐ����Ȃ�true�A���s�Ȃ�false
	 * @see https://learn.microsoft.com/ja-jp/windows/win32/api/mmreg/ns-mmreg-waveformatex
	 */
	bool LoadWavFile(const char* filename, WAVEFORMATEX* outFormat, XAUDIO2_BUFFER* outBuffer, std::vector<BYTE>& waveData);

	/**
	 * @brief ���ׂẴG�~�b�^�[�̉������Đ��J�n����
	 */
	void PlayAll();

	/**
	 * @brief �w��^�O�������ׂẴG�~�b�^�[�̉������Đ��J�n����
	 *
	 * @param tag �Đ��Ώۂ̃G�~�b�^�[�����ʂ���^�O������
	 */
	void PlayByTag(const std::string& tag);

	/**
	 * @brief �w��^�O�������ׂẴG�~�b�^�[�̉������~����
	 *
	 * @param tag ��~�Ώۂ̃G�~�b�^�[�����ʂ���^�O������
	 */
	void StopByTag(const std::string& tag);

	/**
	* @brief �w��^�O�̃G�~�b�^�[�̂݉��ʂ�ύX����
	*
	* @param tag ���ʕύX�Ώۂ̃^�O������
	* @param volume ����(0~1)
	*/
	void SetVolumeByTag(const std::string& tag, float volume);
	void SetVolumeByAll();

	///**
	// * @brief �G�~�b�^�[�X�V�p�X���b�h�̊J�n
	// */
	//void StartUpdateThread(float elapsedTime);
	//
	///**
	// * @brief �G�~�b�^�[�X�V�p�X���b�h�̒�~
	// */
	//void StopUpdateThread();
	void StartFadeIn(const std::string& tag, float duration);

	void StartFadeOut(const std::string& tag, float duration);
	void UpdateFadeVolumes(float elapsedTime);

private:
	IXAudio2* xAudio2 = nullptr; ///< XAudio2�C���X�^���X�ւ̃|�C���^
	IXAudio2MasteringVoice* masteringVoice = nullptr; ///< �}�X�^�����O�{�C�X�ւ̃|�C���^�i�ŏI�o�́j

	X3DAUDIO_HANDLE x3dAudioHandle; ///< X3DAudio�̃n���h���i���������ɍ쐬�j

	UINT32 outputChannelCount = 2; ///< �o�̓f�o�C�X�̃`�����l�����i��F�X�e���I�Ȃ�2�j

	X3DAUDIO_LISTENER listener; ///< ���X�i�[��3D��ԏ�̏��
	std::thread updateThread; ///< �G�~�b�^�[�̍X�V�������s���o�b�N�O���E���h�X���b�h
	std::atomic<bool> running = false; ///< �X���b�h���s���t���O
	std::mutex dataMutex; ///< ���X�i�[�ƃG�~�b�^�[�ւ̃A�N�Z�X�ی�p�~���[�e�b�N�X

	/**
	 * @brief ������\���\���́i�G�~�b�^�[�j
	 */
	struct Emitter {
		X3DAUDIO_EMITTER emitterDesc; ///< X3DAudio�ɂ�鉹�������ɕK�v�ȏ��\����
		IXAudio2SourceVoice* sourceVoice; ///< �X�̉����Đ����s���\�[�X�{�C�X
		WAVEFORMATEX waveFormat; ///< WAV�t�@�C���̃t�H�[�}�b�g���
		XAUDIO2_BUFFER buffer; ///< �����f�[�^�ƍĐ������i�[����\����
		std::vector<BYTE> waveData; ///< WAV�t�@�C���̎��f�[�^�i�o�C�g��j
		XMFLOAT3 position; ///< 3D��ԏ�̉����̈ʒu
		std::string tag; ///< �G�~�b�^�[���ʗp�̕�����^�O
		SoundType soundType = SoundType::SE;
		bool loop = false; ///< ���[�v�Đ��t���O
		bool isOmnidirectional = false; ///< ���w���������t���O�itrue�Ȃ�����Ɋ֌W�Ȃ�������������j
		bool constantVolume = false; ///< ���������𖳌��ɂ���t���O�iBGM�ȂǂɎg�p�j
		float distanceScaler = 1.0f; ///< ���������X�P�[���[�i���������Ȃ��Ȃ�0.0f�j
		float Volume;
		FadeInfo fadeInfo;
	};

	std::vector<Emitter> emitters; ///< �o�^�ς݂̂��ׂẴG�~�b�^�[�̃��X�g


	template<typename T>
	T Lerp(T a, T b, T t)
	{
		return a + (b - a) * t;
	}
};
