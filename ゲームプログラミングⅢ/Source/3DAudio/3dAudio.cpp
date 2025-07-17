#include "3dAudio.h"
#include <cassert>
#include <combaseapi.h>
#include <iostream>
#include <mmdeviceapi.h>         // IMMDevice, IMMDeviceEnumerator
#include <audioclient.h>         // IAudioClient, GetMixFormat
#include <functiondiscoverykeys_devpkey.h> // PKEY_Device_... �Ȃǁi�g�p���Ă��Ȃ���Εs�v�j
#include <combaseapi.h>          // CoCreateInstance �ȂǁiCoInitialize�܂߁j
#include <objbase.h>             // CoInitializeEx�i�Ăяo�����p�j

#include <wrl/client.h>          // Microsoft::WRL::ComPtr ���g���ꍇ
#include "GameObjects/Aircon/AirconManager.h"
using Microsoft::WRL::ComPtr;
/// @brief �f�t�H���g�̃����_�[�f�o�C�X�i�o�́j�̃`�����l���}�X�N���擾����
///
/// Windows��WASAPI���g�p���āA���ݑI������Ă���f�t�H���g�̏o�̓f�o�C�X�i�X�s�[�J�[�Ȃǁj��
/// �Ή����Ă���X�s�[�J�[�\���i��F�X�e���I�A5.1ch�Ȃǁj�Ɋ�Â��`�����l���}�X�N���擾���܂��B
///
/// ���̃}�X�N�́AX3DAudio��XAudio2�ł�DSP�ݒ�i`X3DAUDIO_DSP_SETTINGS::DstChannelCount`�Ȃǁj�ɗ��p�ł��܂��B
///
/// @param[out] outMask �`�����l���}�X�N���i�[�����DWORD�i��F`SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT`�j
/// @retval true �`�����l���}�X�N�̎擾�ɐ���
/// @retval false �G���[�i�f�o�C�X�擾��AudioClient���������s�Ȃǁj
///
/// @note COM�̏������i`CoInitializeEx`�j�͌Ăяo�����ōs���Ă����K�v������܂��B
/// @see https://learn.microsoft.com/en-us/windows/win32/coreaudio/core-audio-apis-in-windows
bool GetOutputChannelMaskFromWASAPI(DWORD& outMask)
{
	HRESULT hr;

	// IMMDeviceEnumerator ���쐬�i�o�̓f�o�C�X�̗񋓁j
	ComPtr<IMMDeviceEnumerator> deviceEnumerator;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
		CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr)) return false;

	// �f�t�H���g�̃����_�[�i�o�́j�f�o�C�X���擾�ieConsole�͈�ʗp�r�j
	ComPtr<IMMDevice> defaultDevice;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	if (FAILED(hr)) return false;

	// IAudioClient ���擾�i�f�o�C�X�̃t�H�[�}�b�g���ɃA�N�Z�X�j
	ComPtr<IAudioClient> audioClient;
	hr = defaultDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &audioClient);
	if (FAILED(hr)) return false;

	// ���݂̃~�b�N�X�t�H�[�}�b�g���擾�iWAVEFORMATEXTENSIBLE���ǂ����𔻒�j
	WAVEFORMATEX* waveFormat = nullptr;
	hr = audioClient->GetMixFormat(&waveFormat);
	if (FAILED(hr) || waveFormat == nullptr) return false;

	// WAVEFORMATEXTENSIBLE �\���̂��m�F���A�`�����l���}�X�N���擾
	if (waveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		auto fmtEx = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(waveFormat);
		outMask = fmtEx->dwChannelMask;
	}
	else
	{
		// ��g���t�H�[�}�b�g�̏ꍇ�i��F�X�e���I�Ȃǁj�A�f�t�H���g�őO�����E�Ƃ݂Ȃ�
		outMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	}

	// �t�H�[�}�b�g�\���̂̉���iWASAPI�̎d�l�ɏ]���j
	CoTaskMemFree(waveFormat);
	return true;
}
/**
 * @brief WAV�t�@�C����ǂݍ��݁A�t�H�[�}�b�g�ƃo�b�t�@���Z�b�g�A�b�v����
 *
 * �t�@�C������RIFF�`����WAV��ǂݍ��݁AWAVEFORMATEX�\���̂�XAUDIO2_BUFFER�\���̂�
 * ��������ݒ肷��BRIFF�`�����N��fmt�`�����N�Adata�`�����N�̓ǂݍ��݂��s���A
 * �o�C�i���f�[�^�𐳂����i�[����B
 *
 * @param filename �ǂݍ���WAV�t�@�C���p�X
 * @param outFormat WAV�̃t�H�[�}�b�g�����i�[����|�C���^
 * @param outBuffer XAUDIO2_BUFFER�\���̂̃|�C���^�i�Đ��o�b�t�@���j
 * @param waveData �����f�[�^�o�C�g�z��i�ǂݍ��񂾉������i�[����j
 * @return true �����Afalse ���s�i�t�@�C�����s���A�t�H�[�}�b�g�s��v�Ȃǁj
 *
 * @see https://learn.microsoft.com/ja-jp/windows/win32/api/mmreg/ns-mmreg-waveformatex
 */
