#include "XI2018_Ver1_0.h"


namespace XI
{
	const  float  deadzone = 0.24f;
	const  int    analogMax = 32768;
	const  float  digToAnalog = 1.0f / analogMax;
	const  float  digToAnalog_TG = 1.0f / 255;

	Obj::WP Obj::winst;	//	????÷X???????????
	vector<GamePad::WP>			Obj::gpads;
	vector<Mouse::WP>			Obj::mouses;

	class  Pad : public GamePad
	{
	public:
		using SP = shared_ptr<Pad>;
		using WP = weak_ptr<Pad>;

	private:
		XINPUT_VIBRATION vibration;//????õÏ?
		int	leftVibFrame;
		int	rightVibFrame;
		bool connected;//??????????
		DWORD  pad_ID;

		Pad(void);

		struct KeyParam {
			RGP		gKey;	//??????????
			VGP       pKey;	//????ôí????õÄ???õ­?
			ButtonState		state;	//???õÔ÷v
		};
		vector<KeyParam>     buttons;
	private:
		bool  Initialize(
			const DWORD&				id_,//0?3??
			const KeyDatas_GP&			buttonGP_);

		bool checkPush(const VGP  bt_, const DWORD  state_);
	protected:
	public:
		ButtonState  CheckBT(VGP		kk_);
		//~Pad();
		//	?õö
		void  B_UpDate();
		//	?????öwöm??
		static  SP  Create(
			const DWORD&				id_,//0?3??
			const KeyDatas_GP&			buttonGP_);
		void  SetVibrationL(float  pow_, int  frame_);
		void  SetVibrationR(float  pow_, int  frame_);
	};

	class  KeyBoard : public GamePad
	{
	public:
		using SP = shared_ptr<KeyBoard>;
		using WP = weak_ptr<KeyBoard>;


	private:
		IDirectInputDevice8*  device;
		bool connected;//??????????
		KeyBoard(void);

		struct KeyParam {
			int					vKey;	//?????????????
			VGP					pKey;	//????ôí????õÄ???õ­?
			ButtonState			state;	//???õÔ÷v
		};
		vector<KeyParam>     buttons;
		vector<DWORD>         kbAxisL;		//??????÷p???
		vector<DWORD>         kbAxisR;		//??????÷p???
		vector<DWORD>         kbTrigger;	//??????÷p???



	private:
		bool  Initialize(
			const AnalogAxisKB&			axisL_,
			const AnalogAxisKB&			axisR_,
			const AnalogTriggerKB&		trigger_,
			const KeyDatas_KB&			buttonKB_);

	protected:

	public:
		ButtonState  CheckBT(VGP		kk_);
		//~KeyBoard();
		//	?????öwöm??
		static  SP  Create(
			const AnalogAxisKB&			axisL_,
			const AnalogAxisKB&			axisR_,
			const AnalogTriggerKB&		trigger_,
			const KeyDatas_KB&			buttonKB_);

		//	?õö
		void  B_UpDate();
	};


	Obj::SP Obj::GetInst()
	{
		if (!winst.expired()) {
			return winst.lock();
		}
		return nullptr;
	}
	Obj::~Obj( )
	{
		//	DI?????????
		ML::SafeRelease(&di);
	}
	Obj::Obj()
		:wnd(0)
		,di(nullptr)
	{
		this->gpads.clear();
		this->mouses.clear();
	}
	Obj::Obj(const Obj&)
		:wnd(0)
		, di(nullptr)
	{
		this->gpads.clear();
		this->mouses.clear();
	}
	Obj::Obj(const Obj&&)
		:wnd(0)
		, di(nullptr)
	{
		this->gpads.clear();
		this->mouses.clear();
	}
	void Obj::Set(GamePad::SP s_)
	{
		gpads.push_back(s_);
	}
	void Obj::Set(Mouse::SP s_)
	{
		mouses.push_back(s_);
	}
	//???????õq?
	void Obj::UpDate()
	{
		{
			class cNullCheckGP {
			public:
				// ???????????????true???
				bool operator()(GamePad::WP t_) const {
					bool rtv = t_.expired();
					return  rtv;
				}
			};
			auto endIt = remove_if(this->gpads.begin(), this->gpads.end(), cNullCheckGP());
			this->gpads.erase(endIt, this->gpads.end());	//??õ¢ô¸???????öÔ??????
			for (auto it = gpads.begin(); it != gpads.end(); ++it) {
				auto  sp = it->lock();
				sp->UpDate();
			}
		}
		class cNullCheckM {
		public:
			// ???????????????true???
			bool operator()(Mouse::WP t_) const {
				bool rtv = t_.expired();
				return  rtv;
			}
		};
		auto endIt = remove_if(this->mouses.begin(), this->mouses.end(), cNullCheckM());
		this->mouses.erase(endIt, this->mouses.end());	//??õ¢ô¸???????öÔ??????
		for (auto it = mouses.begin(); it != mouses.end(); ++it) {
			auto  sp = it->lock();
			sp->UpDate();
		}
	}
	bool Obj::Initialize(HWND wnd_)
	{
		wnd = wnd_;
		if (FAILED(DirectInput8Create(GetModuleHandle(nullptr),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(VOID**)&di,
			nullptr)))
		{
			//	????öwömô¸ù—
			return false;
		}
		return true;
	}
	//	DI??????öwöm??
	Obj::SP Obj::Create(HWND		hw_)	//	?????????
	{
		if (winst.expired()) {
			Obj::SP sp = Obj::SP(new Obj());
			if (sp->Initialize(hw_)) {
				winst = sp;
				return sp;
			}
		}
		return 0;
	}



