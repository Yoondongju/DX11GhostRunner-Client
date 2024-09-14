#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END


BEGIN(Client)

class CSubShuriken final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3		MyOffSet = {};

	}SUBSHURIKEN_DESC;


private:
	CSubShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSubShuriken(const CSubShuriken& Prototype);
	virtual ~CSubShuriken() = default;

public:
	CCollider*	Get_Collider() { return m_pColliderCom; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float4x4* pParentWMatrix , _bool isAttacking, _float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CCollider*  m_pColliderCom = { nullptr };

private:
	_float3		m_MyOffSet = {};



private:
	HRESULT Ready_Components();

public:
	static CSubShuriken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END