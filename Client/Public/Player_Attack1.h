#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Attack1 final : public CState
{
private:
    CPlayer_Attack1(class CGameObject* pOwner);
    virtual ~CPlayer_Attack1() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:


public:
    static CPlayer_Attack1* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
