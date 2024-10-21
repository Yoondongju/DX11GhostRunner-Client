#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CElite_Walk final : public CState
{
private:
    CElite_Walk(class CGameObject* pOwner);
    virtual ~CElite_Walk() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float          m_fRetreatTime = {0.f};
    _float          m_fTurboTime = { 0.f };


private:
    _bool   Check_Death();
    _bool   Check_Turbo(_float fTimeDelta);

public:
    static CElite_Walk* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
