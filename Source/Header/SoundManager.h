#pragma once
// XAudio2関連
#pragma comment(lib, "xaudio2.lib")
#include<xaudio2.h>

// 3Dサウンド用
#include<x3daudio.h>

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
    NONE = -1,

    /* 掛け声的なやつ */
    SOLDIER_R_SHOUT_01,
    SOLDIER_R_SHOUT_02,
    SOLDIER_R_SHOUT_03,

    /* 被弾 */
    SOLDIER_R_HIT_01,
    SOLDIER_R_HIT_02,
    SOLDIER_R_HIT_03,
};

/// <summary>
/// BGMの再生用ID
/// </summary>
enum class BGM_ID
{
    NONE = -1,

    // BGM
    BGM_TITLE_01,
};


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
    IXAudio2SourceVoice* _pSourceVoice;// 音の起点
    bool _isUse;    // 使用中か
    SoundInstance() :
        _pSourceVoice(nullptr),
        _isUse(false)
    {
    };
    ~SoundInstance() {};
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:SoundManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】音声管理
//      Wavファイルの読み込み、再生、停止などの役割を持つ
//      
// 参考サイト：https://zenn.dev/kd_gamegikenblg/articles/4f955822601484
// 
// X3DAudio：https://learn.microsoft.com/ja-jp/windows/win32/xaudio2/x3daudio-overview
//           https://rastertek.com/dx11win10tut58.html
// ***************************************************************************************
class SoundManager
{
private:
    // 本体
    IXAudio2* m_pXAudio2;


    // 最終的な音路出力する
    IXAudio2MasteringVoice* m_pMasteringVoice; 


    // リスナー
    X3DAUDIO_LISTENER m_Listener;              


    // 音声の詳細情報
    XAUDIO2_VOICE_DETAILS m_DeviceDetails;     


    // X3DAudioCalculateからの処理結果を受け取るためのもの！
    // DSP:音の加工に使うもの（？）
    X3DAUDIO_DSP_SETTINGS m_DSPSettings;

    // X3Dインスタンスハンドル
    X3DAUDIO_HANDLE m_X3DInstanceHandle;


    std::unordered_map<SOUND_ID, WaveResource> m_SE_WaveResourceMap;    // SE用
    std::unordered_map<VOICE_ID, WaveResource> m_Voice_WaveResourceMap; // ボイス用
    std::unordered_map<BGM_ID, WaveResource> m_BGM_WaveResourceMap;     // BGM用
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

    // 初期化
    bool InitXA2Sound(void);        

    // 解放
    void UninitXA2Sound(void);
    
    //
    // ロード関数
    //  
    bool LoadSE_Wav(const char *filename, SOUND_ID _id);            // SE用のWavファイル読み込み
    bool LoadBGM_Wav(const char *filename, BGM_ID _id);             // BGM用のWavファイル読み込み
    bool LoadVoice_Wav(const char* filename, VOICE_ID _id);         // ボイス用のWavファイル読み込み
    

    //
    // 再生関数
    //
    bool SoundPlay(SOUND_ID _id, bool _loop = false);
    bool SoundPlay(VOICE_ID _id, bool _loop = false);
    bool SoundPlay(BGM_ID _id, bool _loop = false);


    bool PlaySE(SOUND_ID _id, bool _loop = false);                              // SEの再生
    bool PlaySE_RandPitch(SOUND_ID _id, int _pitchRange, bool _loop = false);   // SEの再生（ピッチをランダムに揺らす）
    bool PlayVoice(VOICE_ID _id);                                               // ボイスの再生
    bool PlayVoice_Rand(VOICE_ID _beginID, int _range);                         // ボイスの再生（指定範囲のボイスからランダムに再生 _beginIDから音声データが連続している必要あり）
    bool PlayBGM(BGM_ID _id, bool _loop = true);                              // BGMの再生
    bool PlaySE_3D(SOUND_ID _id, const VECTOR3::VEC3& _pos, bool _loop = false);
    
    //
    // 停止関数
    //
    bool StopSound(SOUND_TYPE _type, SOUND_ID _id);     // 停止（今はBGMのみ）

private:
    // コピー禁止
    SoundManager(const SoundManager &) = delete;
    SoundManager &operator=(const SoundManager &) = delete;


    // LoadSE_Wavなどが外から明示的に呼ぶ関数であるのに対し、
    // こっちは内部的な読み込みを行っている。
    bool Internal_Load_Wav(const char* _filename, WaveResource& _out, int _channelNum = 2, int _sampleLate = 44100, int _bitsPersample = 16);
    bool Internal_SoundPlay(SOUND_TYPE _type, int _id);
};

