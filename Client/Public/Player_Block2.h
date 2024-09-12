#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Block2 final : public CState
{
private:
    CPlayer_Block2(class CGameObject* pOwner);
    virtual ~CPlayer_Block2() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:


public:
    static CPlayer_Block2* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
