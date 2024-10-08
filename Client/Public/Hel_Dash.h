#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHel_Dash final : public CState
{
private:
    CHel_Dash(class CGameObject* pOwner);
    virtual ~CHel_Dash() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();




public:
    static CHel_Dash* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
