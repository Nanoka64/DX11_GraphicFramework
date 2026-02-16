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


// サウンドIDをintに変換するマクロ
#define SOUND_ID_TO_INT(_id) (static_cast<int>(_id))

/// <summary>
/// 音声の種類
/// </summary>
enum class SOUND_TYPE : char
{
    SE = 0,     
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


    NUM
};

/// <summary>
/// BGMの再生用ID
/// </summary>
enum class BGM_ID
{
    NONE = -1,

    // BGM
    BGM_TITLE_01,


    NUM
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
/// 再生に必要なデータ
/// </summary>
struct SoundInstance
{
    IXAudio2SourceVoice* _pSourceVoice; // 音の起点
    VECTOR3::VEC3 _pos;                 // 3Dサウンド用の位置情報
    bool _is3D;                         // 3Dサウンドかどうか
    bool _isUsed;                       // 再生中かどうか
    bool _isLoop;                       // ループ再生するかどうか
    SOUND_TYPE _soundType;              // 音声の種類（SE、BGM、ボイス）
    float _volumeFactor;                // 音量調整用の係数（0.0f～1.0f 1.0fなら元の音量）
    int _soundID;                       // どの音声データを再生しているかのID

    SoundInstance() :
        _pSourceVoice(nullptr),
        _pos(VECTOR3::VEC3()),
        _is3D(false),
        _isUsed(false),
        _isLoop(false),
        _soundID(-1),
        _soundType(SOUND_TYPE::SE),
        _volumeFactor(0.0)
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


    // リスナー（カメラ）
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
    SoundInstance m_BGM_SoundSlot;                       // BGM用 基本一つだけ再生するので

    std::vector<X3DAUDIO_EMITTER> m_3DAudioEmitterArray;    // 3Dサウンド用のエミッター配列
    
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
    
    // 更新
    bool Update(RendererEngine& renderer);

    //
    // ロード関数
    //  
    bool Load_Wav(const char *filename,SOUND_TYPE _type, int _id);            // SE用のWavファイル読み込み
    

    //
    // 再生関数
    //
    bool Play(SOUND_TYPE _type, int _id, bool _loop = false);                               // 再生
    bool Play_3D(SOUND_TYPE _type, int _id, const VECTOR3::VEC3 &_pos, bool _loop = false); // 再生（3D空間で鳴らす）
    bool Play_RandPitch(SOUND_TYPE _type, int _id, int _pitchRange, bool _loop = false);    // 再生（ピッチをランダムに揺らす）
    bool Play_Rand(SOUND_TYPE _type, int _beginId, int _range, bool _loop = false);         // 指定範囲のサウンドからランダムに再生 _beginIDから音声データが連続している必要あり
    
    bool PlayBGM(BGM_ID _id, bool _loop = true); // BGMの再生
    
    //
    // 停止関数
    //
    bool Stop(SOUND_TYPE _type, int _id);     // 停止（今はBGMのみ）

private:
    // コピー禁止
    SoundManager(const SoundManager &) = delete;
    SoundManager &operator=(const SoundManager &) = delete;


    // LoadSE_Wavなどが外から明示的に呼ぶ関数であるのに対し、
    // こっちは内部的な読み込みを行っている。
    bool Internal_Load_Wav(const char* _filename, WaveResource& _out, int _channelNum = 2, int _sampleLate = 44100, int _bitsPersample = 16);
    bool Internal_SoundPlay(SOUND_TYPE _type, int _id, bool _loop = false);
    bool Internal_SoundPlay_RandPitch(SOUND_TYPE _type, int _id, int _pitchRange, bool _loop = false);
    bool Internal_SoundPlay_Rand(SOUND_TYPE _type, int _beginId, int _range, bool _loop = false);
    bool Internal_SoundPlay_3D(SOUND_TYPE _type, int _id, const VECTOR3::VEC3& _pos, bool _loop = false);

    bool BaseSoundPlay(SOUND_TYPE _type, int _id, const XAUDIO2_BUFFER &_buff, bool _is3D, const VECTOR3::VEC3 &_pos, int _pitch, bool _loop);

    bool SoundTypeAndIDConvertToResource(SOUND_TYPE _type, int _id, WaveResource *&_outResource, std::vector<SoundInstance>*& _outInstArray);
};

