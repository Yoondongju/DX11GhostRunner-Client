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
    _float  m_fLastKeyPressTimeW = -1.f;
    _float  m_fLastKeyPressTimeS = -1.f;
    _float  m_fLastKeyPressTimeA = -1.f;
    _float  m_fLastKeyPressTimeD = -1.f;

    


private:
    _bool Check_HookUp();
    _bool Check_Dash();
    _bool Check_Climb();

public:
    static CPlayer_Jump* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
