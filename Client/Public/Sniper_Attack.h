#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CSniper_Attack final : public CState
{
private:
    CSniper_Attack(class CGameObject* pOwner);
    virtual ~CSniper_Attack() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();



private:
    _float      m_fDetectRadius = { 200.f };     // 탐지 가능한 Range의 반지름

    _float3     m_OriginRight = {};
    _float3     m_OriginUp = {};
    _float3     m_OriginLook = {};

    _bool       m_isMindContorling = { false };

private:
    _bool   Check_DetectOut();
    _bool   Check_Death();

    _bool   Check_MindControling();

public:
    static CSniper_Attack* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
