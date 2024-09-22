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
    _float3         m_TargetPos = _float3(0.f,0.f,0.f);
    _float3         m_TargetLook = _float3(0.f, 0.f, 0.f);
    _float          m_fMinDistance = { 999999.f };

private:
    _float          m_fAccSpeed = {0.f};
    _bool           m_isFirstFound = { true };  // 처음 발견했니 

    _float          m_fStartTime = {0.f};


private:
    void        Check_Collision();

public:
    static CPlayer_Nami* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
