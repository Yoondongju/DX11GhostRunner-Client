#pragma once

#include "Base.h"
#include "Transform.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct GAMEOBJECT_DESC : public CTransform::TRANSFORM_DESC
	{
		_uint					iObjectType = {};

	} GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	const _wstring&			  Get_ChangeLayerName() { return m_strChangeLayerName; }

	void					  Set_FinalLayerName(const _wstring& strFinalLayerName) { m_strFinalLayerName = strFinalLayerName; }
	const _wstring&			  Get_FinalLayerName() { return m_strFinalLayerName; }

	_uint					  Get_ModelListType() { return m_iObjectType; }

	
	virtual void			  Set_ModelPrototypeTag(const _wstring& strModelPrototypeName) { m_strModelPrototypeName = strModelPrototypeName; }

	virtual class CModel*	  Get_Model() { return nullptr; }
	virtual class CFsm*		  Get_Fsm() { return nullptr; }
	virtual class CRigidBody* Get_RigidBody() { return nullptr; }


public:
	class CTransform* Get_Transform() { return m_pTransformCom; }




public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


public:
	class CComponent* Find_Component(const _wstring& strComponentTag);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };


	
	_wstring				m_strChangeLayerName = L"";		// 레이어이름이 바뀌어야 하는경우
	_wstring				m_strFinalLayerName = L"";		// 최종 나의 레이어 이름알고있어야해 이유: 나중에 삭제할때필요

	_wstring				m_strModelPrototypeName = L"";

	_uint					m_iObjectType = {};




protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END