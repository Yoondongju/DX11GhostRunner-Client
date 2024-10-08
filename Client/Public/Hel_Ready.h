#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHel_Ready final : public CState
{
private:
    CHel_Ready(class CGameObject* pOwner);
    virtual ~CHel_Ready() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();



public:
    static CHel_Ready* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
