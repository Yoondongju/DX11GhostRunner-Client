#include "..\Public\Component_Manager.h"
#include "Component.h"

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new map<const _wstring, class CComponent*>[iNumLevels];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag, CComponent * pPrototype)
{
	CComponent* pPrePrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr != pPrePrototype)	// 이미 원형이 있는데 추가할라카면 
	{
		Safe_Release(pPrototype);
		// m_pPrototype[iNumLevelIndex].erase(strPrototypeTag);
		return S_OK;
	}

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);
	pPrototype->Set_PrototypeName(strPrototypeTag);

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, void * pArg)
{
	CComponent*		pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if(nullptr == pPrototype)		
		return nullptr;

	CComponent*		pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

CComponent * CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag)
{
	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if(iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

_bool CComponent_Manager::IsFind_Model(_uint iLevelIndex, const _wstring& strModelPrototypeName)
{
	auto	iter = m_pPrototypes[iLevelIndex].find(strModelPrototypeName);

	if (iter == m_pPrototypes[iLevelIndex].end())
		return false;

	return true;
}

CComponent_Manager * CComponent_Manager::Create(_uint iNumLevels)
{
	CComponent_Manager*		pInstance = new CComponent_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CComponent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComponent_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
