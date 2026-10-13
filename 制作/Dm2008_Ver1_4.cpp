#include "dm2008_Ver1_4.h"

#define kREL(x) if(x){x->Release();x=nullptr;}
#define verCloseHandle(h) {\
	if((h != INVALID_HANDLE_VALUE) || (h != nullptr))\
	{\
		CloseHandle(h);\
		h = nullptr;\
	}\
}
#define verDelete_Scalar(p) {\
	if(p)\
	{\
		delete p;\
		p = nullptr;\
	}\
}
#define verDelete_Vector(p) {\
	if(p)\
	{\
		delete[] p;\
		p = nullptr;\
	}\
}
namespace  DM
{

	Obj::WP Obj::winst;	//	?????X???????????
	Obj::Obj()
	{
		this->wnd = 0;
		this->ds_ptr = nullptr;
		this->dwBufferSize = 0;
		this->pri_ptr = 0;
		ZeroMemory(&this->caps, sizeof(DSCAPS));
	}
	bool Obj::Initialize(HWND wnd_, DWORD dwBufferSize)
	{
		wnd = wnd_;


		//	DirectSound????
		if (FAILED(CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC,
			IID_IDirectSound8, (void **)&this->ds_ptr)))
		{
			return false;	// return Err::CCIS;
		}
		if (FAILED(this->ds_ptr->Initialize(NULL)))
		{
			return false;	// return Err::INIT;
		}
		if (FAILED(this->ds_ptr->SetCooperativeLevel(wnd, DSSCL_PRIORITY)))
		{
			return false;	// return Err::SCL;
		}
		//	????????????????m
		DSBUFFERDESC desc =
		{
			sizeof(DSBUFFERDESC)
		};

		desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
		if (FAILED(this->ds_ptr->CreateSoundBuffer(&desc, &this->pri_ptr, NULL)))
		{
			return false;	// return Err::CSB;
		}
		//	???????
		Obj::ds_ptr->GetCaps(&this->caps);
		Obj::dwBufferSize = dwBufferSize;
		//return Err::NOTHING;
		return true;
	}

	Obj::~Obj()
	{
		//	DirectSound
		kREL(this->pri_ptr);
		kREL(this->ds_ptr);
	}
	//	DM???????w?m??
	Obj::SP Obj::Create(HWND		hw_, DWORD dwBufferSize)	//	?????????
	{
		if (winst.expired()){
			Obj::SP sp = Obj::SP(new Obj());
			if (sp->Initialize(hw_, dwBufferSize)){
				winst = sp;
				return sp;
			}
		}
		return 0;
	}
	Obj::SP Obj::GetInst()
	{
		if (!winst.expired()){
			return winst.lock();
		}
		return 0;
	}



	//--------------------------------------------------------------------------------------------
	void Obj::Set(shared_ptr<class  Sound>  s_)
	{
		this->streams.push_back(s_);
	}
	void Obj::UpDate(void)
	{
		class cNullCheck{
		public:
			// ???????????????true???
			bool operator()(Sound::WP t_) const {
				bool rtv = t_.expired();
				return  rtv;
			}
		};
		auto endIt = remove_if(this->streams.begin(), this->streams.end(), cNullCheck());
		this->streams.erase(endIt, this->streams.end());	//????????????????????

		//	???????????????
		for (auto it = this->streams.begin();
			it != this->streams.end();
			it++)
		{
			auto sp = it->lock();
			sp->Check_Stream(this->dwBufferSize);
		}

	}
	//--------------------------------------------------------------------------------------------
	void Obj::GetDeviceCaps(DSCAPS* caps_ptr)
	{
		memcpy(caps_ptr, &this->caps, sizeof(DSCAPS));
	}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------
	Err::Type Sound::CreateSoundBuffer(WAVEFORMATEX* wf_ptr, DWORD dwBufbyte)
	{
		DSBUFFERDESC desc;
		LPDIRECTSOUNDBUFFER lpTempBuff = NULL;
		Err::Type hRet = Err::NOTHING;		//	??????

		//	???????
		if (!wf_ptr)
		{
			return Err::SFMT;
		}
		//	????????????k
		ZeroMemory(&desc, sizeof(desc));
		desc.lpwfxFormat = wf_ptr;
		desc.dwSize = sizeof(DSBUFFERDESC);
		desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_LOCDEFER |
			DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN;
		desc.dwBufferBytes = dwBufbyte;	//	???????
		//	DirectSoundBuffer?w?m
		auto  dm = Obj::GetInst();
		if (FAILED(dm->ds_ptr->CreateSoundBuffer(&desc, &lpTempBuff, NULL)))
		{
			hRet = Err::CSB;
			goto end;
		}
		//	DirectSoundBuffer8????
		if (FAILED(lpTempBuff->QueryInterface(IID_IDirectSoundBuffer8, (void**)&this->sinfo.sec_ptr)))
		{
			hRet = Err::QI;
			goto end;
		}
	end:
		kREL(lpTempBuff);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	Sound::SP  Sound::CreateSE(const char* szFileName)
	{
		auto sp = SP(new Sound());
		if (sp){
			bool  rtv;
			rtv = sp->CreateSE_Sub(szFileName);
			if (rtv){
				auto  ob = Obj::GetInst();
				return sp;
			}
		}
		return 0;
	}
	//--------------------------------------------------------------------------------------------
	Sound::SP  Sound::CreateStream(const char* szFileName)
	{
		auto sp = SP(new Sound());
		if (sp){
			bool  rtv;
			rtv = sp->CreateStream_Sub(szFileName);
			if (rtv){
				auto  ob = Obj::GetInst();
				ob->Set(sp);
				return sp;
			}
		}
		return 0;
	}
	//--------------------------------------------------------------------------------------------
	bool Sound::CreateSE_Sub(const char* szFileName)
	{
		WAVEFORMATEX wf;
		BYTE* data_ptr = NULL;
		bool hRet = true;		//	??????

		kREL(this->sinfo.sec_ptr);
		ZeroMemory(&wf, sizeof(WAVEFORMATEX));
		//	DirectSoundBuffer???k
		if (Err::NOTHING != this->LoadWaveStreamFirst(szFileName, &data_ptr, 0, &wf))
		{
			hRet = false;// = Err::WRG;
			goto end;
		}
		//	??????w?m
		if (Err::NOTHING != this->CreateSoundBuffer(&wf, this->sinfo.dwReadPosition - this->sinfo.dwHeaderSize))
		{
			goto end;
		}
		//	???????
		this->WriteSoundData(data_ptr, 0, this->sinfo.dwReadPosition - this->sinfo.dwHeaderSize);
		//	??????????~
		strcpy_s(this->sinfo.szFileName, MAX_PATH, szFileName);	//	????????????~
		this->sinfo.iStreamPos = 0;
		this->sinfo.bLoop = false;
		this->sinfo.bStream = false;
	end:
		verDelete_Vector(data_ptr);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	bool Sound::CreateStream_Sub(const char* szFileName)
	{
		BYTE* data_ptr = NULL;
		WAVEFORMATEX wf;
		bool  hRet = true;// Err::NOTHING;		//	??????

		auto  ds = Obj::GetInst();


		kREL(this->sinfo.sec_ptr);
		//	DirectSoundBuffer???k
		if (Err::NOTHING != this->LoadWaveStreamFirst(szFileName, &data_ptr, ds->dwBufferSize, &wf))
		{
			hRet = false;// Err::WRG;
			goto end;
		}
		//	??????w?m
		if (Err::NOTHING != this->CreateSoundBuffer(&wf, ds->dwBufferSize * 2))
		{
			goto end;
		}
		//	???????
		this->WriteSoundData(data_ptr, 0, ds->dwBufferSize);
		//	??????????~
		strcpy_s(this->sinfo.szFileName, MAX_PATH, szFileName);	//	????????????~
		this->sinfo.iStreamPos = 0;
		this->sinfo.bStream = true;
	end:
		verDelete_Vector(data_ptr);
		return hRet;
	}

	//--------------------------------------------------------------------------------------------
	Sound::Sound()
	{
		this->sinfo.sec_ptr = nullptr;
		this->sinfo.bLoop = false;
		this->sinfo.bStream = false;
		this->sinfo.dwCnt = 0;
		this->sinfo.dwHeaderSize = 0;
		this->sinfo.dwLoop = 0;
		this->sinfo.dwReadPosition = 0;
		this->sinfo.dwReStart = 0;
		this->sinfo.iVolume = DSBVOLUME_MAX;
	}
	//--------------------------------------------------------------------------------------------
	Sound::~Sound()
	{
		kREL(this->sinfo.sec_ptr);
	}
	//--------------------------------------------------------------------------------------------
	DWORD Sound::GetCurrentPosition()
	{
		return this->sinfo.dwReadPosition;
	}
	//--------------------------------------------------------------------------------------------
	DWORD Sound::GetStatus( )
	{
		DWORD dwStatus;

		this->sinfo.sec_ptr->GetStatus(&dwStatus);
		return dwStatus;
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::Play_FadeIn(DWORD dwPlayFlag, int iValue)
	{
		HRESULT hRet;

		//	???????????????w???
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}
		hRet = this->sinfo.sec_ptr->Play(0, 0, dwPlayFlag);
		this->SetVolume(kVOLUME_MIN);
		this->sinfo.iVolume = kVOLUME_MIN;
		this->sinfo.effectflag = Effect::FADEIN;
		this->sinfo.iFade = iValue;
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::Play_Normal(bool bLoop)
	{
		HRESULT hRet;
		DWORD dwPlayFlag = 0;

		//	???????????????w???
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}
		//	??w
		this->sinfo.sec_ptr->SetCurrentPosition(0);
		if (bLoop || this->sinfo.bStream)
		{
			dwPlayFlag = DSBPLAY_LOOPING;
		}
		hRet = this->sinfo.sec_ptr->Play(0, 0, dwPlayFlag);
		//	???????????????
		this->SetVolume(this->sinfo.iVolume);
		this->sinfo.bLoop = bLoop;
		this->sinfo.effectflag = Effect::NOTHING;
		return hRet;
	}

	//--------------------------------------------------------------------------------------------
	Err::Type Sound::LoadWaveStreamFirst(const char* szFileName,
		BYTE** data_ptr, DWORD dwSize, WAVEFORMATEX* wf_ptr)
	{
		HANDLE hFile = NULL;		//	Wave????????
		Err::Type hRet = Err::NOTHING;	//	???????~???
		DWORD dwResult;			//	ReadFile??????~??
		DWORD dwRiffSize;		//	RIFF??????????????
		DWORD dwFmtSize;		//	fmt??????????????
		DWORD dwDataSize;		//	data??????????????
		char szChunk[4] = "";	//	???????????~????\0???X?????
		bool bLoop;

		//	???????
		hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hRet = Err::INVOPEN;
			goto end;
		}
		//	RIFF??????????
		ReadFile(hFile, szChunk, 4, &dwResult, NULL);
		//	?????????????RIFF?????????
		if ((dwResult != 4) || (memcmp("RIFF", szChunk, 4)))
		{
			hRet = Err::RIFF;
			goto end;
		}
		ReadFile(hFile, &dwRiffSize, sizeof(DWORD), &dwResult, NULL);
		ReadFile(hFile, szChunk, 4, &dwResult, NULL);
		//	?????????????WAVE?????????
		if ((dwResult != 4) || (memcmp("WAVE", szChunk, 4)))
		{
			hRet = Err::RIFF;
			goto end;
		}
		//	Foramt????
		bLoop = true;
		while (bLoop)
		{
			ReadFile(hFile, szChunk, 4, &dwResult, NULL);
			//	???????????????t????????????
			if (dwResult != 4)
			{
				hRet = Err::DATA;
				goto end;
			}
			//	??????????????
			if (memcmp("fmt", szChunk, 3) == 0)
			{
				bLoop = false;
			}
			else
			{
				//	??????
				ReadFile(hFile, &dwDataSize, 4, &dwResult, NULL);
				SetFilePointer(hFile, dwDataSize, 0, FILE_CURRENT);
			}
		}
		//	fmt??????
		ReadFile(hFile, &dwFmtSize, sizeof(DWORD), &dwResult, NULL);
		//	?????????????
		ReadFile(hFile, wf_ptr, dwFmtSize, &dwResult, NULL);
		if ((wf_ptr->wFormatTag != WAVE_FORMAT_PCM))
		{
			hRet = Err::NOTPCM;
			goto end;
		}
		wf_ptr->cbSize = 0;
		//	Data??????
		bLoop = true;
		while (bLoop)
		{
			ReadFile(hFile, szChunk, 4, &dwResult, NULL);
			//	???????????????t????????????
			if (dwResult != 4)
			{
				hRet = Err::DATA;
				goto end;
			}
			//	Data??????????
			if (memcmp("data", szChunk, 4) == 0)
			{
				bLoop = false;
			}
			else
			{
				//	??????
				ReadFile(hFile, &dwDataSize, 4, &dwResult, NULL);
				SetFilePointer(hFile, dwDataSize, 0, FILE_CURRENT);
			}
		}
		//	???????????
		ReadFile(hFile, &dwDataSize, 4, &dwResult, NULL);
		//	WAV?????????X??????????????????p??
		this->sinfo.dwHeaderSize = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
		//	WAVE?????????
		if (dwSize == 0)
		{
			dwSize = dwDataSize;
		}
		(*data_ptr) = new BYTE[dwSize];
		//	SetFilePointer(hFile, Sound::sinfo[iNumber].dwHeaderSize+dwOffset, NULL, FILE_BEGIN);
		ReadFile(hFile, *data_ptr, dwSize, &dwResult, NULL);
		//	??k????????????????????
		if (dwResult != dwSize)
		{
			hRet = Err::READERROR;
			goto end;
		}
		//	???????????????????????
		this->sinfo.dwReadPosition = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	end:
		verCloseHandle(hFile);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	void Sound::SetLoopInfo(int iReStartPos, int iLoopCnt)
	{
		this->sinfo.dwReStart = iReStartPos;
		this->sinfo.dwCnt = iLoopCnt;
	}
	//--------------------------------------------------------------------------------------------
	void Sound::FadeOut(int iValue)
	{
		this->sinfo.effectflag = Effect::FADEOUT;
		this->sinfo.iFade = iValue;
	}
	//--------------------------------------------------------------------------------------------
	int Sound::GetVolume( )
	{
		long iVolume = 0;

		this->sinfo.sec_ptr->GetVolume(&iVolume);
		return iVolume;
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::SetPan(int iPan)
	{
		return this->sinfo.sec_ptr->SetPan(iPan);
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::SetFrequency(DWORD dwFreq)
	{
		return this->sinfo.sec_ptr->SetFrequency(dwFreq);
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::SetVolume(long int iVolume)
	{
		this->sinfo.iVolume = iVolume;
		return this->sinfo.sec_ptr->SetVolume(iVolume);
	}
	//--------------------------------------------------------------------------------------------
	void Sound::WriteMute(int iSize)
	{
		LPVOID lpSoundData1 = NULL, lpSoundData2 = NULL;
		DWORD size1, size2;

		this->sinfo.sec_ptr->Lock(0, iSize, &lpSoundData1, &size1,
			&lpSoundData2, &size2, DSBLOCK_ENTIREBUFFER);
		// ????????????
		memset(lpSoundData1, 0, iSize);
		// ?????
		this->sinfo.sec_ptr->Unlock(lpSoundData1, size1, lpSoundData2, size2);
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::Stop( )
	{
		auto  ds = Obj::GetInst();
		if (!this->sinfo.sec_ptr)
		{
			return HRESULT(0);
		}
		//	Sound::sinfo[iNumber].bStream = false;
		if (this->sinfo.bStream) {
			WriteMute(ds->dwBufferSize);
			this->sinfo.dwReadPosition = this->sinfo.dwHeaderSize;			//	?????????????????
			this->sinfo.iStreamPos = 0;
		}
		return this->sinfo.sec_ptr->Stop();
	}
	//--------------------------------------------------------------------------------------------
	void Sound::WriteSoundData(BYTE* data_ptr, int iOffset, int iSize)
	{
		LPVOID lpSoundData1 = NULL, lpSoundData2 = NULL;
		DWORD size1, size2;

		this->sinfo.sec_ptr->Lock(iOffset, iSize, &lpSoundData1, &size1,
			&lpSoundData2, &size2, DSBLOCK_ENTIREBUFFER);
		// ????????????
		memcpy(lpSoundData1, data_ptr, iSize);
		// ?????
		this->sinfo.sec_ptr->Unlock(lpSoundData1, size1, lpSoundData2, size2);
	}
	//--------------------------------------------------------------------------------------------
	Err::Type Sound::WriteWaveStream(const char* szFileName,
		BYTE* data_ptr, int iOffset, DWORD dwSize, int iWriteOffset)
	{
		HANDLE hFile = NULL;
		DWORD dwResult;
		DWORD dwRead;	//	???????????????????
		Err::Type hRet = Err::NOTHING;

		//	???????
		hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hRet = Err::INVOPEN;
			goto end;
		}
		//	?????????
		SetFilePointer(hFile, iOffset, NULL, FILE_BEGIN);
		ReadFile(hFile, data_ptr, dwSize, &dwResult, NULL);
		//	??k????????????????????
		if (dwResult != dwSize)
		{
			//	EOF??????k???????
			//	?????????????????????????k??
			dwRead = SetFilePointer(hFile, 0, NULL, FILE_CURRENT) - this->sinfo.dwReadPosition;
			if (!this->sinfo.bLoop)
			{
				ZeroMemory(&data_ptr[dwRead], dwSize - dwRead);
				this->sinfo.effectflag = Effect::STOP;
				goto end;
			}
			//	??????????????k??
			if (this->sinfo.dwReStart != 0)	//	?????????????k????????
			{
				this->sinfo.dwReadPosition = this->sinfo.dwReStart + this->sinfo.dwHeaderSize;
			}
			else
			{
				this->sinfo.dwReadPosition = this->sinfo.dwHeaderSize;
			}
			//	?????????????
			verCloseHandle(hFile);
			this->WriteSoundData(data_ptr, iWriteOffset, dwRead);
			this->WriteWaveStream(szFileName, &data_ptr[dwRead],
				this->sinfo.dwReadPosition, dwSize - dwRead, iWriteOffset + dwRead);
		}
		else
		{
			//	???????????????????????
			this->sinfo.dwReadPosition = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
			this->WriteSoundData(data_ptr, iWriteOffset, dwSize);
		}
	end:
		verCloseHandle(hFile);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	void Sound::LoadStream(int iWriteOffset, int iStreamPos)
	{
		BYTE* data_ptr = NULL;	//	???????

		auto  dm = Obj::GetInst();
		data_ptr = new BYTE[dm->dwBufferSize];
		WriteWaveStream(this->sinfo.szFileName, data_ptr,
			this->sinfo.dwReadPosition, dm->dwBufferSize, iWriteOffset);
		this->sinfo.iStreamPos = iStreamPos;
		verDelete_Vector(data_ptr);
	}
	//--------------------------------------------------------------------------------------------
	void Sound::Check_Stream(DWORD  dwBufferSize_)
	{
		DWORD dwStatus;	//	??w??????~
		DWORD dwPos;	//	??w????????~
		//	?????K?????????
		if (!this->sinfo.sec_ptr)
		{
			return;
		}
		//	?????????w?????????
		if (!this->sinfo.bStream)
		{
			return;
		}
		//	???????w??????????
		this->sinfo.sec_ptr->GetStatus(&dwStatus);
		if (!(dwStatus & DSBSTATUS_PLAYING))
		{
			return;
		}
		//	???????????????????
		this->sinfo.sec_ptr->GetCurrentPosition(&dwPos, NULL);
		//	??w?????????0????0-11025??22050-44100???????????
		if ((this->sinfo.iStreamPos == 0) && (dwPos > 0) && (dwPos < (dwBufferSize_ / 2)))
		{
			this->LoadStream(dwBufferSize_, 1);
		}
		//	??w?????????1????22050-33075??0-22050?????????
		if ((this->sinfo.iStreamPos == 1) && (dwPos > dwBufferSize_))
		{
			this->LoadStream(0, 0);
		}
		//	????????
		if (this->sinfo.effectflag == Effect::FADEOUT)
		{
			this->sinfo.iVolume -= this->sinfo.iFade;//kVALUE_FADE;
			if (this->sinfo.iVolume < kVOLUME_MIN)
			{
				this->Stop();
				this->sinfo.effectflag = Effect::NOTHING;
			}
			this->SetVolume(this->sinfo.iVolume);
		}
		if (this->sinfo.effectflag == Effect::FADEIN)
		{
			this->sinfo.iVolume += this->sinfo.iFade;//kVALUE_FADE;
			if (this->sinfo.iVolume > DSBVOLUME_MAX)
			{
				this->sinfo.iVolume = DSBVOLUME_MAX;
				this->sinfo.effectflag = Effect::NOTHING;
			}
			this->SetVolume(this->sinfo.iVolume);
		}
		if (this->sinfo.effectflag == Effect::STOP)
		{
			this->Stop( );
			this->sinfo.effectflag = Effect::NOTHING;
		}
	}



	//--------------------------------------------------------------------------------------------
	//	???		?	GetErrorMessage
	//	??m??		?	???s??
	//	??m?^		?	2002.06.24
	//	???		?	????????????????
	//	?????	?	?number???p????????szMsg????????
	//					?iLength???????????????????k??????
	//	??		?	??`??number		?r???????????
	//					??`??szMsg		????????~??
	//					??`??iLength		??~??????
	//	????	?	char szWork		??????????~??
	//	????	?	strcpy_s	??????
	//					memcpy			??????
	//	????		?	?????
	//	?????	?	2002.06.24	Ver1.00		???s??	???????
	//					2007.02.09	Ver1.01		???s??	????????
	//--------------------------------------------------------------------------------------------

	std::string GetErrorMessage(Err::Type number)
	{
		std::string szMsg;

		switch (number)
		{
		case Err::CCIM: szMsg = "DirectMusic???m????"; break;
		case Err::CCIS: szMsg = "DirectSound???m????"; break;
		case Err::INIT: szMsg = "Initialize????"; break;
		case Err::SCL: szMsg = "SetCooperativeLevel????"; break;
		case Err::CSB: szMsg = "??????m????"; break;
		case Err::QI: szMsg = "QueryInterface????"; break;
		case Err::SFMT: szMsg = "?????????k????"; break;
		case Err::IA: szMsg = "InitAudio????"; break;
		case Err::CCILOADER: szMsg = "??????m????"; break;
		case Err::MBTWC: szMsg = "unicode??????"; break;
		case Err::SSD: szMsg = "?????????k????"; break;
		case Err::PSE: szMsg = "???????????w????"; break;
		case Err::INVALID: szMsg = "????????????"; break;
		case Err::DL: szMsg = "??????????"; break;
		case Err::UNLD: szMsg = "?????????"; break;
		case Err::INVOPEN: szMsg = "?????????"; break;
		case Err::RIFF: szMsg = "RIFF????????????"; break;
		case Err::FMT: szMsg = "fmt????????????"; break;
		case Err::NOTPCM: szMsg = "??????p???WAVE?????????????"; break;
		case Err::DATA: szMsg = "data??????X????"; break;
		case Err::READERROR: szMsg = "????????"; break;
		case Err::NOTHING: szMsg = "?????????"; break;
		default: szMsg = "?k??????????"; break;
		}
		return szMsg;
	}
	//-----------------------------------------------------------------------
	//Mouse?
	//-----------------------------------------------------------------------
	map<string, DM::Sound::SP>	SoundManager::arr;
	bool SoundManager::notObject;
	void  SoundManager::Set(const string name_, DM::Sound::SP sp_)
	{
		arr[name_] = sp_;
	}
	//-----------------------------------------------------------------------
	DM::Sound::SP  SoundManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("SoundManager ?p????X?????"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  SoundManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void SoundManager::AllClear()
	{
		arr.clear();
	}
	//---------------------------------------------------------------------------
}