bool Audio3DSystem::LoadWavFile(const char* filename, WAVEFORMATEX* outFormat, XAUDIO2_BUFFER* outBuffer, std::vector<BYTE>& waveData) {

	std::ifstream file(filename, std::ios::binary);
	if (!file) return false; ///< �t�@�C���I�[�v�����s����false��Ԃ�

	DWORD chunkType;
	DWORD chunkDataSize;
	DWORD fileType;
	DWORD fmtSize;

	// ===== RIFF�`�����N�w�b�_�̓ǂݍ��� =====
	file.read(reinterpret_cast<char*>(&chunkType), sizeof(DWORD));
	// RIFF�`�����N�^�C�v�̓��g���G���f�B�A���� 'RIFF' �� 'FFIR' �Ƃ��ēǂݍ��܂��
	if (chunkType != 'FFIR') return false; ///< RIFF�`�����N�^�C�v�s��v�œǂݍ��ݎ��s

	file.read(reinterpret_cast<char*>(&chunkDataSize), sizeof(DWORD)); // �`�����N�S�̃T�C�Y
	file.read(reinterpret_cast<char*>(&fileType), sizeof(DWORD));      // WAVE�^�C�v
	if (fileType != 'EVAW') return false; ///< WAVE�^�C�v�s��v�œǂݍ��ݎ��s

	// ===== fmt�`�����N�̓ǂݍ��� =====
	file.read(reinterpret_cast<char*>(&chunkType), sizeof(DWORD));
	if (chunkType != ' tmf') return false; ///< fmt�`�����N�^�C�v�s��v

	file.read(reinterpret_cast<char*>(&fmtSize), sizeof(DWORD));
	std::vector<BYTE> fmtChunk(fmtSize);
	file.read(reinterpret_cast<char*>(fmtChunk.data()), fmtSize);
	memcpy(outFormat, fmtChunk.data(), fmtSize); ///< �t�H�[�}�b�g�����R�s�[

	// ===== data�`�����N�̌����Ɠǂݍ��� =====
	while (file.read(reinterpret_cast<char*>(&chunkType), sizeof(DWORD))) {
		file.read(reinterpret_cast<char*>(&chunkDataSize), sizeof(DWORD));
		if (chunkType == 'atad') { ///< data�`�����N����
			waveData.resize(chunkDataSize);
			file.read(reinterpret_cast<char*>(waveData.data()), chunkDataSize);
			break;
		}
		else {
			// �s���ȃ`�����N�̓X�L�b�v
			file.seekg(chunkDataSize, std::ios::cur);
		}
	}

	// XAUDIO2_BUFFER�\���̂����������A�f�[�^���Z�b�g
	ZeroMemory(outBuffer, sizeof(XAUDIO2_BUFFER));
	outBuffer->AudioBytes = static_cast<UINT32>(waveData.size()); ///< �����f�[�^�o�C�g��
	outBuffer->pAudioData = waveData.data();                       ///< �����f�[�^�|�C���^
	outBuffer->Flags = XAUDIO2_END_OF_STREAM;                      ///< �X�g���[���I���t���O
	return true;
}

/**
 * @brief XAudio2�����X3DAudio�̏��������s��
 *
 * COM�̃}���`�X���b�h���f���������AXAudio2�C���X�^���X�����A
 * �}�X�^�[�{�C�X�쐬�A�o�̓`�����l�����擾�AX3DAudio�̏��������s���B
 * ���X�i�[�̏��������������Őݒ肷��B
 */
