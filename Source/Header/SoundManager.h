#pragma once
// XAudio2関連
#pragma comment(lib, "xaudio2.lib")
#include<xaudio2.h>

// マルチメディア関連（waveファイルの読み込み用）
#pragma comment ( lib, "winmm.lib" )
#include<mmsystem.h>

#define MAX_WAV 100 //WAVサウンド最大数

/// <summary>
/// 音声の種類
/// </summary>
enum class SOUND_TYPE
{
    SE,     
    BGM,
    VOICE,
};

/// <summary>
/// 音声の再生用のID
/// </summary>
enum class SOUND_ID
{
    NONE = -1,

    // BGM
    BGM_TITLE_01,

    // SE
    /* 銃声 */
    GUN_FIRE01,            
    GUN_FIRE02,

    /* 兵士 */
    SOLDIER_R_JUMP_IN,       // ジャンプ - 開始
    SOLDIER_R_JUMP_LAND,     // ジャンプ - 着地


    /* 敵 */
    ENEMY_ANT_HIT01,        // アリ被弾 
    ENEMY_ANT_DEAD,         // アリやられる

    /* システム */
    SYSTEM_MOVING_CURSOR01, // 項目にカーソルがホバーしている状態

    NUM
};

/// <summary>
/// 声の再生用ID
/// </summary>
enum class VOICE_ID
{
    /* 掛け声的な奴 */
    SOLDIER_R_SHOUT_01,
    SOLDIER_R_SHOUT_02,
    SOLDIER_R_SHOUT_03,

    /* 被弾 */
    SOLDIER_R_HIT_01,
    SOLDIER_R_HIT_02,
    SOLDIER_R_HIT_03,
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:SoundManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】音声管理
// 参考サイト：https://zenn.dev/kd_gamegikenblg/articles/4f955822601484
// ***************************************************************************************
class SoundManager
{
private:

    IXAudio2* m_pXAudio2;
    IXAudio2MasteringVoice* m_pMasteringVoice;      // 最終的な音路出力する

    /// <summary>
    /// wavデータ
    /// </summary>
    struct WaveResource
    {
        WAVEFORMATEX _waveFormat;   // waveファイルのフォーマット
        BYTE* _waveBuffer;          // 波形データ
        DWORD _size;                // 波形データのサイズ

        WaveResource() :
            _waveFormat(),
            _waveBuffer(nullptr),
            _size(0)
        {
        };

        // 無いとリークする
        ~WaveResource() { delete[] _waveBuffer; }
    };

    /// <summary>
    /// サウンドデータ
    /// </summary>
    struct SoundInstance
    {
        IXAudio2SourceVoice *_pSourceVoice;// 音の起点
        bool _isUse;    // 使用中か
        SoundInstance() :
            _pSourceVoice(nullptr),
            _isUse(false)
        {};
        ~SoundInstance() {};
    };

    std::unordered_map<SOUND_ID, WaveResource> m_SE_WaveResourceMap;    // SE用
    std::unordered_map<VOICE_ID, WaveResource> m_Voice_WaveResourceMap; // ボイス用
    std::vector<SoundInstance> m_SoundSlotArray;         // 主にSE用のソースボイス
    std::vector<SoundInstance> m_VoiceSoundSlotArray;    // ボイス用のソースボイス
    SoundInstance m_BGM_SoundSlot;                       // BGM用 基本一つなので

    // 音量調整用
    float m_SEVolume;
    float m_BGMVolume;
    float m_VoiceVolume;
public:
    SoundManager();
    ~SoundManager();

    bool InitXA2Sound(void);
    void UninitXA2Sound(void);
    bool LoadSE_Wav(const char *filename, SOUND_ID _id);
    bool LoadVoice_Wav(const char* filename, VOICE_ID _id);
    bool PlaySE(SOUND_ID _id, bool _loop = false);
    bool PlaySE_RandPitch(SOUND_ID _id, int _pitchRange, bool _loop = false);
    bool PlayVoice(VOICE_ID _id);
    bool PlayVoice_Rand(VOICE_ID _beginID, int _range);
    bool PlayBGM(SOUND_ID _id, bool _loop = true);
    bool StopSound(SOUND_TYPE _type, SOUND_ID _id);

private:
    // コピー禁止
    SoundManager(const SoundManager &) = delete;
    SoundManager &operator=(const SoundManager &) = delete;


    bool Internal_Load_Wav(const char* _filename, WaveResource& _out, int _channelNum = 2, int _sampleLate = 44100, int _bitsPersample = 16);

};

