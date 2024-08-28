#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Block final : public CState
{
private:
    CPlayer_Block(class CGameObject* pOwner);
    virtual ~CPlayer_Block() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:


public:
    static CPlayer_Block* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