	//GamePad------------------------------------------------------------------------
	GamePad::GamePad(	)
	{
	}
	GamePad::SP GamePad::CreateKB(
		const AnalogAxisKB&			axisL_,
		const AnalogAxisKB&			axisR_,
		const AnalogTriggerKB&		trigger_,
		const KeyDatas_KB&			buttonKB_)
	{
		return  KeyBoard::Create(axisL_, axisR_, trigger_, buttonKB_);
	}
	GamePad::SP GamePad::CreateGP(
		const DWORD&				id_,//0?3??
		const KeyDatas_GP&			buttonGP_)
	{
		return  Pad::Create(id_, buttonGP_);
	}
	//	?õö
	void GamePad::UpDate()
	{
		this->B_UpDate();
	}
	void  GamePad::SetVibrationL(float  pow_, int  frame_)
	{
		if (this->link != nullptr) {
			link->SetVibrationL(pow_, frame_);
		}
	};
	void  GamePad::SetVibrationR(float  pow_, int  frame_)
	{
		if (this->link != nullptr) {
			link->SetVibrationR(pow_, frame_);
		}
	};

	GamePad::ButtonState  GamePad::UpdateButtonState(bool  nbs_, ButtonState  old_)
	{
		if (nbs_) {
			if (old_ == ButtonState::Err) { old_ = ButtonState::Off; }
			//??õÔ÷v(???)
			switch (old_) {
			case ButtonState::Off:
			case ButtonState::Up:
				old_ = ButtonState::Down;	break;	//	?????&???÷ü????
			case ButtonState::On:
			case ButtonState::Down:
				old_ = ButtonState::On;	break;	//	?????&???÷ü????
			}
		}
		else {
			//??õÔ÷v(??)
			switch (old_) {
			case ButtonState::Off:
			case ButtonState::Up:
				old_ = ButtonState::Off;	break;	//	?????&???÷ü????
			case ButtonState::On:
			case ButtonState::Down:
				old_ = ButtonState::Up;	break;	//	?????&???÷ü????
			}
		}
		return old_;
	}
	void  GamePad::SIUpdate(Stick&  si_, float  ax_, float  ay_)
	{
		si_.axis.x = ax_;
		si_.axis.y = ay_;

		//ù`???õŽ??õË???????????
		if (-deadzone < si_.axis.x  &&  si_.axis.x < deadzone &&
			-deadzone < si_.axis.y  &&  si_.axis.y < deadzone) {
			si_.axis.x = 0;
			si_.axis.y = 0;
		}
		//?????????????????
		si_.BL = UpdateButtonState(ax_ < -deadzone, si_.BL);
		si_.BR = UpdateButtonState(ax_ > +deadzone, si_.BR);
		si_.BD = UpdateButtonState(ay_ < -deadzone, si_.BD);
		si_.BU = UpdateButtonState(ay_ > +deadzone, si_.BU);
	}
	void  GamePad::TGUpdate(Trigger&  tg_, float  lt_, float  rt_)
	{
		tg_.volumeL = lt_;
		tg_.volumeR = rt_;
		//ù`???õŽ??õË???????????
		if (-deadzone < tg_.volumeL  &&  tg_.volumeL < deadzone &&
			-deadzone < tg_.volumeR  &&  tg_.volumeR < deadzone) {
			tg_.volumeL = 0;
			tg_.volumeR = 0;
		}
		tg_.L2 = UpdateButtonState(lt_ > deadzone, tg_.L2);
		tg_.R2 = UpdateButtonState(rt_ > deadzone, tg_.R2);
	}


