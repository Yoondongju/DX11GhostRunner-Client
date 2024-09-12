#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPistol_Walk final : public CState
{
private:
    CPistol_Walk(class CGameObject* pOwner);
    virtual ~CPistol_Walk() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float      m_fCanAttackDistance = {60.f};

private:
    _bool      Check_Attack(_float fTimeDelta);

public:
    static CPistol_Walk* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
