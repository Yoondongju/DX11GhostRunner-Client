#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Sh_Dash final : public CState
{
private:
    CPlayer_Sh_Dash(class CGameObject* pOwner);
    virtual ~CPlayer_Sh_Dash() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _bool Check_Jump();

public:
    static CPlayer_Sh_Dash* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
