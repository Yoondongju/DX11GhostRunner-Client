#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Block3 final : public CState
{
private:
    CPlayer_Block3(class CGameObject* pOwner);
    virtual ~CPlayer_Block3() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:


public:
    static CPlayer_Block3* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
