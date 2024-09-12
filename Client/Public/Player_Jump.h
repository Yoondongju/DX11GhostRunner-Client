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
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();



private:
    _float  m_fLastKeyPressTimeSpace = 0.f;
    _bool   m_isCanDoubleJump = { true };



private:
    _bool Check_HookUp();



    _bool Check_Climb();

    _bool Check_DoubleJump();

    _bool Check_Sh_Attack1();


public:
    static CPlayer_Jump* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