	GamePad::SP GamePad::Link(GamePad::SP a, GamePad::SP b)
	{
	//????????????ô‰???????
		if(a != 0 && b != 0){	a->Link(b);	}	//??????????????
		else if(a != 0){					}	//?????????????B2
		else if(b != 0){		a = b;		}	//?????B2????????
		else{								}	//?????B2???????B2
		return a;
	}

	void  GamePad::ConvertSI(StickInfo&  si_, Stick&  s_)
	{
		si_.axis.x = s_.axis.x;
		si_.axis.y = s_.axis.y;

		//?ø»?÷÷???????????????ù`?
		if (si_.axis.x != 0 || si_.axis.y != 0) {//???????????õq???
			//??0ø»???90ø»????ø»????
			si_.angleDYP = atan2(-si_.axis.y, si_.axis.x);//?????ø»?ô{õq
			//??0ø»?õÄ?90ø»????ø»????
			si_.angleUYP = atan2(si_.axis.y, si_.axis.x);//?????ø»?ô{õq

			//?????????0?1?÷×????
			si_.volume = sqrt(si_.axis.x * si_.axis.x + si_.axis.y * si_.axis.y);
			si_.volume = min(si_.volume, 1.0f);
		}
		else {
			si_.volume = 0.0f;
		}
		si_.BL = this->ConvertKS(s_.BL);
		si_.BR = this->ConvertKS(s_.BR);
		si_.BD = this->ConvertKS(s_.BD);
		si_.BU = this->ConvertKS(s_.BU);
	}
	void  GamePad::ConvertTG(TriggerInfo&  ti_, Trigger&  t_)
	{
		ti_.volumeL = t_.volumeL;
		ti_.volumeR = t_.volumeR;
		ti_.L2 = this->ConvertKS(t_.L2);
		ti_.R2 = this->ConvertKS(t_.R2);
	}

	StickInfo  GamePad::StickL()
	{
		StickInfo  me;
		ConvertSI(me, this->ls);
		StickInfo  linkRtv;
		if (link != nullptr) {
			linkRtv = link->StickL();
			if (me.volume < linkRtv.volume) {
				return linkRtv;
			}
		}
		return me;
	}
	StickInfo  GamePad::StickR()
	{
		StickInfo  me;
		ConvertSI(me, this->rs);
		StickInfo  linkRtv;
		if (link != nullptr) {
			linkRtv = link->StickR();
			if (me.volume < linkRtv.volume) {
				return linkRtv;
			}
		}
		return me;
	}
	TriggerInfo  GamePad::TriggerLR()
	{
		TriggerInfo  me;
		ConvertTG(me, this->tg);
		TriggerInfo  linkRtv;
		if (link != nullptr) {
			linkRtv = link->TriggerLR();
			if (me.volumeL < linkRtv.volumeL) {
				me.volumeL = linkRtv.volumeL;
			}
			if (me.volumeR < linkRtv.volumeR) {
				me.volumeR = linkRtv.volumeR;
			}
		}
		return me;
	}



	ButtonInfo  GamePad::ConvertKS(ButtonState  ks_)
	{
		ButtonInfo  rtv;
		switch (ks_) {
		default:
		case XI::GamePad::ButtonState::Off:
			rtv.SetOff();
			break;
		case XI::GamePad::ButtonState::Down:
			rtv.SetDown();
			break;
		case XI::GamePad::ButtonState::On:
			rtv.SetOn();
			break;
		case XI::GamePad::ButtonState::Up:
			rtv.SetUp();
			break;
		}
		return rtv;
	}
	//????????????÷gø¿?õÔ÷v???
	GamePad::ButtonState  GamePad::IntegrationKey(ButtonState  a_, ButtonState  b_)
	{
		using BS = ButtonState;
		BS ret = BS::Off;

		BS  mat[5][5] = {
			//Err       On      Off       Down      Up
			{ BS::Err,  BS::On, BS::Off,  BS::Down, BS::Up}, //Err
			{ BS::On,   BS::On, BS::On,   BS::On,   BS::On}, //On
			{ BS::Off,  BS::On, BS::Off,  BS::Down, BS::Up}, //Off
			{ BS::Down, BS::On, BS::Down, BS::Down, BS::On}, //Down
			{ BS::Up,   BS::On, BS::Up,   BS::On,   BS::Up}, //Up
		};
		return  mat[(BYTE)a_][(BYTE)b_];
	}

