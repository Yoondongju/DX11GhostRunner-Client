#pragma once


#include "Client_Defines.h"
#include "GameObject.h"



BEGIN(Client)

class CMonster abstract : public CGameObject
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


protected:
	class CPlayer*	m_pPlayer = { nullptr };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END