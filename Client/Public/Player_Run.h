#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Run final : public CState
{
private:
    CPlayer_Run(class CGameObject* pOwner);
    virtual ~CPlayer_Run() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();



private:
    _float  m_fSwordSpinTime = { 5.f };
    _bool   m_fFristSpinCall = { true };


private:
    _bool Check_HookUp();
    _bool Check_Jump();

    _bool Check_Block1();
    _bool Check_Block2();
    _bool Check_Block3();


private:
    _bool Check_CutAll();


public:
    static CPlayer_Run* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
