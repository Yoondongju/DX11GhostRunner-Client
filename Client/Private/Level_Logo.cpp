#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "Level_Loading.h"

#include "GameInstance.h"
#include "FreeCamera.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;


	CFreeCamera::CAMERA_FREE_DESC		Desc{};

	Desc.fSensor = 0.2f;
	Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(90.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 4000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), TEXT("No Model Prototype"), &Desc)))
		return E_FAIL;

	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(TEXT("19_Daniel_Deluxe_-_Air.ogg"), 1.f);

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (true == m_isEnterGame)
	{
		m_fAccTime += fTimeDelta;

		if (m_fAccTime >= 0.65f)
		{
			m_pGameInstance->UnActiveRefraction();
			m_pGameInstance->UnActiveBlur();
			m_fAccTime = 0.f;
			m_isEnterGame = false;

			if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1))))
				return;	
		}
	}

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		m_pGameInstance->Play_Sound(TEXT("SFX_extratime_pickup_lay1_01.ogg"), SOUND_PLAYER, 3.f);

		m_pGameInstance->ActiveRefraction(nullptr, CRenderer::REFRACTION_TYPE::SCREENSPLIT);
		m_pGameInstance->ActiveBlur(nullptr,CRenderer::BLUR_TYPE::GAUSSIAN_BLUR);
		m_isEnterGame = true;	
	}
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag,
		TEXT("Prototype_GameObject_LogoPanel") , TEXT("UI인데 테스트용"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag,
		TEXT("Prototype_GameObject_LogoName"), TEXT("UI인데 테스트용"))))
		return E_FAIL;


	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

}