void Audio3DSystem::Initialize()
{
	// COM�������i�}���`�X���b�h���f���j
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// XAudio2�C���X�^���X����
	HRESULT hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr)); ///< ���s���̓A�T�[�g

	// �}�X�^�����O�{�C�X�����i�f�t�H���g�o�̓f�o�C�X�j
	hr = xAudio2->CreateMasteringVoice(&masteringVoice);
	assert(SUCCEEDED(hr));

	// �}�X�^�[�{�C�X�̏ڍ׎擾�i�`�����l�������j
	XAUDIO2_VOICE_DETAILS voiceDetails = {};
	masteringVoice->GetVoiceDetails(&voiceDetails);

	// �o�̓`�����l�����i��: �X�e���I��2�j
	UINT32 channelCount = voiceDetails.InputChannels;

	// WASAPI�o�R�Ń`�����l���}�X�N�擾
	DWORD channelMask = 0;
	if (!GetOutputChannelMaskFromWASAPI(channelMask))
	{
		// ���s���̓X�e���I����
		channelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	}

	// X3DAudio�������B���x�萔�͉���[m/s]
	X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, x3dAudioHandle);

	// ���X�i�[��񏉊���
	ZeroMemory(&listener, sizeof(listener));
	listener.OrientFront = { 0.0f, 0.0f, 1.0f }; ///< ���ʂ�Z����������
	listener.OrientTop = { 0.0f, 1.0f, 0.0f };   ///< �������Y����������

	// �o�̓`�����l������ۑ�
	outputChannelCount = channelCount;

	// 3D�I�[�f�B�I�V�X�e���ɃG�~�b�^�[��ǉ�
	AddEmitter("Data/Sound/electrical_noise.wav", { 0.0f, 0.0f, 0.0f }, "electrical_noise", SoundType::BGM, 0.1f, true, true, true, 0.0f);


	AddEmitter("Data/Sound/atmosphere_noise.wav", { 0.0f, 0.0f, 0.0f }, "atmosphere_noise", SoundType::BGM, 0.5f, true);
	for (int i = 0; i < AIRCON_MAX; i++)
	{
		AddEmitter("Data/Sound/air_conditioner.wav", AirconManager::Instance().GetAirconPosition(i), "aircon", SoundType::SE, 0.2f, true, true, false, 0.1f);

	}

	//AddEmitter("Data/Sound/air_conditioner.wav", { -8.0f, 3.0f, 22.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { 8.0f, 3.0f, 22.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { -21.0f, 3.0f, 13.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { 0.0f, 3.0f, 13.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { 21.0f, 3.0f, 13.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { -21.0f, 3.0f, 5.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { 0.0f , 3.0f, 5.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { 21.0f, 3.0f, 5.0f }, "aircon",  SoundType::BGM,true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { -21.0f, 3.0f, -13.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { 21.0f, 3.0f, -13.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { -21.0f, 3.0f,-23.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { 5.0f, 3.0f,-23.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);
	//AddEmitter("Data/Sound/air_conditioner.wav", { 21.0f, 3.0f,-23.0f }, "aircon", SoundType::BGM, true, true, false, 0.1f);


	AddEmitter("Data/Sound/enemy_run.wav", { 21.0f, 3.0f,-23.0f }, "enemy_run", SoundType::SE, 1.f, true, true, false, 0.1f);
	AddEmitter("Data/Sound/enemy_walk.wav", { 21.0f, 3.0f,-23.0f }, "enemy_walk", SoundType::SE, 1.f, true, true, false, 0.1f);

	AddEmitter("Data/Sound/change_camera.wav", { 21.0f, 3.0f,-23.0f }, "change_camera", SoundType::SE, 0.1f, true, true, false, 1.f);
	AddEmitter("Data/Sound/lightoff.wav", { 21.0f, 3.0f,-23.0f }, "lightoff", SoundType::SE, 0.1f, true, true, false, 1.f);
	AddEmitter("Data/Sound/Lighton.wav", { 21.0f, 3.0f,-23.0f }, "Lighton", SoundType::SE, 0.1f, true, true, false, 1.f);
	AddEmitter("Data/Sound/selectButton.wav", { 21.0f, 3.0f,-23.0f }, "selectButton", SoundType::SE, 0.1f, true, true, false, 1.f);



}

/**
 * @brief �G�~�b�^�[�i�����j��ǉ�����
 *
 * �w�肳�ꂽWAV�t�@�C����ǂݍ��݁A3D��ԏ�̎w��ʒu�ɉ�����z�u����B
 * ���[�v�Đ��△�w�����A���������Ȃ��Ȃǂ̃I�v�V������ݒ�\�B
 * ������XAudio2��SourceVoice���쐬���A�o�b�t�@���ݒ肷��B
 *
 * @param wavPath WAV�t�@�C���̃p�X
 * @param pos 3D��ԏ�̉����ʒu
 * @param tag �G�~�b�^�[���ʗp�^�O
 * @param loop ���[�v�Đ��t���O�itrue�Ȃ烋�[�v�j
 * @param isOmnidirectional ���w���������t���O�itrue�Ȃ�����Ɉˑ����Ȃ������j
 * @param constantVolume �������������t���O�itrue�Ȃ��Ɉ�艹�ʁj
 */
