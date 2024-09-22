#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Idle final : public CState
{
private:
    CPlayer_Idle(class CGameObject* pOwner);
    virtual ~CPlayer_Idle() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float m_fLastKeyPressTimeW = 0.f;  
    _float m_fLastKeyPressTimeS = 0.f;  
    _float m_fLastKeyPressTimeA = 0.f;  
    _float m_fLastKeyPressTimeD = 0.f;  
    
    _float  m_fSwordSpinTime = { 5.f };
    _bool   m_fFristSpinCall = { true };


private:
    _bool Check_RunAndWalk();

    _bool Check_Dash();
    _bool Check_Sh_Dash();

    _bool Check_HookUp();
    _bool Check_Jump();


    _bool Check_Block1();
    _bool Check_Block2();
    _bool Check_Block3();

    _bool Check_Attack1();
    _bool Check_Attack2();
    _bool Check_Attack3();

    _bool Check_Sh_Attack1();
        
    _bool Check_SwapWeapon();

private:
    _bool Check_CutAll();
    _bool Check_Nami();
    _bool Check_MindControl();
    _bool Check_TimeStop();


public:
    static CPlayer_Idle* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
