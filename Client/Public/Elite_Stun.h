#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CElite_Stun final : public CState
{
private:
    CElite_Stun(class CGameObject* pOwner);
    virtual ~CElite_Stun() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:



public:
    static CElite_Stun* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