void Audio3DSystem::AddEmitter(const char* wavPath, const XMFLOAT3& pos, const std::string& tag, SoundType soundType,
	float volume, bool loop, bool isOmnidirectional, bool constantVolume, float distanceScaler)
{

	Emitter e{};
	if (!LoadWavFile(wavPath, &e.waveFormat, &e.buffer, e.waveData)) {
		std::cerr << "WAV�ǂݍ��ݎ��s: " << wavPath << std::endl;
		return;
	}

	HRESULT hr = xAudio2->CreateSourceVoice(&e.sourceVoice, &e.waveFormat);
	assert(SUCCEEDED(hr)); ///< SourceVoice�������s�̓A�T�[�g

	e.position = pos;
	e.tag = tag;
	e.soundType = soundType;
	e.loop = loop;
	e.isOmnidirectional = isOmnidirectional;
	e.constantVolume = constantVolume;
	e.distanceScaler = distanceScaler;

	e.buffer.AudioBytes = static_cast<UINT32>(e.waveData.size());
	e.buffer.pAudioData = e.waveData.data();
	e.buffer.Flags = XAUDIO2_END_OF_STREAM;
	e.buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

	// �G�~�b�^�[�̉��������ݒ�
	e.emitterDesc.Position = pos;
	e.emitterDesc.ChannelCount = 1;
	e.emitterDesc.CurveDistanceScaler = 1.0f; // ���������̃X�P�[���B��ŏ㏑������邱�Ƃ�����
	e.emitterDesc.InnerRadius = 1.0f;
	e.emitterDesc.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;

	e.Volume = volume;
	emitters.push_back(std::move(e));

	SetVolumeByAll();
}

/**
 * @brief ���X�i�[�̈ʒu�E�������X�V����
 *
 * @param pos ���X�i�[��3D�ʒu
 * @param front ���X�i�[�̐��ʕ����x�N�g���i���K�������j
 * @param up ���X�i�[�̏�����x�N�g���i���K�������j
 */
void Audio3DSystem::UpdateListener(const XMFLOAT3& pos, const XMFLOAT3& front, const XMFLOAT3& up)
{
	std::lock_guard<std::mutex> lock(dataMutex); ///< �r������
	listener.Position = pos;
	listener.OrientFront = front;
	listener.OrientTop = up;
}

/**
 * @brief �e�G�~�b�^�[��3D�I�[�f�B�I�v�Z�Ɖ����p�����[�^�X�V���s��
 *
 * X3DAudioCalculate��p���ċ���������h�b�v���[���ʂ��v�Z���A
 * SourceVoice�̏o�̓}�g���N�X����g�����ݒ肵��3D��������������B
 */
void Audio3DSystem::UpdateEmitters()
{
	for (auto& e : emitters) {
		FLOAT32 matrix[XAUDIO2_MAX_AUDIO_CHANNELS * XAUDIO2_MAX_AUDIO_CHANNELS] = {};

		X3DAUDIO_DSP_SETTINGS dspSettings = {};
		dspSettings.SrcChannelCount = 1; // ���m��������
		dspSettings.DstChannelCount = outputChannelCount; // �ʏ��2�i�X�e���I�j
		dspSettings.pMatrixCoefficients = matrix;

		e.emitterDesc.Position = e.position;

		if (e.isOmnidirectional) {
			e.emitterDesc.OrientFront = { 0, 0, 0 };
			e.emitterDesc.OrientTop = { 0, 0, 0 };
		}
		else {
			e.emitterDesc.OrientFront = { 0, 0, 1 };
			e.emitterDesc.OrientTop = { 0, 1, 0 };
		}

		// ���������X�P�[���[�i0�ɂ���Ƌ����ɂ��ω��Ȃ��j
		e.emitterDesc.CurveDistanceScaler = e.constantVolume ? 0 : e.distanceScaler;

		X3DAudioCalculate(
			x3dAudioHandle,
			&listener,
			&e.emitterDesc,
			X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER,
			&dspSettings
		);

		e.sourceVoice->SetOutputMatrix(nullptr, dspSettings.SrcChannelCount, dspSettings.DstChannelCount, matrix);
		e.sourceVoice->SetFrequencyRatio(dspSettings.DopplerFactor);

		SetVolumeByAll();
	}
}

