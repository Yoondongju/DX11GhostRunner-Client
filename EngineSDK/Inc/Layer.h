#pragma once

#include "Base.h"

/* 객체들을 보관한다. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	_uint Get_GamObjectSize() { return m_GameObjects.size(); }
	list<class CGameObject*>& Get_GamObjects() { return m_GameObjects; }

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);



public:
	class CComponent* Find_Component(const _wstring& strComponentTag, _uint iIndex);
	class CGameObject* Find_Object(_uint iIndex);


private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END