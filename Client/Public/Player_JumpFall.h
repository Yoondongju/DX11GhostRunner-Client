#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_JumpFall final : public CState
{
private:
    CPlayer_JumpFall(class CGameObject* pOwner);
    virtual ~CPlayer_JumpFall() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _bool       m_isFirstCallLoop = { true };
    _bool       m_isFirstCallEnd = { true };



private:
    _bool Check_HookUp();
    _bool Check_Nami();

    _bool Check_Jump();

public:
    static CPlayer_JumpFall* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
