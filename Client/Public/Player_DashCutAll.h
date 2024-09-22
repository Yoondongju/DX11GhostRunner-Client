#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_DashCutAll final : public CState
{
private:
    CPlayer_DashCutAll(class CGameObject* pOwner);
    virtual ~CPlayer_DashCutAll() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    void        Check_Collision();

public:
    static CPlayer_DashCutAll* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
