#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CSniper_Idle final : public CState
{
private:
    CSniper_Idle(class CGameObject* pOwner);
    virtual ~CSniper_Idle() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();



private:
    _float      m_fDetectRadius = { 200.f };     // 탐지 가능한 Range의 반지름

private:
    _bool   Check_Detect();
    _bool   Check_Death();



public:
    static CSniper_Idle* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