/**
 * @brief �w��^�O�̃G�~�b�^�[�̈ʒu��ύX����
 *
 * �o�^����Ă���S�G�~�b�^�[�̒�����^�O���������A�Y��������̂̈ʒu���X�V����B
 *
 * @param tag �ύX�ΏۃG�~�b�^�[�̃^�O������
 * @param newPos �V����3D��Ԉʒu
 */
void Audio3DSystem::SetEmitterPositionByTag(const std::string& tag, const XMFLOAT3& newPos)
{
	for (auto& e : emitters) {
		if (e.tag == tag) {
			e.position = newPos;
			e.emitterDesc.Position = newPos;
		}
	}
}

/**
 * @brief ���ׂẴG�~�b�^�[�̉������Đ��J�n����
 *
 * �e�G�~�b�^�[��XAUDIO2_BUFFER��SourceVoice�ɓo�^���A�Đ����J�n����B
 */
void Audio3DSystem::PlayAll()
{
	for (auto& e : emitters) {
		e.sourceVoice->FlushSourceBuffers(); // �o�b�t�@���Z�b�g
		e.sourceVoice->SubmitSourceBuffer(&e.buffer);
		e.sourceVoice->Start();
	}
}

/**
 * @brief �w��^�O�̃G�~�b�^�[�̂ݍĐ��J�n����
 *
 * ���ɍĐ����̏ꍇ�̓o�b�t�@�����Z�b�g���čĐ�����蒼���B
 *
 * @param tag �Đ��Ώۂ̃^�O������
 */
void Audio3DSystem::PlayByTag(const std::string& tag)
{
	for (auto& e : emitters) {
		if (e.tag == tag) {
			e.sourceVoice->FlushSourceBuffers(); // �o�b�t�@���Z�b�g
			e.sourceVoice->SubmitSourceBuffer(&e.buffer);
			e.sourceVoice->Start();
		}
	}
}

/**
 * @brief �w��^�O�̃G�~�b�^�[�̉������~����
 *
 * @param tag ��~�Ώۂ̃^�O������
 */
void Audio3DSystem::StopByTag(const std::string& tag)
{
	for (auto& e : emitters) {
		if (e.tag == tag) {
			e.sourceVoice->Stop();
		}
	}
}

/**
 * @brief �G�~�b�^�[��3D�����X�V�����p�X���b�h���J�n����
 *
 * 60FPS�����̎����i��16ms�j��UpdateEmitters���Ăяo���A
 * ���̉����v�Z�Ɖ����p�����[�^�̍X�V���s���B
 */
void Audio3DSystem::StartUpdateThread()
{
	running = true;
	updateThread = std::thread([this]() {
		while (running) {
			{
				std::lock_guard<std::mutex> lock(dataMutex); ///< �r������
				UpdateEmitters();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ��60FPS�X�V
		}
		});
}

/**
 * @brief �G�~�b�^�[�X�V�X���b�h���~���A�I����ҋ@����
 */
void Audio3DSystem::StopUpdateThread()
{
	running = false;
	if (updateThread.joinable()) {
		updateThread.join();
	}
}
/**
 * @brief �w��^�O�̃G�~�b�^�[�̂݉��ʂ�ύX����
 *
 * @param tag ���ʕύX�Ώۂ̃^�O������
 */
void Audio3DSystem::SetVolumeByTag(const std::string& tag, float volume)
{
	std::lock_guard<std::mutex> lock(dataMutex); ///< �r������

	for (auto& e : emitters) {
		if (e.tag == tag) {
			//if (e.sourceVoice) {
			//	//switch (e.soundType)
			//	//{
			//	//case SoundType::SE:
			//	//	//e.sourceVoice->SetVolume(volume * masterVolume * seVolume);
			//	//	break;
			//	//case SoundType::BGM:
			//	//	//e.sourceVoice->SetVolume(volume * masterVolume * bgmVolume);
			//	//	break;
			//	//}
			//}
			e.Volume = volume;
		}
	}
}
void Audio3DSystem::SetVolumeByAll()
{
	std::lock_guard<std::mutex> lock(dataMutex); ///< �r������

	for (auto& e : emitters) {
		if (e.sourceVoice) {
			switch (e.soundType)
			{
			case SoundType::SE:
				e.sourceVoice->SetVolume(e.Volume * masterVolume * seVolume);
				break;
			case SoundType::BGM:
				e.sourceVoice->SetVolume(e.Volume * masterVolume * bgmVolume);
				break;
			}
		}
	}
}