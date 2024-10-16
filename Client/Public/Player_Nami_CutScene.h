#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Nami_CutScene final : public CState
{
private:
    CPlayer_Nami_CutScene(class CGameObject* pOwner);
    virtual ~CPlayer_Nami_CutScene() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float          m_fAccSpeed = { 0.f };
    _float          m_fStartTime = { 0.f };


private:
    void        Check_Collision();



public:
    static CPlayer_Nami_CutScene* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
