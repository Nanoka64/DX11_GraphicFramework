#include "pch.h"
#include "SoundManager.h"
#include <windows.h>

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
SoundManager::SoundManager():
	m_pXAudio2(nullptr),
	m_pMasteringVoice(nullptr)
	//m_WavBuffer(nullptr),
	//m_SoundIndex(0)
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
	HRESULT hr = S_OK;

	//COMインターフェースの初期化------------------------------------------
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)){
		MessageBox(NULL, "初期化エラー", "SoundManager", MB_OK);
		return false;
	}


	//XAudio2デバイス作成------------------------------------------
	hr = XAudio2Create(&m_pXAudio2, XAUDIO2_DEBUG_ENGINE);
	if (FAILED(hr)) {
		CoUninitialize();
		return false;
	}

	
	//マスターボイスの作成------------------------------------------
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr)) {
		CoUninitialize();
		return false;
	}


	// ソースボイスの作成
	for (int i = 0; i < 64; i++)
	{
		SoundInstance sound;
		WAVEFORMATEX wf = {};
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.nChannels = 2;			// ステレオ
		wf.nSamplesPerSec = 44100;  // 44.1kHz
		wf.wBitsPerSample = 16;     // 16bit (一般的)
		wf.cbSize = 0;

		// 1ブロックあたりのバイト数 = (チャンネル数) * (1サンプルあたりのバイト数)
		wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);

		// 1秒あたりの平均バイト数 = (サンプリングレート) * (ブロックサイズ)
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

		//ソースボイス作成
		if (m_pXAudio2->CreateSourceVoice(&sound._pSourceVoice, &wf) != S_OK){
			continue;
		}

		m_SoundInstanceArray.push_back(sound);
	}

	return true;
}

//*---------------------------------------------------------------------------------------
//*【?】解放
//* ※ 作成した順番と逆に解放
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

	// ソースボイス削除
	for (auto& instance : m_SoundInstanceArray) {
		if (instance._pSourceVoice) {
			instance._pSourceVoice->DestroyVoice();
			instance._pSourceVoice = nullptr;
		}
	}

	CoUninitialize();
}

//*---------------------------------------------------------------------------------------
//*【?】サウンドファイルの読み込み
//* WAVファイルはRIFF形式という方法で構成されていて、
//* チャンクというデータの塊が複数連結している。
//* チャンクは基本的に「ID」「サイズ」「中身」という感じで要素が入っている
//* waveファイルではRIFFが親チャンクとなり、その子としてfmt、dataなどのチャンクが居るっぽい
//*
//* fmt：形式情報
//* data：実際の波形データ
//* 
//* [引数]
//* *filename：読み込むファイル名 
//* [返値]
//* true:成功
//* false:失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::LoadXA2Sound(char *_filename, SOUND_ID _id)
{
	HMMIO mmio_handle = NULL;	// mmioハンドル
	DWORD size;
	BYTE* buffer;
	WAVEFORMATEX wfx;			// waveファイルのフォーマット情報
	MMCKINFO ckInfo = {};		// チャンク情報
	MMCKINFO rifckInfo = {};	// RIFFチャンク用
	MMRESULT mmhr;

	//	ファイルを開く
	mmio_handle = mmioOpen(_filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
	
	if (!mmio_handle){
		MessageBox(NULL, "wavファイルを開けませんでした", "SoundManager", MB_OK);
		return false;
	}

	// RIFF（親）チャンクへ進入************************************************************
	mmhr = mmioDescend(mmio_handle, &rifckInfo, NULL, 0);	
	if (mmhr != MMSYSERR_NOERROR){
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		MessageBox(NULL, "RIFFチャンクへの進入失敗", "SoundManager", MB_OK);
		return false;
	}


	// fmtチャンクへ進入************************************************************
	ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmhr = mmioDescend(mmio_handle, &ckInfo, &rifckInfo, MMIO_FINDCHUNK);
	if (mmhr != MMSYSERR_NOERROR) {
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		MessageBox(NULL, "fmtチャンクへの進入失敗", "SoundManager", MB_OK);
		return false;
	}
	
	DWORD readSize = mmioRead(mmio_handle, (HPSTR)&wfx, sizeof(wfx)/* std::min((DWORD)sizeof(WAVEFORMATEX), ckInfo.cksize)*/);	// 読み込み
	wfx.cbSize = 0;
	
	// サイズチェック
	if (readSize != ckInfo.cksize){
		MessageBox(NULL, "読み込みサイズが一致していません", "SoundManager", MB_OK);
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		return false;
	}
	// フォーマットチェック
	if (wfx.wFormatTag != WAVE_FORMAT_PCM){
		MessageBox(NULL, "Waveフォーマットエラーです", "SoundManager", MB_OK);
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		return false;
	}

	mmioAscend(mmio_handle, &ckInfo, 0); // fmtチャンクから退出


	// dataチャンクへ進入************************************************************
	ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmhr = mmioDescend(mmio_handle, &ckInfo, &rifckInfo, MMIO_FINDCHUNK); 
	if (mmhr != MMSYSERR_NOERROR) {
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		MessageBox(NULL, "dataチャンクへの進入失敗", "SoundManager", MB_OK);
		return false;
	}

	size = ckInfo.cksize;
	buffer = new BYTE[size];
	readSize = mmioRead(mmio_handle, (HPSTR)buffer, size);
	if (readSize != ckInfo.cksize) {
		// 読み込みサイズが一致していません
		mmioClose(mmio_handle, MMIO_FHOPEN);
		return false;
	}

	// データを保持
	m_WaveResourceMap[_id]._waveFormat = wfx;
	m_WaveResourceMap[_id]._waveBuffer = buffer;
	m_WaveResourceMap[_id]._size = size;

	// ファイルを閉じる
	mmioClose(mmio_handle, MMIO_FHOPEN);

	buffer = nullptr;

	return true;
}

//*---------------------------------------------------------------------------------------
//*【?】サウンドの再生
//*
//* [引数]
//* index：再生するサウンドのハンドル
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
void SoundManager::PlayXA2Sound(SOUND_ID _id)
{
	// 見つからなければそのまま返す
	auto it = m_WaveResourceMap.find(_id);
	if (it == m_WaveResourceMap.end())
	{
		return;
	}


	// 波形データをソースボイスに渡す
	XAUDIO2_BUFFER buff = {};
	buff.pAudioData = m_WaveResourceMap[_id]._waveBuffer;
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.AudioBytes = m_WaveResourceMap[_id]._size;


	//if (m_pSourceVoice[index]->SubmitSourceBuffer(&buff) != S_OK) {
	//	return;
	//}

	//// 音を鳴らす
	//m_pSourceVoice[index]->Start(0, XAUDIO2_COMMIT_NOW);
}


//*---------------------------------------------------------------------------------------
//*【?】再生止める(AIに頼んだ奴)
//*
//* [引数]
//* index：停止させるサウンドのハンドル
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
void SoundManager::StopXA2Sound(SOUND_ID _id)
{
	//if (index < 0 || index >= m_SoundIndex)
	//	return;

	//m_pSourceVoice[index]->Stop(0, 0);
	//m_pSourceVoice[index]->FlushSourceBuffers();
}