#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHel_RunJump final : public CState
{
private:
    CHel_RunJump(class CGameObject* pOwner);
    virtual ~CHel_RunJump() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();



public:
    static CHel_RunJump* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
