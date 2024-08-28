#include "../Public/State.h"
#include "../Public/GameObject.h"

#include "../Public/GameInstance.h"

CState::CState(_uint iStateIndex, class CGameObject* pOwner)
	: m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_iStateIndex{ iStateIndex }
	, m_pOwner{ pOwner }
{
	Safe_AddRef(m_pGameInstance);
	//Safe_AddRef(m_pOwner);
}

void CState::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	//Safe_Release(m_pOwner);
}
