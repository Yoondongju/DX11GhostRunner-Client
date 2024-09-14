#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CJetpack_Fly final : public CState
{
private:
    CJetpack_Fly(class CGameObject* pOwner);
    virtual ~CJetpack_Fly() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float      m_fCanFindPlayerDistance = {160.f};

    _bool       m_isFindPlayer = { false };
    


    _float3     m_fOriginRight = {};
    _float3     m_fOriginUp = {};
    _float3     m_fOriginLook = {};

private:
    _bool       Check_Detect();


public:
    static CJetpack_Fly* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
