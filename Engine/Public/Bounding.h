#pragma once

#include "Base.h"
#include "DebugDraw.h"
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct
	{
		_float3		vCenter;
	}BOUNDING_DESC;
protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;


public:
	_bool	IsBoundingCollisionEnter()
	{
		if (false == m_isPreColl &&
			true == m_isCurColl)
		{
			return true;
		}
		else
			return false;
	}
	_bool	IsBoundingCollisionStay()
	{
		if (true == m_isPreColl &&
			true == m_isCurColl)
		{
			return true;
		}
		else
			return false;
	}
	_bool	IsBoundingCollisionExit()
	{
		if (true == m_isPreColl &&
			false == m_isCurColl)
		{
			return true;
		}
		else
			return false;
	}





public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix);
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);

public:
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pBounding) = 0;



protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_bool					m_isPreColl = { false };
	_bool					m_isCurColl = { false };

public:
	virtual void Free() override;
};

END