#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHel_Sleep final : public CState
{
private:
    CHel_Sleep(class CGameObject* pOwner);
    virtual ~CHel_Sleep() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:




public:
    static CHel_Sleep* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
