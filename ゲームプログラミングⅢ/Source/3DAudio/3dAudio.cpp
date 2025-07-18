#include "3dAudio.h"
#include <cassert>
#include <combaseapi.h>
#include <iostream>
#include <mmdeviceapi.h>         // IMMDevice, IMMDeviceEnumerator
#include <audioclient.h>         // IAudioClient, GetMixFormat
#include <functiondiscoverykeys_devpkey.h> // PKEY_Device_... など（使用していなければ不要）
#include <combaseapi.h>          // CoCreateInstance など（CoInitialize含め）
#include <objbase.h>             // CoInitializeEx（呼び出し元用）

#include <wrl/client.h>          // Microsoft::WRL::ComPtr を使う場合
#include "GameObjects/Aircon/AirconManager.h"
using Microsoft::WRL::ComPtr;
/// @brief デフォルトのレンダーデバイス（出力）のチャンネルマスクを取得する
///
/// WindowsのWASAPIを使用して、現在選択されているデフォルトの出力デバイス（スピーカーなど）が
/// 対応しているスピーカー構成（例：ステレオ、5.1chなど）に基づくチャンネルマスクを取得します。
///
/// このマスクは、X3DAudioやXAudio2でのDSP設定（`X3DAUDIO_DSP_SETTINGS::DstChannelCount`など）に利用できます。
///
/// @param[out] outMask チャンネルマスクが格納されるDWORD（例：`SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT`）
/// @retval true チャンネルマスクの取得に成功
/// @retval false エラー（デバイス取得やAudioClient初期化失敗など）
///
/// @note COMの初期化（`CoInitializeEx`）は呼び出し元で行っておく必要があります。
/// @see https://learn.microsoft.com/en-us/windows/win32/coreaudio/core-audio-apis-in-windows
bool GetOutputChannelMaskFromWASAPI(DWORD& outMask)
{
	HRESULT hr;

	// IMMDeviceEnumerator を作成（出力デバイスの列挙）
	ComPtr<IMMDeviceEnumerator> deviceEnumerator;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
		CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr)) return false;

	// デフォルトのレンダー（出力）デバイスを取得（eConsoleは一般用途）
	ComPtr<IMMDevice> defaultDevice;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	if (FAILED(hr)) return false;

	// IAudioClient を取得（デバイスのフォーマット情報にアクセス）
	ComPtr<IAudioClient> audioClient;
	hr = defaultDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &audioClient);
	if (FAILED(hr)) return false;

	// 現在のミックスフォーマットを取得（WAVEFORMATEXTENSIBLEかどうかを判定）
	WAVEFORMATEX* waveFormat = nullptr;
	hr = audioClient->GetMixFormat(&waveFormat);
	if (FAILED(hr) || waveFormat == nullptr) return false;

	// WAVEFORMATEXTENSIBLE 構造体か確認し、チャンネルマスクを取得
	if (waveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		auto fmtEx = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(waveFormat);
		outMask = fmtEx->dwChannelMask;
	}
	else
	{
		// 非拡張フォーマットの場合（例：ステレオなど）、デフォルトで前方左右とみなす
		outMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	}

	// フォーマット構造体の解放（WASAPIの仕様に従う）
	CoTaskMemFree(waveFormat);
	return true;
}
/**
 * @brief WAVファイルを読み込み、フォーマットとバッファをセットアップする
 *
 * ファイルからRIFF形式のWAVを読み込み、WAVEFORMATEX構造体とXAUDIO2_BUFFER構造体に
 * 音声情報を設定する。RIFFチャンクやfmtチャンク、dataチャンクの読み込みを行い、
 * バイナリデータを正しく格納する。
 *
 * @param filename 読み込むWAVファイルパス
 * @param outFormat WAVのフォーマット情報を格納するポインタ
 * @param outBuffer XAUDIO2_BUFFER構造体のポインタ（再生バッファ情報）
 * @param waveData 音声データバイト配列（読み込んだ音声を格納する）
 * @return true 成功、false 失敗（ファイルが不正、フォーマット不一致など）
 *
 * @see https://learn.microsoft.com/ja-jp/windows/win32/api/mmreg/ns-mmreg-waveformatex
 */
