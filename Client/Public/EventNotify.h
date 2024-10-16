#pragma once
#include "Client_Defines.h"
#include "UIObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CEventNotify final : public CUIObject
{
public:
	enum TEXT_EVENT
	{
		UNABLE_SKILL_COOLTIME,
		UNABLE_CUTALL,
		UNABLE_MINDCONTROL,
		UNABLE_HOMING_SH,
		UNABLE_SWAPWEAPON,
		TEXT_EVENT_END
	};




private:
	CEventNotify(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEventNotify(const CEventNotify& Prototype);
	virtual ~CEventNotify() = default;

public:
	void		Set_Active(_bool b , TEXT_EVENT eType ) 
	{ 
		m_bActivate = b;
		m_eCurEvent = eType;
	}


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	TEXT_EVENT			m_eCurEvent = { TEXT_EVENT_END };
	_float				m_fUnableTime = { 2.f };
	_bool				m_isFirstTextRender = { false };
	_bool				m_isVOSoundActive = { false };


public:
	static CEventNotify* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

