#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Attack3 final : public CState
{
private:
    CPlayer_Attack3(class CGameObject* pOwner);
    virtual ~CPlayer_Attack3() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    void        Check_Collision();

public:
    static CPlayer_Attack3* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
