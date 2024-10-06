#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CElite_CutScene final : public CState
{
private:
    CElite_CutScene(class CGameObject* pOwner);
    virtual ~CElite_CutScene() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float3         m_CutSceneExitPointPage1 = { };
    _float3         m_CutSceneExitPointPage2 = { };

    _float          m_fAccRotationSpeed = { 0.f };
    _float          m_fSpeed = { 1.f };



public:
    static CElite_CutScene* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
