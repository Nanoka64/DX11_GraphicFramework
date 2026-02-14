#include "pch.h"
#include "SoundManager.h"
#include <windows.h>

constexpr int SE_CHANNELS_NUM    = 2;		// SEのチャンネル数
constexpr int VOICE_CHANNELS_NUM = 1;		// ボイスのチャンネル数
constexpr int SAMPLE_RATE		 = 44100;	// サンプル数 44.1kHz
constexpr int BITS_PER_SAMPLE	 = 16;		// サンプルあたりのビット数

constexpr float DEFAULT_SE_VOL    = 0.5f;		// SEのデフォルトの音量
constexpr float DEFAULT_BGM_VOL   = 0.3f;		// BGMのデフォルトの音量
constexpr float DEFAULT_VOICE_VOL = 0.7f;		// ボイスのデフォルトの音量


constexpr int SE_SVPOOL_NUM    = 64;  // SV(SourceVoice)のプール数
constexpr int VOICE_SVPOOL_NUM = 32;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
SoundManager::SoundManager():
	m_pXAudio2(nullptr),
	m_pMasteringVoice(nullptr),
	m_SEVolume(DEFAULT_SE_VOL),
	m_BGMVolume(DEFAULT_BGM_VOL),
	m_VoiceVolume(DEFAULT_VOICE_VOL)
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
//* [引数] 
//* なし
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::InitXA2Sound(void)
{
	HRESULT hr = S_OK;

	//COMインターフェースの初期化------------------------------------------
	//hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
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


	// ※ SEと声でプールは分けている。
	
	// **************************************************************************
	// ソースボイスの作成（SE用）
	// **************************************************************************
	for (int i = 0; i < SE_SVPOOL_NUM; i++)
	{
		SoundInstance sound;
		WAVEFORMATEX wf = {};
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.nChannels = SE_CHANNELS_NUM;	     // 2（ステレオ）
		wf.nSamplesPerSec = SAMPLE_RATE;     // 44.1kHz
		wf.wBitsPerSample = BITS_PER_SAMPLE; // 16bit
		wf.cbSize = 0;

		// 1ブロックあたりのバイト数 = (チャンネル数) * (1サンプルあたりのバイト数)
		wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);

		// 1秒あたりの平均バイト数 = (サンプリングレート) * (ブロックサイズ)
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

		//ソースボイス作成
		if (m_pXAudio2->CreateSourceVoice(&sound._pSourceVoice, &wf) != S_OK){
			return false;
		}

		// SE
		m_SoundSlotArray.push_back(sound);
	}

	// **************************************************************************
	// ソースボイスの作成（声用）
	// **************************************************************************
	for (int i = 0; i < VOICE_SVPOOL_NUM; i++)
	{
		SoundInstance sound;
		WAVEFORMATEX wf = {};
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.nChannels = VOICE_CHANNELS_NUM;	 // 1（モノラル）
		wf.nSamplesPerSec = SAMPLE_RATE;     // 44.1kHz
		wf.wBitsPerSample = BITS_PER_SAMPLE; // 16bit
		wf.cbSize = 0;

		// 1ブロックあたりのバイト数 = (チャンネル数) * (1サンプルあたりのバイト数)
		wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);

		// 1秒あたりの平均バイト数 = (サンプリングレート) * (ブロックサイズ)
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

		//ソースボイス作成
		if (m_pXAudio2->CreateSourceVoice(&sound._pSourceVoice, &wf) != S_OK) {
			return false;
		}
		// ボイス
		m_VoiceSoundSlotArray.push_back(sound);
	}



	// **************************************************************************
	// BGM用ソースボイスの作成
	// **************************************************************************
	SoundInstance sound;
	WAVEFORMATEX wf = {};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = SE_CHANNELS_NUM;	     // 2（ステレオ）
	wf.nSamplesPerSec = SAMPLE_RATE;     // 44.1kHz
	wf.wBitsPerSample = BITS_PER_SAMPLE; // 16bit
	wf.cbSize = 0;
	// 1ブロックあたりのバイト数 = (チャンネル数) * (1サンプルあたりのバイト数)
	wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);

	// 1秒あたりの平均バイト数 = (サンプリングレート) * (ブロックサイズ)
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

	//ソースボイス作成
	if (m_pXAudio2->CreateSourceVoice(&m_BGM_SoundSlot._pSourceVoice, &wf) != S_OK) {
		return false;
	}


	// システム
	LoadSE_Wav("Resource/Sound/SE/System/moving-cursor-2.wav", SOUND_ID::SYSTEM_MOVING_CURSOR01);

	// 武器
	LoadSE_Wav("Resource/Sound/SE/Weapon/GunFire_01.wav", SOUND_ID::GUN_FIRE01);
	LoadSE_Wav("Resource/Sound/SE/Weapon/GunFire_02.wav", SOUND_ID::GUN_FIRE02);

	// 兵士
	LoadSE_Wav("Resource/Sound/SE/Ranger/パンチの衣擦れ1.wav", SOUND_ID::SOLDIER_R_JUMP_IN);
	LoadSE_Wav("Resource/Sound/SE/Ranger/ジャンプの着地.wav", SOUND_ID::SOLDIER_R_JUMP_LAND);

	// 敵
	LoadSE_Wav("Resource/Sound/SE/Enemy/DSGNImpt_Impact Smear Short 04_RSCPC_HV.wav", SOUND_ID::ENEMY_ANT_HIT01);
	LoadSE_Wav("Resource/Sound/SE/Enemy/DSGNImpt_Impact High Spark 04_RSCPC_HV.wav", SOUND_ID::ENEMY_ANT_DEAD);

	// BGM
	LoadSE_Wav("Resource/Sound/BGM/Flash_Shadow.wav", SOUND_ID::BGM_TITLE_01);

	// ボイス
	LoadVoice_Wav("Resource/Sound/Voice/Ranger/an000_01.wav", VOICE_ID::SOLDIER_R_SHOUT_01);
	LoadVoice_Wav("Resource/Sound/Voice/Ranger/an000_02.wav", VOICE_ID::SOLDIER_R_SHOUT_02);
	LoadVoice_Wav("Resource/Sound/Voice/Ranger/an000_03.wav", VOICE_ID::SOLDIER_R_SHOUT_03);

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
	// ソースボイス削除
	for (auto& instance : m_SoundSlotArray) {
		if (instance._pSourceVoice) {
			instance._pSourceVoice->DestroyVoice();
		}
	}	
	
	// マスターボイス
	if (m_pMasteringVoice != nullptr)
	{
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = nullptr;
	}

	// XAudio本体
	if (m_pXAudio2 != nullptr)
	{
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}


	CoUninitialize();
}


