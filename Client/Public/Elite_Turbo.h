#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CElite_Turbo final : public CState
{
private:
    CElite_Turbo(class CGameObject* pOwner);
    virtual ~CElite_Turbo() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _bool           m_bStartTurboDash = { false };
    _float          m_fAccSpeed = { 0.f };
    _float3         m_GoDirNor = {};

    _float          m_fMaxRushDistance = { 250 - 30 };     // 최대 돌진 거리     Elite_Alert의 발견할수있는 탐지거리와 와꾸맞추자
 
    _float3         m_RushStartPos = {};                   // 돌진을 처음 시작한 거리

    _uint           m_iCountSuccessParrying = { 0 };

private:
    _bool   Check_Death();
    _bool   Check_TurboDash(_float fTimeDelta);





public:
    static CElite_Turbo* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
