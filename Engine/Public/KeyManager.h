#pragma once
#include "Base.h"

BEGIN(Engine)

class CKeyManager final : public CBase
{
public:
	typedef struct
	{
		KEY_STATE	eState;  // 키의 상태값
		bool		bPrev;   // 이전 프레임에서 눌렸는지 안눌렸는지 
	}KEYINFO;

private:
	CKeyManager();
	virtual ~CKeyManager() = default;

public:
	KEY_STATE	Get_KeyState(KEY _eKey) { return m_vecKey[(int)_eKey].eState; }

public:
	HRESULT Initialize(HWND hWnd);
	void Update();

private:
	HWND				m_hWnd = {};

	vector<KEYINFO>		m_vecKey;

public:
	static CKeyManager* Create(HWND hWnd);
	virtual void Free() override;

};

END