//*---------------------------------------------------------------------------------------
//*【?】SE用WAVファイルの読み込み
//* 
//* [注意]
//* ・対応フォーマット
//* 　拡張子：wav 
//* 　チャンネル数：2（ステレオ）
//* 　サンプル数：44100（44.1kHz）
//* 　サンプルビット数：16
//* 
//* [引数]
//* *filename：読み込むファイル名 
//* _id：対応するSEのID
//* 
//* [返値]
//* true:成功
//* false:失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::LoadSE_Wav(const char *_filename, SOUND_ID _id)
{
	auto it = m_SE_WaveResourceMap.find(_id);
	if (it != m_SE_WaveResourceMap.end()){
		MessageBox(NULL, "既に登録されたIDです", "SoundManager", MB_OK);
		return false;
	}

	WaveResource wavRes = {};

	// wavのロード
	if (!Internal_Load_Wav(_filename, m_SE_WaveResourceMap[_id]))
	{
		return false;
	}


	return true;
}


//*---------------------------------------------------------------------------------------
//*【?】ボイス用WAVファイルの読み込み
//* 
//* [注意]
//* ・対応フォーマット
//* 　拡張子：wav 
//* 　チャンネル数：2（ステレオ）
//* 　サンプル数：44100（44.1kHz）
//* 　サンプルビット数：16
//* 
//* [引数]
//* *filename：読み込むファイル名 
//* _id：対応するボイスID
//* 
//* [返値]
//* true:成功
//* false:失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::LoadVoice_Wav(const char* _filename, VOICE_ID _id)
{
	auto it = m_Voice_WaveResourceMap.find(_id);
	if (it != m_Voice_WaveResourceMap.end()) {
		MessageBox(NULL, "既に登録されたIDです", "SoundManager", MB_OK);
		return false;
	}

	WaveResource wavRes = {};

	// wavのロード（チャンネル数 : 1（モノラル））
	if (!Internal_Load_Wav(_filename, m_Voice_WaveResourceMap[_id], 1))
	{
		return false;
	}

	return true;
}


