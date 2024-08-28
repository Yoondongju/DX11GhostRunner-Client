#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Walk final : public CState
{
private:
    CPlayer_Walk(class CGameObject* pOwner);
    virtual ~CPlayer_Walk() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    void Check_HookUp();
    void Check_Jump();

public:
    static CPlayer_Walk* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
