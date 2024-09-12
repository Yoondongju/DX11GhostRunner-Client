#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CMira_Attack final : public CState
{
private:
    CMira_Attack(class CGameObject* pOwner);
    virtual ~CMira_Attack() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();



private:
    _float      m_fDetectRadius = { 150.f };     // 탐지 가능한 Range의 반지름

    _float3     m_OriginRight = {};
    _float3     m_OriginUp = {};
    _float3     m_OriginLook = {};

private:
    _bool   Check_DetectOut();
    _bool   Check_Death();



public:
    static CMira_Attack* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
