#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CMira_Suiside final : public CState
{
private:
    CMira_Suiside(class CGameObject* pOwner);
    virtual ~CMira_Suiside() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();




public:
    static CMira_Suiside* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
