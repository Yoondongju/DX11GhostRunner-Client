#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollisionManager final : public CBase
{
private:
	CCollisionManager();
	virtual ~CCollisionManager() = default;


public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);




private:
	class CGameInstance*								m_pGameInstance = { nullptr };
	vector<class CGameObject*>							m_ColObject;		      // �浹�ϴ� ������Ʈ
																			

private:
	void CollisionUpdateGroup(class CGameObject* pSourObj, class CGameObject* pDestObj);	




public:
	static CCollisionManager* Create();
	virtual void Free() override;
};

END

