#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CJetpack_Idle final : public CState
{
private:
    CJetpack_Idle(class CGameObject* pOwner);
    virtual ~CJetpack_Idle() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _bool   Check_Fly();

public:
    static CJetpack_Idle* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