//*---------------------------------------------------------------------------------------
//*【?】WAVファイルの読み込み（実際の内部的な読み込み関数）
//* 
//* WAVファイルはRIFF形式という方法で構成されていて、
//* チャンクというデータの塊が複数連結している。
//* チャンクは基本的に「ID」「サイズ」「中身」という感じで要素が入っている
//* waveファイルではRIFFが親チャンクとなり、その子としてfmt、dataなどのチャンクが居るっぽい
//*
//* fmt：形式情報
//* data：実際の波形データ
//* 
//* [注意]
//* ・対応フォーマット
//* 　拡張子：wav 
//* 　チャンネル数：2（ステレオ）
//* 　サンプル数：44100（44.1kHz）
//* 　サンプルビット数：16
//* 
//* [引数]
//* *filename：読み込むファイル名 
//* &_out：波形データの出力先
//* 
//* [返値]
//* true:成功
//* false:失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::Internal_Load_Wav(const char* _filename, WaveResource& _out, int _channelNum, int _sampleLate, int _bitsPersample )
{
	HMMIO mmio_handle = NULL;	// mmioハンドル
	DWORD size;
	BYTE* buffer;
	WAVEFORMATEX wfx = {};		// waveファイルのフォーマット情報
	MMCKINFO ckInfo = {};		// チャンク情報
	MMCKINFO rifckInfo = {};	// RIFFチャンク用
	MMRESULT mmhr;

	char str_buffer[512] = {};
	strcpy_s(str_buffer, _filename);

	//	ファイルを開く
	mmio_handle = mmioOpen(str_buffer, NULL, MMIO_ALLOCBUF | MMIO_READ);

	if (!mmio_handle) {
		MessageBox(NULL, "wavファイルを開けませんでした", "SoundManager", MB_OK);
		return false;
	}

	// ##########################################################################################
	//							RIFF（親）チャンクへ進入
	// ##########################################################################################
	mmhr = mmioDescend(mmio_handle, &rifckInfo, NULL, 0);

	// fccType が 'WAVE' であることを確認
	if (mmhr != MMSYSERR_NOERROR || rifckInfo.fccType != mmioFOURCC('W', 'A', 'V', 'E')) {
		MessageBox(NULL, "RIFFチャンクへの進入失敗", "SoundManager", MB_OK);
		mmioClose(mmio_handle, 0);
		return false;
	}


	// ##########################################################################################
	//							fmtチャンクへ進入
	// ##########################################################################################
	ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmhr = mmioDescend(mmio_handle, &ckInfo, &rifckInfo, MMIO_FINDCHUNK);
	if (mmhr != MMSYSERR_NOERROR) {
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		MessageBox(NULL, "fmtチャンクへの進入失敗", "SoundManager", MB_OK);
		return false;
	}

	DWORD readSize = mmioRead(mmio_handle, (HPSTR)&wfx, sizeof(wfx)/* std::min((DWORD)sizeof(WAVEFORMATEX), ckInfo.cksize)*/);	// 読み込み

	wfx.cbSize = 0;

	// フォーマットチェック
	if (wfx.wFormatTag != WAVE_FORMAT_PCM) {
		MessageBox(NULL, "フォーマットエラーです", "SoundManager", MB_OK);
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		return false;
	}
	// チャンネル数チェック
	if (wfx.nChannels != _channelNum) {
		MessageBox(NULL, "フォーマットエラーです", "SoundManager", MB_OK);
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		return false;
	}
	// サンプル数チェック
	if (wfx.nSamplesPerSec != _sampleLate) {
		MessageBox(NULL, "フォーマットエラーです", "SoundManager", MB_OK);
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		return false;
	}
	// ビット数チェック
	if (wfx.wBitsPerSample != _bitsPersample) {
		MessageBox(NULL, "フォーマットエラーです", "SoundManager", MB_OK);
		mmioClose(mmio_handle, MMIO_FHOPEN);	// ファイルを閉じる
		return false;
	}

	mmioAscend(mmio_handle, &ckInfo, 0); // fmtチャンクから退出


	// ##########################################################################################
	//							dataチャンクへ進入
	// ##########################################################################################
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
		delete[] buffer;	// バッファの開放
		// 読み込みサイズが一致していません
		mmioClose(mmio_handle, MMIO_FHOPEN);
		return false;
	}


	// 格納
	_out._size = size;
	_out._waveBuffer = buffer;
	_out._waveFormat = wfx;


	// ファイル閉じる
	mmioClose(mmio_handle, MMIO_FHOPEN);

	buffer = nullptr;

	return true;
}


