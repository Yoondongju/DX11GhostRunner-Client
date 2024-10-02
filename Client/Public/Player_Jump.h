#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Jump final : public CState
{
private:
    CPlayer_Jump(class CGameObject* pOwner);
    virtual ~CPlayer_Jump() = default;

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

private:
    _float  m_fLastKeyPressTimeSpace = 0.f;
    _bool   m_isCanDoubleJump = { true };



private:
    _bool Check_Jump();

private:
    _bool Check_HookUp();
    _bool Check_Climb();

    _bool Check_DoubleJump();
    _bool Check_Sh_Attack1();

private:
    _bool Check_Dash();
    _bool Check_Sh_Dash();


private:
    _bool Check_CutAll();
    _bool Check_Nami();
    _bool Check_TimeStop();



public:
    static CPlayer_Jump* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
