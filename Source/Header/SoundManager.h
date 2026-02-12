#pragma once
// XAudio2関連
#pragma comment(lib, "xaudio2.lib")
#include<xaudio2.h>

// マルチメディア関連（waveファイルの読み込み用）
#pragma comment ( lib, "winmm.lib" )
#include<mmsystem.h>

#define MAX_WAV 100 //WAVサウンド最大数

/// <summary>
/// 音声の再生用のID
/// </summary>
enum class SOUND_ID
{
    // BGM

    // SE
    TEST,


    NUM
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
    //IXAudio2SourceVoice *m_pSourceVoice[MAX_WAV];   // 音の起点（音を鳴らすたびに作成する必要がある）
    // BYTE *m_WavBuffer[MAX_WAV];     //波形データ
    // DWORD m_WavSize[MAX_WAV];       //波形データのサイズ
    // int m_SoundIndex;               //読み込む音のインデックス

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
        SoundInstance() {};
        ~SoundInstance() {};
    };

    std::unordered_map<SOUND_ID, WaveResource> m_WaveResourceMap; 
    std::vector<SoundInstance> m_SoundInstanceArray;    // 

public:
    SoundManager();
    ~SoundManager();

    std::unordered_map<std::string, int> m_SoundMap;
    bool InitXA2Sound(void);
    void UninitXA2Sound(void);
    bool LoadXA2Sound(char *filename, SOUND_ID id);
    void PlayXA2Sound(SOUND_ID _id);
    void StopXA2Sound(SOUND_ID _id);

private:
    // コピー禁止
    SoundManager(const SoundManager &) = delete;
    SoundManager &operator=(const SoundManager &) = delete;
};