bool Audio3DSystem::LoadWavFile(const char* filename, WAVEFORMATEX* outFormat, XAUDIO2_BUFFER* outBuffer, std::vector<BYTE>& waveData) {

	std::ifstream file(filename, std::ios::binary);
	if (!file) return false; ///< ファイルオープン失敗時はfalseを返す

	DWORD chunkType;
	DWORD chunkDataSize;
	DWORD fileType;
	DWORD fmtSize;

	// ===== RIFFチャンクヘッダの読み込み =====
	file.read(reinterpret_cast<char*>(&chunkType), sizeof(DWORD));
	// RIFFチャンクタイプはリトルエンディアンで 'RIFF' が 'FFIR' として読み込まれる
	if (chunkType != 'FFIR') return false; ///< RIFFチャンクタイプ不一致で読み込み失敗

	file.read(reinterpret_cast<char*>(&chunkDataSize), sizeof(DWORD)); // チャンク全体サイズ
	file.read(reinterpret_cast<char*>(&fileType), sizeof(DWORD));      // WAVEタイプ
	if (fileType != 'EVAW') return false; ///< WAVEタイプ不一致で読み込み失敗

	// ===== fmtチャンクの読み込み =====
	file.read(reinterpret_cast<char*>(&chunkType), sizeof(DWORD));
	if (chunkType != ' tmf') return false; ///< fmtチャンクタイプ不一致

	file.read(reinterpret_cast<char*>(&fmtSize), sizeof(DWORD));
	std::vector<BYTE> fmtChunk(fmtSize);
	file.read(reinterpret_cast<char*>(fmtChunk.data()), fmtSize);
	memcpy(outFormat, fmtChunk.data(), fmtSize); ///< フォーマット情報をコピー

	// ===== dataチャンクの検索と読み込み =====
	while (file.read(reinterpret_cast<char*>(&chunkType), sizeof(DWORD))) {
		file.read(reinterpret_cast<char*>(&chunkDataSize), sizeof(DWORD));
		if (chunkType == 'atad') { ///< dataチャンク発見
			waveData.resize(chunkDataSize);
			file.read(reinterpret_cast<char*>(waveData.data()), chunkDataSize);
			break;
		}
		else {
			// 不明なチャンクはスキップ
			file.seekg(chunkDataSize, std::ios::cur);
		}
	}

	// XAUDIO2_BUFFER構造体を初期化し、データをセット
	ZeroMemory(outBuffer, sizeof(XAUDIO2_BUFFER));
	outBuffer->AudioBytes = static_cast<UINT32>(waveData.size()); ///< 音声データバイト数
	outBuffer->pAudioData = waveData.data();                       ///< 音声データポインタ
	outBuffer->Flags = XAUDIO2_END_OF_STREAM;                      ///< ストリーム終了フラグ
	return true;
}

/**
 * @brief XAudio2およびX3DAudioの初期化を行う
 *
 * COMのマルチスレッドモデル初期化、XAudio2インスタンス生成、
 * マスターボイス作成、出力チャンネル数取得、X3DAudioの初期化を行う。
 * リスナーの初期向きもここで設定する。
 */
void Audio3DSystem::Initialize()
{
	// COM初期化（マルチスレッドモデル）
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// XAudio2インスタンス生成
	HRESULT hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr)); ///< 失敗時はアサート

	// マスタリングボイス生成（デフォルト出力デバイス）
	hr = xAudio2->CreateMasteringVoice(&masteringVoice);
	assert(SUCCEEDED(hr));

	// マスターボイスの詳細取得（チャンネル数等）
	XAUDIO2_VOICE_DETAILS voiceDetails = {};
	masteringVoice->GetVoiceDetails(&voiceDetails);

	// 出力チャンネル数（例: ステレオは2）
	UINT32 channelCount = voiceDetails.InputChannels;

	// WASAPI経由でチャンネルマスク取得
	DWORD channelMask = 0;
	if (!GetOutputChannelMaskFromWASAPI(channelMask))
	{
		// 失敗時はステレオ仮定
		channelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	}

	// X3DAudio初期化。速度定数は音速[m/s]
	X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, x3dAudioHandle);

	// リスナー情報初期化
	ZeroMemory(&listener, sizeof(listener));
	listener.OrientFront = { 0.0f, 0.0f, 1.0f }; ///< 正面をZ軸正方向に
	listener.OrientTop = { 0.0f, 1.0f, 0.0f };   ///< 上方向をY軸正方向に

	// 出力チャンネル数を保存
	outputChannelCount = channelCount;

	// 3Dオーディオシステムにエミッターを追加
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
 * @brief エミッター（音源）を追加する
 *
 * 指定されたWAVファイルを読み込み、3D空間上の指定位置に音源を配置する。
 * ループ再生や無指向性、距離減衰なしなどのオプションを設定可能。
 * 内部でXAudio2のSourceVoiceを作成し、バッファも設定する。
 *
 * @param wavPath WAVファイルのパス
 * @param pos 3D空間上の音源位置
 * @param tag エミッター識別用タグ
 * @param loop ループ再生フラグ（trueならループ）
 * @param isOmnidirectional 無指向性音源フラグ（trueなら方向に依存しない音響）
 * @param constantVolume 距離減衰無効フラグ（trueなら常に一定音量）
 */