//*---------------------------------------------------------------------------------------
//*【?】SEの再生
//*
//* [引数]
//* index：再生するサウンドのハンドル
//* _loop：ループさせるか
//* 
//* [返値] 
// true:成功
// false:失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::PlaySE(SOUND_ID _id, bool _loop )
{
	// 見つからなければそのまま返す
	auto it = m_SE_WaveResourceMap.find(_id);
	if (it == m_SE_WaveResourceMap.end()){
		return false;
	}

	auto& resource = m_SE_WaveResourceMap[_id];
	HRESULT hr = S_OK;

	// 波形データ
	XAUDIO2_BUFFER buff = {};
	buff.pAudioData = resource._waveBuffer;		// 波形データの先頭アドレス
	buff.AudioBytes = resource._size;			// 波形データのサイズ
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.LoopCount = _loop ? XAUDIO2_LOOP_INFINITE : 0;	  // ループさせるか


	// 再生が終了しているボイスを探して
	for (auto& slot : m_SoundSlotArray) {
		XAUDIO2_VOICE_STATE state;
		slot._pSourceVoice->GetState(&state);

		// 待機中のバッファが0個 ＝ 再生が終わっている
		if (state.BuffersQueued == 0)
		{
			hr = slot._pSourceVoice->SetFrequencyRatio(1.0f);	// ピッチは元の音声のまま
			if (FAILED(hr)) return false;

			// 音量
			hr = slot._pSourceVoice->SetVolume(m_SEVolume);
			if (FAILED(hr)) return false;

			// 音声バッファの追加
			hr = slot._pSourceVoice->SubmitSourceBuffer(&buff);
			if (FAILED(hr)) return false;

			// 音を鳴らす
			hr = slot._pSourceVoice->Start();
			if (FAILED(hr)) return false;

			return true;
		}
	}

	return false;
}


