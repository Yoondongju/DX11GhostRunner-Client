#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Nami final : public CState
{
private:
    CPlayer_Nami(class CGameObject* pOwner);
    virtual ~CPlayer_Nami() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();



private:
    vector<class CGameObject*>            m_Targets = {};
    _float                                m_fVisibleAngle = { 90.f };
    _float                                m_fVisiableDistance = { 800.f };
    _bool                                 m_isFindTargets = { false };
    _float                                m_fTargetCreateTime = { 0.f };
    _uint                                 m_iTargetIndex = { 0 };
    _bool                                 m_isHuntActive = { false };

private:
    _float3         m_TargetPos = _float3(0.f,0.f,0.f);
    _float3         m_TargetLook = _float3(0.f, 0.f, 0.f);
    _float          m_fMinDistance = { 999999.f };

private:
    _float          m_fAccSpeed = {0.f};
 

    _float          m_fStartTime = {0.f};


private:
    void        FindVisiableEnemy();
    void        FindShortestDistanceEnemy();


    void        Check_Collision();



public:
    static CPlayer_Nami* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