	VGamePad  GamePad::GetState()
	{
		VGamePad  rtv;
		rtv.LStick = this->StickL();
		rtv.RStick = this->StickR();
		rtv.Trigger = this->TriggerLR();
		rtv.B1 = ConvertKS(this->CheckBT(XI::VGP::B1));
		rtv.B2 = ConvertKS(this->CheckBT(XI::VGP::B2));
		rtv.B3 = ConvertKS(this->CheckBT(XI::VGP::B3));
		rtv.B4 = ConvertKS(this->CheckBT(XI::VGP::B4));

		rtv.L1 = ConvertKS(this->CheckBT(XI::VGP::L1));
		rtv.R1 = ConvertKS(this->CheckBT(XI::VGP::R1));

		rtv.ST = ConvertKS(this->CheckBT(XI::VGP::ST));
		rtv.SE = ConvertKS(this->CheckBT(XI::VGP::SE));

		rtv.L3 = ConvertKS(this->CheckBT(XI::VGP::L3));
		rtv.R3 = ConvertKS(this->CheckBT(XI::VGP::R3));

		rtv.HL = ConvertKS(this->CheckBT(XI::VGP::HL));
		rtv.HR = ConvertKS(this->CheckBT(XI::VGP::HR));
		rtv.HU = ConvertKS(this->CheckBT(XI::VGP::HU));
		rtv.HD = ConvertKS(this->CheckBT(XI::VGP::HD));

		rtv.S0 = ConvertKS(this->CheckBT(XI::VGP::S0));
		rtv.S1 = ConvertKS(this->CheckBT(XI::VGP::S1));
		rtv.S2 = ConvertKS(this->CheckBT(XI::VGP::S2));
		rtv.S3 = ConvertKS(this->CheckBT(XI::VGP::S3));
		rtv.S4 = ConvertKS(this->CheckBT(XI::VGP::S4));
		rtv.S5 = ConvertKS(this->CheckBT(XI::VGP::S5));
		rtv.S6 = ConvertKS(this->CheckBT(XI::VGP::S6));
		rtv.S7 = ConvertKS(this->CheckBT(XI::VGP::S7));
		rtv.S8 = ConvertKS(this->CheckBT(XI::VGP::S8));
		rtv.S9 = ConvertKS(this->CheckBT(XI::VGP::S9));
		return rtv;
	}
	//Pad------------------------------------------------------------------------
	Pad::Pad(void)
		:vibration()
		,leftVibFrame(0)
		,rightVibFrame(0)
	{
	}
	bool Pad::Initialize(
		const DWORD&				id_,//0?3??
		const KeyDatas_GP&			buttonGP_)
	{
		this->pad_ID = id_;
		for(DWORD c = 0;  c < buttonGP_.size(); c++)
		{
			KeyParam w = { buttonGP_[c].gKey, buttonGP_[c].pKey, ButtonState::Off };
			this->buttons.push_back(w);
		}

		return true;
	}
	//	?????öwöm??
	Pad::SP  Pad::Create(
		const DWORD&				id_,//0?3??
		const KeyDatas_GP&			buttonGP_)
	{
		auto sp = SP(new Pad());
		if (sp) {
			if (sp->Initialize(id_, buttonGP_))
			{
				if (sp) {
					auto  ob = Obj::GetInst();
					ob->Set(sp);
				}
				return sp;
			}
		}
		return 0;
	}
	//??????????
	bool Pad::checkPush(const VGP  bt_, const DWORD  state_)
	{
		int  bt = (int)bt_;
		//                   B1, B2, B3, B4, L1, R1, SE, ST, L3, R3, HL, HR, HU, HD,
		const int  btc[] = { 12, 13, 14, 15,  8,  9,  5,  4,  6,  7,  2,  3,  0,  1 };
		if ((int)bt >= _countof(btc)) { return false; }
		DWORD  bit = 1 << btc[bt];
		return  (state_ & bit) ? true : false;
	}
	GamePad::ButtonState	 Pad::CheckBT(VGP		kk_)
	{
		ButtonState ret = ButtonState::Off;

		//??ùž?????÷põ¼???????õq??
		for (auto it = begin(buttons); it != end(buttons); ++it)
		{
			if (kk_ == it->pKey)		//ø²??????????????????õq??
			{
				ret = IntegrationKey(ret, (ButtonState)it->state);
			}
		}
		if (link != nullptr) {
			ret = IntegrationKey(ret, link->CheckBT(kk_));
		}
		return ret;
	}
	void  Pad::SetVibrationL(float  pow_, int  frame_)
	{
		if (this->link != nullptr) {
			link->SetVibrationL(pow_, frame_);
		}

		this->vibration.wLeftMotorSpeed = static_cast<WORD>(65535.0f * pow_);
		XInputSetState(this->pad_ID, &this->vibration);
		this->leftVibFrame = frame_;
	}
	void  Pad::SetVibrationR(float  pow_, int  frame_)
	{
		if (this->link != nullptr) {
			link->SetVibrationR(pow_, frame_);
		}

		this->vibration.wRightMotorSpeed = static_cast<WORD>(65535.0f * pow_);
		XInputSetState(this->pad_ID, &this->vibration);
		this->rightVibFrame = frame_;
	}

	

