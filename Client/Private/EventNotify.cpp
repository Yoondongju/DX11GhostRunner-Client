#include "stdafx.h"
#include "../Public/EventNotify.h"
#include "GameInstance.h"



CEventNotify::CEventNotify(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CEventNotify::CEventNotify(const CEventNotify& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CEventNotify::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEventNotify::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_bActivate = false;

	return S_OK;
}

void CEventNotify::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CEventNotify::Update(_float fTimeDelta)
{
	if (true == m_bActivate)
	{
		if (m_fUnableTime <= 0.f)
		{
			m_bActivate = false;
			m_isVOSoundActive = false;

			m_fUnableTime = 2.f;
		}
		
		m_fUnableTime -= fTimeDelta;
	}


	if (false == m_bActivate)
		return;



	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CEventNotify::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CEventNotify::Render()
{
	const _tchar*		pCurText = { nullptr };
	const _tchar*		pCurSoundText = { nullptr };

	
	switch (m_eCurEvent)
	{
	case Client::CEventNotify::UNABLE_SKILL_COOLTIME:
		pCurText = TEXT("아직 스킬을 사용할 수 없습니다.");
		pCurSoundText = TEXT("NoSkill.ogg");
		break;
	case Client::CEventNotify::UNABLE_CUTALL:
		pCurText = TEXT("스킬을 사용할 적이 시야에 없거나 그 수가 부족합니다.");
		pCurSoundText = TEXT("NoEnemySight.ogg");
		break;
	case Client::CEventNotify::UNABLE_MINDCONTROL:
		pCurText = TEXT("스킬을 사용할 적이 시야에 없거나 그 수가 부족합니다.");
		pCurSoundText = TEXT("NoEnemySight.ogg");
		break;
	case Client::CEventNotify::UNABLE_HOMING_SH:
		pCurText = TEXT("추적할 대상이 존재하지 않습니다.");
		pCurSoundText = TEXT("NoTrackEnemy.ogg");
		break;
	case Client::CEventNotify::UNABLE_SWAPWEAPON:
		pCurText = TEXT("아직 무기를 변경할 수 없습니다.");
		pCurSoundText = TEXT("NoChangeWeapon.ogg");
		break;
	case Client::CEventNotify::TEXT_EVENT_END:
		break;
	default:
		break;
	}


	if (false == m_isVOSoundActive)
	{
		m_pGameInstance->Play_Sound(pCurSoundText, SOUND_PLAYERVOICE, 2.f);
		m_isVOSoundActive = true;
	}

	
	_float	fFontSize = 0.5f;
	
	_uint iTextLength = wcslen(pCurText);		// 글자 길이
	 
	_float fPosX = (g_iWinSizeX - iTextLength * 15) * 0.5f;
	
	 m_pGameInstance->Render_Text(TEXT("Font_145"), pCurText,
		XMVectorSet(fPosX, g_iWinSizeY - 200.f, 0.f, 1.f),
		 fFontSize,
		XMVectorSet(m_fUnableTime / 2.f ,1.f, 1.f, 1.f),
		 0.f,
		 XMVectorSet(0.f, 0.f, 0.f, 1.f));

	
	

	return S_OK;
}






CEventNotify* CEventNotify::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEventNotify* pInstance = new CEventNotify(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEventNotify"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEventNotify::Clone(void* pArg)
{
	CEventNotify* pInstance = new CEventNotify(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEventNotify"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventNotify::Free()
{
	// 레이어 에서 얘의 Free 들어왓고
	// 자식들이 자기 부모를 계속 지운단말이야

	for (auto& pUI : m_childUI_List)
	{
		Safe_Release(pUI);
	}
	m_childUI_List.clear();

	__super::Free();
}
