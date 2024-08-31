#include "../Public/Fsm.h"
#include "../Public/State.h"
#include "../Public/GameObject.h"

CFsm::CFsm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent{ pDevice,pContext }
{

}

CFsm::CFsm(const CFsm& Prototype)
	: CComponent{ Prototype }
	, m_iNumMaxState{ Prototype.m_iNumMaxState }
{

}

HRESULT CFsm::Initialize_Prototype(_uint iNumMaxState)
{
	m_iNumMaxState = iNumMaxState;

	return S_OK;
}

HRESULT CFsm::Initialize(void* pArg)
{
	m_States.resize(m_iNumMaxState);

	return S_OK;
}

void CFsm::Update(_float fTimeDelta)
{
	m_pNowState->Update(fTimeDelta);
}

void CFsm::Release_States()
{
	//for (auto& iter : m_States)
	//	Safe_Release(iter);
	//
	//Safe_Release(m_pNowState);
	//Safe_Release(m_pPreState);
	//
	//m_States.clear();
}

_uint CFsm::Get_CurStateIndex()
{
	return m_pNowState->Get_StateIndex();
}

HRESULT CFsm::Add_State(CState* pState)
{
	if (nullptr == pState)
	{
		MSG_BOX(TEXT("추가하시려는 State가 NULL 입니다."));
		return E_FAIL;
	}


	_uint iStateIndex = pState->Get_StateIndex(); // Idle은 뭐 20  Walk는 15  Climb은 0 이게 그 애니메이션 == 이랑 State랑 와꾸가 맞아야함

	m_States[iStateIndex] = pState;


	return S_OK;
}

void CFsm::Change_State(_uint iStateIndex, _int iStateDir, void* pArg)
{
	//Safe_Release(m_pPreState);
	//Safe_Release(m_pNowState);

	m_pPreState = m_pNowState;
	//Safe_AddRef(m_pPreState);


	m_pNowState->End_State();


	m_pNowState = m_States[iStateIndex];
	//Safe_AddRef(m_pNowState);

	if (-1 != iStateDir)
		m_pNowState->Set_Dir(CState::STATE_DIR(iStateDir));

	m_pNowState->Start_State();

	return;
}

void CFsm::Set_State(_uint iStateIndex, void* pArg)
{
	m_pNowState = m_States[iStateIndex];
	m_pPreState = m_States[iStateIndex];

	//Safe_AddRef(m_pNowState);
	//Safe_AddRef(m_pPreState);

	m_pNowState->Start_State();

	return;
}

CFsm* CFsm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumMaxState)
{
	CFsm* pInstance = new CFsm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumMaxState)))
	{
		MSG_BOX(TEXT("Failed to Created : CFsm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CFsm::Clone(void* pArg)
{
	CFsm* pInstance = new CFsm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CFsm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFsm::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		for (_uint i = 0; i < m_iNumMaxState; i++)
		{
			Safe_Release(m_States[i]);
		}
		m_States.clear();
	}



	Safe_Release(m_pNowState);
	Safe_Release(m_pPreState);
}