//*---------------------------------------------------------------------------------------
//*【?】SEの再生
//*　　 ピッチをランダムにする（銃声などのずっと同じ音にしたくない場合）
//*
//* [引数]
//* index：再生するサウンドのハンドル
//* _pitchRange：ピッチをどのくらいの範囲に収めるか（100の場合、±5%揺らぐ）
//* _loop：ループさせるか
//* 
//* [返値] 
// true:成功
// false:失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::PlaySE_RandPitch(SOUND_ID _id, int _pitchRange, bool _loop)
{
	// 見つからなければそのまま返す
	auto it = m_SE_WaveResourceMap.find(_id);
	if (it == m_SE_WaveResourceMap.end()) {
		return false;
	}

	auto& resource = m_SE_WaveResourceMap[_id];
	HRESULT hr = S_OK;

	// 波形データ
	XAUDIO2_BUFFER buff = {};
	buff.pAudioData = resource._waveBuffer;		// 波形データの先頭アドレス
	buff.AudioBytes = resource._size;			// 波形データのサイズ
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.LoopCount = _loop ? XAUDIO2_LOOP_INFINITE : 0;	  // ループさせるか


	// 再生が終了しているボイスを探して
	for (auto& slot : m_SoundSlotArray) {
		XAUDIO2_VOICE_STATE state;
		slot._pSourceVoice->GetState(&state);

		// 待機中のバッファが0個 ＝ 再生が終わっている
		if (state.BuffersQueued == 0)
		{
			// ピッチをいじる
			float pitch = 1.0f + ((rand() % _pitchRange - (_pitchRange / 2)) / 1000.0f); 
			hr = slot._pSourceVoice->SetFrequencyRatio(pitch);
			if (FAILED(hr)) return false;

			// 音量
			hr = slot._pSourceVoice->SetVolume(m_SEVolume);
			if (FAILED(hr)) return false;

			// 音声バッファの追加
			hr = slot._pSourceVoice->SubmitSourceBuffer(&buff);
			if (FAILED(hr)) return false;

			// 音を鳴らす
			hr = slot._pSourceVoice->Start();
			if (FAILED(hr)) return false;

			return true;
		}
	}

	return false;
}


//*---------------------------------------------------------------------------------------
//*【?】ボイスの再生
//*
//* [引数]
//* index：再生するサウンドのハンドル
//* 
//* [返値] 
// true:成功
// false:失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::PlayVoice(VOICE_ID _id)
{
	// 見つからなければそのまま返す
	auto it = m_Voice_WaveResourceMap.find(_id);
	if (it == m_Voice_WaveResourceMap.end()) {
		return false;
	}

	auto& resource = m_Voice_WaveResourceMap[_id];
	HRESULT hr = S_OK;

	// 波形データ
	XAUDIO2_BUFFER buff = {};
	buff.pAudioData = resource._waveBuffer;		// 波形データの先頭アドレス
	buff.AudioBytes = resource._size;			// 波形データのサイズ
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.LoopCount = 0;	  // ループさせない


	// 再生が終了しているボイスを探して
	for (auto& slot : m_VoiceSoundSlotArray) {
		XAUDIO2_VOICE_STATE state;
		slot._pSourceVoice->GetState(&state);

		// 待機中のバッファが0個 ＝ 再生が終わっている
		if (state.BuffersQueued == 0)
		{
			hr = slot._pSourceVoice->SetFrequencyRatio(1.0f);	// ピッチは元の音声のまま
			if (FAILED(hr)) return false;

			// 音量
			hr = slot._pSourceVoice->SetVolume(m_VoiceVolume);
			if (FAILED(hr)) return false;

			// 音声バッファの追加
			hr = slot._pSourceVoice->SubmitSourceBuffer(&buff);
			if (FAILED(hr)) return false;

			// 音を鳴らす
			hr = slot._pSourceVoice->Start();
			if (FAILED(hr)) return false;

			return true;
		}
	}

	return false;
}


