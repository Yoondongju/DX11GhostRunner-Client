#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHel_Idle final : public CState
{
private:
    CHel_Idle(class CGameObject* pOwner);
    virtual ~CHel_Idle() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float      m_fDetectRadius = { 400.f };


private:
    _bool   Check_Detect();

    _bool   Check_Attack();
    _bool   Check_Jump();
    _bool   Check_Dash();


public:
    static CHel_Idle* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
