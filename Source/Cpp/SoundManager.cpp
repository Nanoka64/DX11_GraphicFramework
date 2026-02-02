#include "pch.h"
#include "SoundManager.h"
#include <windows.h>

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
SoundManager::SoundManager():
	m_pXAudio2(nullptr),
	m_pMasteringVoice(nullptr),
	m_pSourceVoice(nullptr),
	m_WavBuffer(nullptr),
	m_SoundIndex(0)
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
SoundManager::~SoundManager()
{

}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数] なし
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::InitXA2Sound(void)
{
	//COMインターフェースの初期化
	//CoInitializeEx(NULL, COINIT_MULTITHREADED);

	//XAudio2デバイス作成
	if (XAudio2Create(&m_pXAudio2, XAUDIO2_DEBUG_ENGINE) != S_OK)
	{
		//CoUninitialize();
		return false;
	}

	//マスターボイスの作成
	if (m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice) != S_OK)
	{
		//CoUninitialize();
		return false;
	}

	m_SoundIndex = 0;

	return true;
}

//*---------------------------------------------------------------------------------------
//*【?】解放
//*
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void SoundManager::UninitXA2Sound(void)
{
	if (m_pMasteringVoice != nullptr)
	{
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = nullptr;
	}

	if (m_pXAudio2 != nullptr)
	{
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}

	//CoUninitialize();
}

//*---------------------------------------------------------------------------------------
//*【?】サウンドファイルの読み込み
//*
//* [引数]
//* *filename：読み込むファイル名 
//* [返値]
//* ハンドル 
//*----------------------------------------------------------------------------------------
int SoundManager::LoadXA2Sound(char *filename)
{
	HMMIO hMMIO = NULL;
	DWORD size;
	WAVEFORMATEX *pwfex;
	MMCKINFO ckInfo = {};
	MMCKINFO rifckInfo = {};
	PCMWAVEFORMAT pcmFormat;
	DWORD index = m_SoundIndex;

	m_SoundIndex++;

	//ファイルを開く
	hMMIO = mmioOpen(filename, NULL, MMIO_ALLOCBUF | MMIO_READ);

	//fmtチャンク読み込み
	mmioDescend(hMMIO, &rifckInfo, NULL, 0);
	ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmioDescend(hMMIO, &ckInfo, &rifckInfo, MMIO_FINDCHUNK);
	mmioRead(hMMIO, (HPSTR)&pcmFormat, sizeof(pcmFormat));
	pwfex = (WAVEFORMATEX *)new BYTE[sizeof(WAVEFORMATEX)];
	memcpy(pwfex, &pcmFormat, sizeof(pcmFormat));
	pwfex->cbSize = 0;
	mmioAscend(hMMIO, &ckInfo, 0);

	//dataチャンク読み込み
	ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmioDescend(hMMIO, &ckInfo, &rifckInfo, MMIO_FINDCHUNK);
	size = ckInfo.cksize;
	m_WavBuffer[index] = new BYTE[size];
	mmioRead(hMMIO, (HPSTR)m_WavBuffer[index], size);

	//ソースボイス作成
	if (m_pXAudio2->CreateSourceVoice(&m_pSourceVoice[index], pwfex) != S_OK)
	{
		return -1;
	}

	m_WavSize[index] = size;

	//読み込んだインデックスを返す
	return index;
}

//*---------------------------------------------------------------------------------------
//*【?】サウンドの再生
//*
//* [引数]
//* index：再生するサウンドのハンドル
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
void SoundManager::PlayXA2Sound(int index)
{
	if (index < 0 || index >= m_SoundIndex)
		return;

	//再生中のサウンドを止める
	m_pSourceVoice[index]->Stop(0, 0);
	m_pSourceVoice[index]->FlushSourceBuffers();

	//再生
	XAUDIO2_BUFFER buff = {};
	buff.pAudioData = m_WavBuffer[index];
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.AudioBytes = m_WavSize[index];
	if (m_pSourceVoice[index]->SubmitSourceBuffer(&buff) != S_OK)
		return;

	m_pSourceVoice[index]->Start(0, XAUDIO2_COMMIT_NOW);
}


//*---------------------------------------------------------------------------------------
//*【?】再生止める(AIに頼んだ奴)
//*
//* [引数]
//* index：停止させるサウンドのハンドル
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
void SoundManager::StopXA2Sound(int index)
{
	if (index < 0 || index >= m_SoundIndex)
		return;

	m_pSourceVoice[index]->Stop(0, 0);
	m_pSourceVoice[index]->FlushSourceBuffers();
}