#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CElite_Alert final : public CState
{
private:
    CElite_Alert(class CGameObject* pOwner);
    virtual ~CElite_Alert() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float          m_fCanTurboDistance = 250.f;
    _float          m_fCanTurboActiveTime = { 1.1f };

    _bool           m_isAttackActive = { false };

private:
    _bool   Check_Death();
    _bool   Check_Turbo(_float fTimeDelta);



public:
    static CElite_Alert* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
