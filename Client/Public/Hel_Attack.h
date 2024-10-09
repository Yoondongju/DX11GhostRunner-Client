#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHel_Attack final : public CState
{
private:
    CHel_Attack(class CGameObject* pOwner);
    virtual ~CHel_Attack() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float      m_fAttackRange = { 0.f };
    

public:
    static CHel_Attack* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
