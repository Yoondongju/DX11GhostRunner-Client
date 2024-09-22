#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Sh_Run final : public CState
{
private:
    CPlayer_Sh_Run(class CGameObject* pOwner);
    virtual ~CPlayer_Sh_Run() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _bool Check_HookUp();
    _bool Check_Jump();

    _bool Check_Sh_Attack1();

public:
    static CPlayer_Sh_Run* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