	void  Pad::B_UpDate()
	{
		XINPUT_STATE	state;//???õÏ?

		//????õÔ÷v?÷ò??????????ù´??????
		if (XInputGetState(this->pad_ID, &state) != ERROR_SUCCESS)
		{
			connected = false;
			return;
		}
		DWORD  btstate = state.Gamepad.wButtons;

		//?????÷p?
		bool keyFlag;
		for (auto it = begin(buttons); it != end(buttons); ++it)
		{
			//??????÷p?????????????
			keyFlag = false;
			//POV÷p?
			if (     it->gKey == RGP::HSL) { keyFlag = this->checkPush(VGP::HL, btstate); }
			else if (it->gKey == RGP::HSR) { keyFlag = this->checkPush(VGP::HR, btstate); }
			else if (it->gKey == RGP::HSU) { keyFlag = this->checkPush(VGP::HU, btstate); }
			else if (it->gKey == RGP::HSD) { keyFlag = this->checkPush(VGP::HD, btstate); }
			//????????
			else if (it->gKey >= RGP::B01 && it->gKey <= RGP::B16) {
				keyFlag = this->checkPush(static_cast<VGP>(it->gKey), btstate);
			}
			//?÷p?????öŸøk?????
			else { continue; }
			it->state = UpdateButtonState(keyFlag, it->state);
		}

		//?÷ˆ÷Î???
		if (state.Gamepad.sThumbLX == -32768) { state.Gamepad.sThumbLX = -32767; }
		if (state.Gamepad.sThumbLY == -32768) { state.Gamepad.sThumbLY = -32767; }


		//?????????÷p?
		SIUpdate(this->ls, state.Gamepad.sThumbLX * digToAnalog, state.Gamepad.sThumbLY  * digToAnalog);
		SIUpdate(this->rs, state.Gamepad.sThumbRX * digToAnalog, state.Gamepad.sThumbRY  * digToAnalog);
		TGUpdate(this->tg,	state.Gamepad.bLeftTrigger * digToAnalog_TG,
							state.Gamepad.bRightTrigger * digToAnalog_TG);

		//????????øeô 
		if (this->leftVibFrame > 0) {
			this->leftVibFrame--;
			if (this->leftVibFrame == 0) {
				this->vibration.wLeftMotorSpeed = 0;
				XInputSetState(this->pad_ID, &this->vibration);
			}
		}
		if (this->rightVibFrame > 0) {
			this->rightVibFrame--;
			if (this->rightVibFrame == 0) {
				this->vibration.wRightMotorSpeed = 0;
				XInputSetState(this->pad_ID, &this->vibration);
			}
		}
	}
	//KeyBoard------------------------------------------------------------------------
	KeyBoard::KeyBoard(void)
	{

	}
	//	?????öwöm??
	KeyBoard::SP  KeyBoard::Create(
		const AnalogAxisKB&			axisL_,
		const AnalogAxisKB&			axisR_,
		const AnalogTriggerKB&		trigger_,
		const KeyDatas_KB&			buttonKB_)
	{
		auto sp = SP(new KeyBoard());
		if (sp) {
			if (sp->Initialize(axisL_, axisR_, trigger_, buttonKB_))
			{
				if (sp) {
					auto  ob = Obj::GetInst();
					ob->Set(sp);
				}
				return sp;
			}
		}
		return 0;
	}
	static GUID					a_guid;
	static int					a_id;
	BOOL CALLBACK diEnumDevice(const DIDEVICEINSTANCE* inDevice,
		void* inContext)
	{
		//????ùÖ?
		DWORD* num = (DWORD*)inContext;
		//ô™øk???ùÖ????????÷¤
		if ((*num) == a_id)
		{
			a_guid = inDevice->guidInstance;	//??????÷X
			(*num)++;
			return DIENUM_STOP;					//????????õK???
		}
		else
		{
			(*num)++;
			return DIENUM_CONTINUE;			//??????????????
		}
	}

