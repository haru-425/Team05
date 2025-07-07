#pragma once

struct GameSettings
{
    float sensitivity   = 1.00f; // 感度
    float masterVolume  = 1.00f; // マスター音量
    float bgmVolume     = 1.00f; // BGM音量
    float seVolume      = 1.00f; // SE音量
};

/**
 * @brief ゲームの設定情報を保持・管理するシングルトンマネージャ
 *
 * GameSettings 構造体の取得・保存・読み込み・リセットなどの処理を提供します
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
    * @brief 設定情報を取得
    * @date 2025/07/04
    */
    const GameSettings& GetGameSettings() const { return settings; }

    /**
    * @brief 設定を変更する
    * @date 2025/07/04
    */
    void SetGameSettings(const GameSettings& gameSettings) { settings = gameSettings; }

    /**
    * @brief ファイルから設定を書き出します
    * 
    * 保存先に問題がある場合、処理をしません
    * 
    * @date 2025/07/04
    */
    void Save();

    /**
    * @brief ファイルから設定を読み込みます
    * 
    * 読み込み元に問題がある場合、処理をしません
    * 
    * @date 2025/07/04
    */
    void Load();

    /**
    * @brief 設定を初期化します
    * 
    * GameSettings 構造体のデフォルトコンストラクタを使用し、
    * 全項目を初期状態に戻す
    * 
    * @date 2025/07/04
    */
    void ResetToDefault() {
        settings = GameSettings();
    }

private:
    GameSettings settings; // 現在の設定を保存するメンバ変数
};