void Audio3DSystem::AddEmitter(const char* wavPath, const XMFLOAT3& pos, const std::string& tag, SoundType soundType,
	float volume, bool loop, bool isOmnidirectional, bool constantVolume, float distanceScaler)
{

	Emitter e{};
	if (!LoadWavFile(wavPath, &e.waveFormat, &e.buffer, e.waveData)) {
		std::cerr << "WAV読み込み失敗: " << wavPath << std::endl;
		return;
	}

	HRESULT hr = xAudio2->CreateSourceVoice(&e.sourceVoice, &e.waveFormat);
	assert(SUCCEEDED(hr)); ///< SourceVoice生成失敗はアサート

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

	// エミッターの音響特性設定
	e.emitterDesc.Position = pos;
	e.emitterDesc.ChannelCount = 1;
	e.emitterDesc.CurveDistanceScaler = 1.0f; // 距離減衰のスケール。後で上書きされることもある
	e.emitterDesc.InnerRadius = 1.0f;
	e.emitterDesc.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;

	e.Volume = volume;
	emitters.push_back(std::move(e));

	SetVolumeByAll();
}

/**
 * @brief リスナーの位置・向きを更新する
 *
 * @param pos リスナーの3D位置
 * @param front リスナーの正面方向ベクトル（正規化推奨）
 * @param up リスナーの上方向ベクトル（正規化推奨）
 */
void Audio3DSystem::UpdateListener(const XMFLOAT3& pos, const XMFLOAT3& front, const XMFLOAT3& up)
{
	std::lock_guard<std::mutex> lock(dataMutex); ///< 排他制御
	listener.Position = pos;
	listener.OrientFront = front;
	listener.OrientTop = up;
}

/**
 * @brief 各エミッターの3Dオーディオ計算と音声パラメータ更新を行う
 *
 * X3DAudioCalculateを用いて距離減衰やドップラー効果を計算し、
 * SourceVoiceの出力マトリクスや周波数比を設定して3D音響を実現する。
 */
void Audio3DSystem::UpdateEmitters()
{
	for (auto& e : emitters) {
		FLOAT32 matrix[XAUDIO2_MAX_AUDIO_CHANNELS * XAUDIO2_MAX_AUDIO_CHANNELS] = {};

		X3DAUDIO_DSP_SETTINGS dspSettings = {};
		dspSettings.SrcChannelCount = 1; // モノラル音源
		dspSettings.DstChannelCount = outputChannelCount; // 通常は2（ステレオ）
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

		// 距離減衰スケーラー（0にすると距離による変化なし）
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
 * @brief 指定タグのエミッターの位置を変更する
 *
 * 登録されている全エミッターの中からタグを検索し、該当するものの位置を更新する。
 *
 * @param tag 変更対象エミッターのタグ文字列
 * @param newPos 新しい3D空間位置
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
 * @brief すべてのエミッターの音声を再生開始する
 *
 * 各エミッターのXAUDIO2_BUFFERをSourceVoiceに登録し、再生を開始する。
 */
void Audio3DSystem::PlayAll()
{
	for (auto& e : emitters) {
		e.sourceVoice->FlushSourceBuffers(); // バッファリセット
		e.sourceVoice->SubmitSourceBuffer(&e.buffer);
		e.sourceVoice->Start();
	}
}

/**
 * @brief 指定タグのエミッターのみ再生開始する
 *
 * 既に再生中の場合はバッファをリセットして再生をやり直す。
 *
 * @param tag 再生対象のタグ文字列
 */
void Audio3DSystem::PlayByTag(const std::string& tag)
{
	for (auto& e : emitters) {
		if (e.tag == tag) {
			e.sourceVoice->FlushSourceBuffers(); // バッファリセット
			e.sourceVoice->SubmitSourceBuffer(&e.buffer);
			e.sourceVoice->Start();
		}
	}
}

/**
 * @brief 指定タグのエミッターの音声を停止する
 *
 * @param tag 停止対象のタグ文字列
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
 * @brief エミッターの3D音響更新処理用スレッドを開始する
 *
 * 60FPS相当の周期（約16ms）でUpdateEmittersを呼び出し、
 * 立体音響計算と音声パラメータの更新を行う。
 */
void Audio3DSystem::StartUpdateThread()
{
	running = true;
	updateThread = std::thread([this]() {
		while (running) {
			{
				std::lock_guard<std::mutex> lock(dataMutex); ///< 排他制御
				UpdateEmitters();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 約60FPS更新
		}
		});
}

/**
 * @brief エミッター更新スレッドを停止し、終了を待機する
 */
void Audio3DSystem::StopUpdateThread()
{
	running = false;
	if (updateThread.joinable()) {
		updateThread.join();
	}
}
/**
 * @brief 指定タグのエミッターのみ音量を変更する
 *
 * @param tag 音量変更対象のタグ文字列
 */
void Audio3DSystem::SetVolumeByTag(const std::string& tag, float volume)
{
	std::lock_guard<std::mutex> lock(dataMutex); ///< 排他制御

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
	std::lock_guard<std::mutex> lock(dataMutex); ///< 排他制御

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