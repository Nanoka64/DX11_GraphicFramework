#pragma once
#include <mmsystem.h>
#include <xaudio2.h>

#define MAX_WAV 100 //WAVサウンド最大数

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:SoundManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】音声管理
// 
// ***************************************************************************************
class SoundManager
{
private:

    IXAudio2 *m_pXAudio2;
    IXAudio2MasteringVoice *m_pMasteringVoice;
    IXAudio2SourceVoice *m_pSourceVoice[MAX_WAV];
    BYTE *m_WavBuffer[MAX_WAV];     //波形データ
    DWORD m_WavSize[MAX_WAV];       //波形データのサイズ
    int m_SoundIndex;               //読み込む音のインデックス


public:
    SoundManager();
    ~SoundManager();

    std::unordered_map<std::string, int> m_SoundMap;
    bool InitXA2Sound(void);
    void UninitXA2Sound(void);
    int LoadXA2Sound(char *filename);
    void PlayXA2Sound(int index);
    void StopXA2Sound(int index);

private:
    // コピー禁止
    SoundManager(const SoundManager &) = delete;
    SoundManager &operator=(const SoundManager &) = delete;
};

