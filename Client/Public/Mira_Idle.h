#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CMira_Idle final : public CState
{
private:
    CMira_Idle(class CGameObject* pOwner);
    virtual ~CMira_Idle() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();



private:
    _float      m_fDetectRadius = { 150.f };     // 탐지 가능한 Range의 반지름

private:
    _bool   Check_Detect();
    _bool   Check_Death();

    _bool   Check_MindControling();


public:
    static CMira_Idle* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