//*---------------------------------------------------------------------------------------
//*【?】ボイスの再生（範囲ランダム）
//*
//* [引数]
//* _beginID：ランダム再生に使用する最初のハンドル
//* _range：_beginIDからのランダム範囲
//* 
//* [返値] 
// true:成功
// false:失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::PlayVoice_Rand(VOICE_ID _beginID, int _range)
{
	// ランダム求める
	VOICE_ID resultID = static_cast<VOICE_ID>(static_cast<int>(_beginID) + rand() % _range);

	// 見つからなければそのまま返す
	auto it = m_Voice_WaveResourceMap.find(resultID);
	if (it == m_Voice_WaveResourceMap.end()) {
		return false;
	}

	auto& resource = m_Voice_WaveResourceMap[resultID];
	HRESULT hr = S_OK;

	// 波形データ
	XAUDIO2_BUFFER buff = {};
	buff.pAudioData = resource._waveBuffer;		// 波形データの先頭アドレス
	buff.AudioBytes = resource._size;			// 波形データのサイズ
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.LoopCount = 0;	  // ループさせない


	// 再生が終了しているボイスを探して
	for (auto& slot : m_VoiceSoundSlotArray) {
		XAUDIO2_VOICE_STATE state;
		slot._pSourceVoice->GetState(&state);

		// 待機中のバッファが0個 ＝ 再生が終わっている
		if (state.BuffersQueued == 0)
		{
			hr = slot._pSourceVoice->SetFrequencyRatio(1.0f);	// ピッチは元の音声のまま
			if (FAILED(hr)) return false;

			// 音量
			hr = slot._pSourceVoice->SetVolume(m_VoiceVolume);
			if (FAILED(hr)) return false;

			// 音声バッファの追加
			hr = slot._pSourceVoice->SubmitSourceBuffer(&buff);
			if (FAILED(hr)) return false;

			// 音を鳴らす
			hr = slot._pSourceVoice->Start();
			if (FAILED(hr)) return false;

			return true;
		}
	}

	return false;
}




//*---------------------------------------------------------------------------------------
//*【?】BGMの再生
//*
//* [引数]
//* _id：再生するサウンドのID
//* _loop：ループするかどうか
//* 
//* [返値] 
// true:成功
// false:失敗
//*----------------------------------------------------------------------------------------
bool SoundManager::PlayBGM(SOUND_ID _id, bool _loop)
{
	// 見つからなければそのまま返す
	auto it = m_SE_WaveResourceMap.find(_id);
	if (it == m_SE_WaveResourceMap.end()) {
		return false;
	}

	auto& resource = m_SE_WaveResourceMap[_id];
	HRESULT hr = S_OK;

	// 波形データ
	XAUDIO2_BUFFER buff = {};
	buff.pAudioData = resource._waveBuffer;		// 波形データの先頭アドレス
	buff.AudioBytes = resource._size;			// 波形データのサイズ
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.LoopCount = _loop ? XAUDIO2_LOOP_INFINITE : 0;	  // ループさせるか


	XAUDIO2_VOICE_STATE state;
	m_BGM_SoundSlot._pSourceVoice->GetState(&state);

	// 音量
	hr = m_BGM_SoundSlot._pSourceVoice->SetVolume(m_BGMVolume);
	if (FAILED(hr)) return false;

	// 音声バッファの追加
	hr = m_BGM_SoundSlot._pSourceVoice->SubmitSourceBuffer(&buff);
	if (FAILED(hr)) return false;

	// 音を鳴らす
	hr = m_BGM_SoundSlot._pSourceVoice->Start();
	if (FAILED(hr)) return false;

	return true;
}


//*---------------------------------------------------------------------------------------
//*【?】再生止める
//*
//* [引数]
//* _type：サウンドの種類
//* _id：停止するサウンドのID
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
bool SoundManager::StopSound(SOUND_TYPE _type, SOUND_ID _id)
{
	// 見つからなければそのまま返す
	auto it = m_SE_WaveResourceMap.find(_id);
	if (it == m_SE_WaveResourceMap.end()) {
		return false;
	}

	auto& resource = m_SE_WaveResourceMap[_id];
	HRESULT hr = S_OK;

	if (_type == SOUND_TYPE::BGM)
	{
		// 再生停止（XAUDIO2_PLAY_TAILSは止めた後、リバーブを掛けれるらしい）
		hr = m_BGM_SoundSlot._pSourceVoice->Stop(XAUDIO2_PLAY_TAILS);
		if (FAILED(hr)) return false;
		
		// キューにたまっているバッファをクリアする
		hr = m_BGM_SoundSlot._pSourceVoice->FlushSourceBuffers();
		if (FAILED(hr)) return false;
	}

	return true;
}