#include "CollisionManager.h"

#include "GameObject.h"
#include "GameInstance.h"
#include <ContainerObject.h>

CCollisionManager::CCollisionManager()
	: m_pGameInstance{CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CCollisionManager::Initialize()
{
	return S_OK;
}


void CCollisionManager::Update(_float fTimeDelta)
{

	for (_uint iRow = 0; iRow < m_ColObject.size(); iRow++)
	{
		for (_uint iCol = iRow; iCol < m_ColObject.size(); iCol++)
		{	
			if (iRow == iCol)	// 지가 지랑 충돌하는건 X
				continue;

			CollisionUpdateGroup(m_ColObject[iRow], m_ColObject[iCol]);
		}
	}

}




void CCollisionManager::CollisionUpdateGroup(CGameObject* pSourObj, CGameObject* pDestObj)
{
	
}


CCollisionManager* CCollisionManager::Create()
{
	CCollisionManager* pInstance = new CCollisionManager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollisionManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollisionManager::Free()
{
	__super::Free();


	Safe_Release(m_pGameInstance);
	
	for (_uint i = 0; i < m_ColObject.size(); i++)
	{
		Safe_Release(m_ColObject[i]);
	}
	m_ColObject.clear();
}
