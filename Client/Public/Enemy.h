#pragma once


#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CEnemy abstract : public CContainerObject
{
protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy(const CEnemy& Prototype);
	virtual ~CEnemy() = default;


public:
	_bool			IsMindControling() { return m_isMindControling; }
	void			Set_MindControling(_bool b , CEnemy* pTarget) 
	{ 
		m_isMindControling = b;

		if (nullptr != pTarget)
		{
			m_pTargetEnemy = pTarget;
			Safe_AddRef(m_pTargetEnemy);
		}
		else if (nullptr == pTarget)
		{
			m_pTargetEnemy = pTarget;
			Safe_Release(m_pTargetEnemy);
		}
	}

	CEnemy*&				Get_TargetEnemy() { return m_pTargetEnemy; }
	

public:
	virtual CCollider*		Get_Collider() { return nullptr; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	
public:
	virtual void Check_CollByTargetEnemy() {};


protected:
	_bool			m_isMindControling = { false };		// 마인드 컨트롤 당했니?

	CEnemy*			m_pTargetEnemy = { nullptr };

 


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END