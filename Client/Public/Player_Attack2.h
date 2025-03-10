#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Attack2 final : public CState
{
private:
    CPlayer_Attack2(class CGameObject* pOwner);
    virtual ~CPlayer_Attack2() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    void        Check_Collision();

public:
    static CPlayer_Attack2* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
