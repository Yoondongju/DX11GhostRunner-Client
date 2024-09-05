#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_PHYSX, TYPE_END };

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;


public:
	void	Set_PrePhysXCollision(_bool isColl)
	{
		m_isPhysXPreColl = isColl;
	}

	void	Set_CurPhysXCollision(_bool isColl)
	{ 
		m_isPhysXCurColl = isColl;
	}

	_bool	Get_CurPhysXCollision() { return m_isPhysXCurColl; }


	_bool	IsPhysXCollisionEnter()
	{ 
		if (false == m_isPhysXPreColl &&
			true == m_isPhysXCurColl)
		{
			return true;
		}
		else
			return false;
	}

	_bool	IsPhysXCollisionStay()
	{
		if (true == m_isPhysXPreColl &&
			true == m_isPhysXCurColl)
		{
			return true;
		}
		else
			return false;
	}

	_bool	IsPhysXCollisionExit()
	{
		if (true == m_isPhysXPreColl &&
			false == m_isPhysXCurColl)
		{
			return true;
		}
		else
			return false;
	}
	
public:
	_bool	IsBoundingCollisionEnter();
	_bool	IsBoundingCollisionStay();
	_bool	IsBoundingCollisionExit();










public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float4x4* pWorldMatrix);
	virtual HRESULT Render();

public:
	_bool Intersect(CCollider* pTargetCollider);


private:
	class CBounding*	m_pBounding = { nullptr };
	TYPE				m_eColliderType = {};


private:
	_bool		m_isPhysXPreColl = false;
	_bool		m_isPhysXCurColl = false;



#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect*						 m_pEffect = { nullptr };
	ID3D11InputLayout*					 m_pInputLayout = { nullptr };


#endif




public:
	static CCollider* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

