#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHomingShuriken final : public CState
{
public:
    struct Float3Wrapper {
        _float3* pValue;
        _float  fDistanceToPlayer;  // 플레이어와의 거리를 미리 계산한 값

            // 거리 기준으로 비교
        bool operator<(const Float3Wrapper& other) const {
            return fDistanceToPlayer < other.fDistanceToPlayer;  // 거리를 기준으로 오름차순 정렬
        }
    };

private:
    CHomingShuriken(class CGameObject* pOwner);
    virtual ~CHomingShuriken() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    vector<Float3Wrapper>     m_TargetsPos = {};
   

    _float3            m_StartPos = {};
    _float             m_fAccScaling = 1.f;
    _bool              m_isDownScale = { false };

    _float             m_fStartTime = {};
    _float             m_fAccSpeed = {};
    _bool              m_isHuntStart = { false };
    _uint              m_iNumKill = { 0 };

    



private:
    void        FindVisiableEnemy();
    void        Check_Collision();

public:
    static CHomingShuriken* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
