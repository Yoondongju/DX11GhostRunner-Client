#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CState :
    public CBase
{
public:
    enum STATE_DIR { FRONT, BACK, LEFT, RIGHT, STATE_DIR_END };

protected:
    CState(_uint iStateIndex, class CGameObject* pOwner);
    virtual ~CState() = default;

public:
    void        Set_Dir(STATE_DIR eDir) { m_eDir = eDir; }

    _uint		Get_StateIndex() { return m_iStateIndex; }


public:
    virtual HRESULT Initialize() { return S_OK; }
    virtual HRESULT Start_State(void* pArg = nullptr) = 0;
    virtual void    Update(_float fTimeDelta) = 0;
    virtual void    End_State() = 0;


protected:
    _uint 			        m_iStateIndex;
    _float                  m_fAccTime = { 0.f };
    STATE_DIR               m_eDir = { STATE_DIR_END };
    _float			        m_fSoundTimer = 0.f;



    class CGameInstance* m_pGameInstance = { nullptr };
    class CGameObject*   m_pOwner = { nullptr };

public:
    virtual void Free() override;
};

END