	bool KeyBoard::Initialize(
		const AnalogAxisKB&			axisL_,
		const AnalogAxisKB&			axisR_,
		const AnalogTriggerKB&		trigger_,
		const KeyDatas_KB&			buttonKB_)
	{
		Obj::SP dii = Obj::GetInst();
		if (dii == 0) { return false; }

		struct {
			DWORD				dType;
			LPCDIDATAFORMAT		dFormat;
			DWORD				cLevel;

		}kind_KeyBoard = { DI8DEVCLASS_KEYBOARD, &c_dfDIKeyboard,  DISCL_NONEXCLUSIVE | DISCL_FOREGROUND };
		
		//??????õq???----------------------------------------------------------
		//ô™øk?????õq?????????a_guid?????a_id?????
		DWORD deviceCount = 0;
		a_id = 0;
		if (FAILED(dii->DInput().EnumDevices(
			kind_KeyBoard.dType, diEnumDevice,
			&deviceCount, DIEDFL_ATTACHEDONLY))) {
			return false;
		}
		//ù`??????öwöm??
		if (FAILED(dii->DInput().CreateDevice(a_guid, &device, nullptr))) { return false; }
		//ù`?????????????öŸøk??
		if (FAILED(device->SetDataFormat(kind_KeyBoard.dFormat))) { return false; }
		//ù`???????÷ò????öŸøk??
		if (FAILED(device->SetCooperativeLevel(dii->Wnd(), kind_KeyBoard.cLevel))) { return false; }

		DIPROPDWORD dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = 8;//øÐ??ù½øk??????????????
		if (FAILED(device->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
		{
			return false;
		}

		//----------------------------------------------------------
		for (DWORD c = 0; c < buttonKB_.size(); c++)
		{
			KeyParam w = { buttonKB_[c].vKey, buttonKB_[c].pKey, ButtonState::Off };
			this->buttons.push_back(w);
		}
		//??ø²?
		kbAxisL.push_back(axisL_.vKeyL);
		kbAxisL.push_back(axisL_.vKeyR);
		kbAxisL.push_back(axisL_.vKeyU);
		kbAxisL.push_back(axisL_.vKeyD);

		kbAxisR.push_back(axisR_.vKeyL);
		kbAxisR.push_back(axisR_.vKeyR);
		kbAxisR.push_back(axisR_.vKeyU);
		kbAxisR.push_back(axisR_.vKeyD);

		kbTrigger.push_back(trigger_.vKeyL);
		kbTrigger.push_back(trigger_.vKeyR);
		return true;
	}
	GamePad::ButtonState	 KeyBoard::CheckBT(VGP		kk_)
	{
		ButtonState ret = ButtonState::Off;

		//??ùž?????÷põ¼???????õq??
		for (auto it = begin(buttons); it != end(buttons); ++it)
		{
			if (kk_ == it->pKey)		//ø²??????????????????õq??
			{
				ret = IntegrationKey(ret, (ButtonState)it->state);
			}
		}
		if (link != nullptr) {
			ret = IntegrationKey(ret, link->CheckBT(kk_));
		}
		return ret;
	}
	void  KeyBoard::B_UpDate()
	{
		BYTE keyStatus[256] = { 0 };
		//?????ô‰??ù€??ùi??
		int acquireCnt = 0;
		BOOL brFlag = FALSE;
		if (FAILED(device->Poll()))
		{
			do
			{
				if (SUCCEEDED(device->Acquire())) { brFlag = TRUE; }
				if (++acquireCnt > 30) { return; }
			} while (brFlag == FALSE);
		}
		//	?????õÏ?ôæøß
		if (FAILED(device->GetDeviceState(sizeof(BYTE) * 256, &keyStatus))) { return; }
		//	??ù`??ø²????????÷p?
		float  ax_L = 0, ay_L = 0;
		if (kbAxisL.size() == 4) {
			if (keyStatus[kbAxisL[0]] != 0) { ax_L -= 1; }
			if (keyStatus[kbAxisL[1]] != 0) { ax_L += 1; }
			if (keyStatus[kbAxisL[2]] != 0) { ay_L += 1; }
			if (keyStatus[kbAxisL[3]] != 0) { ay_L -= 1; }
		}
		float  ax_R = 0, ay_R = 0;
		if (kbAxisR.size() == 4) {
			if (keyStatus[kbAxisR[0]] != 0) { ax_R -= 1; }
			if (keyStatus[kbAxisR[1]] != 0) { ax_R += 1; }
			if (keyStatus[kbAxisR[2]] != 0) { ay_R += 1; }
			if (keyStatus[kbAxisR[3]] != 0) { ay_R -= 1; }
		}
		//??????
		float t_L2=0, t_R2=0;
		if (keyStatus[this->kbTrigger[0]] != 0) { t_L2 += 1; }
		if (keyStatus[this->kbTrigger[1]] != 0) { t_R2 += 1; }


		//LR?????õÏ??õö
		SIUpdate(ls, ax_L, ay_L);
		SIUpdate(rs, ax_R, ay_R);
		//??
		TGUpdate(tg, t_L2, t_R2);

		//???????ø²????????÷p?
		for (auto it = begin(buttons); it != end(buttons); ++it) {
			it->state = UpdateButtonState(keyStatus[it->vKey] != 0, it->state);
		}
	}

	//Mouse------------------------------------------------------------------------
	Err  Mouse::init(float scaleX_, float scaleY_)
	{
		Obj::SP dii = Obj::GetInst();
		if (dii == 0){ return Err::NOTDEVICE; }

		//--------------------------------
		//DirectMouse
		if (FAILED(dii->DInput().CreateDevice(GUID_SysMouse, &mouse, nullptr))){ return Err::CREATEDEVICE; }
		if (FAILED(mouse->SetDataFormat(&c_dfDIMouse2))){ return Err::DATAFORMAT; } // ????????????????öŸøk
		mouse->SetCooperativeLevel(dii->Wnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);//???öŸøk
		// ?????öŸøk    
		DIPROPDWORD diprop;
		diprop.diph.dwSize = sizeof(diprop);
		diprop.diph.dwHeaderSize = sizeof(diprop.diph);
		diprop.diph.dwObj = 0;
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.dwData = DIPROPAXISMODE_REL; // ?÷p÷Î???DIPROPAXISMODE_REL?öŸøk?ö¤÷p÷Î?DIPROPAXISMODE_ABS?    
		mouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);

		// ù`?öh??ô    
		mouse->Acquire();
		//--------------------------------
		scaleX = scaleX_;
		scaleY = scaleY_;

		return Err::NOTHING;
	}

	Mouse::SP Mouse::Create(float scaleX_, float scaleY_)
	{
		auto sp = SP(new Mouse());
		if (sp){
			if (sp->init(scaleX_, scaleY_) != Err::NOTHING){
				MessageBox(nullptr, _T("???????????"), nullptr, MB_OK);
				return nullptr;
			}
			else{
				auto  ob = Obj::GetInst();
				ob->Set(sp);
			}
		}
		return sp;
	}
	Mouse::Mouse()
	{
	}

	Mouse::~Mouse()
	{
		if (mouse != NULL)
		{
			mouse->Unacquire();
			ML::SafeRelease(&mouse);
		}
	}

	void Mouse::UpDate()
	{
		//--------------------------------
		//add koyanagi
		// ÷Î??õö
		ZeroMemory(&mouseState, sizeof(DIMOUSESTATE2));
		if (FAILED(mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState))){
			mouse->Acquire();
		}
		//???????ôæøß
		POINT  wp = { LONG(this->pos_Screen.x) , LONG(this->pos_Screen.y) };
		GetCursorPos(&wp);
		//????????ô{õq
		this->pos_Screen = {int(wp.x),int(wp.y)};
		Obj::SP dii = Obj::GetInst();
		ScreenToClient(dii->Wnd(), &wp);
		this->pos = { int(wp.x),int(wp.y) };

		this->pos.x = int(this->pos.x / scaleX);
		this->pos.y = int(this->pos.y / scaleY);
		//????
		mouseWheel = mouseState.lZ;
		//????
		for (int i = 0; i < (int)MB::kMax; i++)
		{
			if ((mouseState.rgbButtons[i] & 0x80) >> 7){
				if (mouseButton[i] == ButtonState::Off || mouseButton[i] == ButtonState::Up)  mouseButton[i] = ButtonState::Down;
				else  mouseButton[i] = ButtonState::On;
			}
			else{
				if (mouseButton[i] == ButtonState::On || mouseButton[i] == ButtonState::Down)  mouseButton[i] = ButtonState::Up;
				else  mouseButton[i] = ButtonState::Off;
			}
		}
		//--------------------------------
	}
	ButtonInfo  Mouse::ConvertKS(XI::Mouse::ButtonState  ks_)
	{
		ButtonInfo  rtv;
		switch (ks_) {
		default:
		case XI::Mouse::ButtonState::Off:
			rtv.SetOff();
			break;
		case XI::Mouse::ButtonState::Down:
			rtv.SetDown();
			break;
		case XI::Mouse::ButtonState::On:
			rtv.SetOn();
			break;
		case XI::Mouse::ButtonState::Up:
			rtv.SetUp();
			break;
		}
		return rtv;
	}
	//
	VMouse  Mouse::GetState()
	{
		VMouse  rtv;
		rtv.pos = this->pos;
		rtv.pos_Screen = this->pos_Screen;
		rtv.wheel = this->mouseWheel;
		rtv.LB = ConvertKS(this->CheckBT(Mouse::MB::LB));
		rtv.RB = ConvertKS(this->CheckBT(Mouse::MB::RB));
		rtv.CB = ConvertKS(this->CheckBT(Mouse::MB::CB));

		rtv.B1 = ConvertKS(this->CheckBT(Mouse::MB::RESERVED1));
		rtv.B2 = ConvertKS(this->CheckBT(Mouse::MB::RESERVED2));
		rtv.B3 = ConvertKS(this->CheckBT(Mouse::MB::RESERVED3));
		rtv.B4 = ConvertKS(this->CheckBT(Mouse::MB::RESERVED4));
		rtv.B5 = ConvertKS(this->CheckBT(Mouse::MB::RESERVED5));
		return  rtv;
	}
	Mouse::ButtonState Mouse::CheckBT(Mouse::MB  mb_)
	{
		return mouseButton[(int)mb_];
	}

	//-----------------------------------------------------------------------
	//GamePad?
	//-----------------------------------------------------------------------
	map<string, XI::GamePad::SP>	GamePadManager::arr;
	bool GamePadManager::notObject;
	void  GamePadManager::Set(const string& name_, XI::GamePad::SP sp_)
	{
		arr[name_] = sp_;
	}
	//-----------------------------------------------------------------------
	XI::GamePad::SP  GamePadManager::Get(const string& name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("GamePadManager ÷põ¼?÷X?????"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  GamePadManager::Clear(const string& name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void GamePadManager::AllClear()
	{
		arr.clear();
	}
	//-----------------------------------------------------------------------
	//Mouse?
	//-----------------------------------------------------------------------
	map<string, XI::Mouse::SP>	MouseManager::arr;
	bool MouseManager::notObject;
	void  MouseManager::Set(const string& name_, XI::Mouse::SP sp_)
	{
		arr[name_] = sp_;
	}
	//-----------------------------------------------------------------------
	XI::Mouse::SP  MouseManager::Get(const string& name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("MouseManager ÷põ¼?÷X?????"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  MouseManager::Clear(const string& name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void MouseManager::AllClear()
	{
		arr.clear();
	}
	//---------------------------------------------------------------------------
}

