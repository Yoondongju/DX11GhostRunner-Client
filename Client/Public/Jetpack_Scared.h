#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CJetpack_Scared final : public CState
{
private:
    CJetpack_Scared(class CGameObject* pOwner);
    virtual ~CJetpack_Scared() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State();
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();




public:
    static CJetpack_Scared* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
