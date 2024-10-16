#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHel_CutScene final : public CState
{
private:
    CHel_CutScene(class CGameObject* pOwner);
    virtual ~CHel_CutScene() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float      m_fAccRotationSpeed = { 0.f };
    _bool       m_isStartTurn = { false };
    _bool       m_isStartJump = { false };
    _float      m_fInitialYSpeed = {0.f};
    _bool       m_isEndJump = { false };
    _bool       m_isActiveNamiCutScene = { false };


    _float3     m_TrigerPos_FromJump_Page2 = {};            // 점프할 트리거위치
    _float3     m_TrigerPos_FromBlock_Page2 = {};           // 막을 트리거위치


    _float      m_t = 0.0f;


private:
    _vector BezierCurve(_float t, const _fvector& P0, const _fvector& P1, const _fvector& P2, const _fvector& P3);

private:
    void        Page1(_float fTimeDelta);
    void        Page2(_float fTimeDelta);


public:
    static CHel_CutScene* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
