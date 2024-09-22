#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Block1 final : public CState
{
private:
    CPlayer_Block1(class CGameObject* pOwner);
    virtual ~CPlayer_Block1() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:


public:
    static CPlayer_Block1* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
