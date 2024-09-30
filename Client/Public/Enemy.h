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
	_bool					IsTargetting() { return m_isTargeting; }
	void					Set_Targeting(_bool b) { m_isTargeting = b; }

	_bool					IsFinalTargetting() { return m_isFinalTargeting; }
	void					Set_FinalTargeting(_bool b) { m_isFinalTargeting = b; }




public:
	virtual CCollider*		Get_Collider() { return nullptr; }
	virtual _bool			IsDead() { return false; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	
public:
	virtual void Check_CollByTargetEnemy() {};
	virtual void Check_Collision_Me() {};
	


protected:
	_bool				m_isTargeting = { false };			// 내가 탐색한 타겟으로 정해졋니
	_bool				m_isFinalTargeting = { false };		// 최종 내가 사냥할 타겟으로 정해졋니

protected:
	_bool				m_isMindControling = { false };		// 마인드 컨트롤 당했니?
	CEnemy*				m_pTargetEnemy = { nullptr };

	


 
private:
	HRESULT		Create_EnemyMarker();


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END