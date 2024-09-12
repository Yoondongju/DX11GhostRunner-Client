#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CMira_WalkIn final : public CState
{
private:
    CMira_WalkIn(class CGameObject* pOwner);
    virtual ~CMira_WalkIn() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();




public:
    static CMira_WalkIn* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
