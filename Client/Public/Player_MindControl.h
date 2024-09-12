#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_MindControl final : public CState
{
private:
    CPlayer_MindControl(class CGameObject* pOwner);
    virtual ~CPlayer_MindControl() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:


public:
    static CPlayer_MindControl* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
