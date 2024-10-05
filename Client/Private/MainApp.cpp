#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "Level_Loading.h"
#include "GameInstance.h"



CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	/* 내가 레퍼런스 카운트를 증가시키는 시점 : */
	/* 원래 보관하기위한 포인터에 저장을 할때는 증가시키지 않는다. */
	/* 원래 보관된 위치에 있던 주소를 다른 포인터형변수에 저장을 하게되면 이때는 증가시킨다. */
	Safe_AddRef(m_pGameInstance);	
}

HRESULT CMainApp::Initialize()
{	
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowsed = true;

	/* 엔진을 사용하기위한 여러 초기화 과정을 수행한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	/*m_pContext->RSSetState();
	m_pContext->OMSetDepthStencilState();

	D3D11_BLEND_DESC		bLENDdEESC;
	ID3D11BlendState*		pBlendState;
	m_pDevice->CreateBlendState();
	m_pContext->OMSetBlendState(pBlendState);*/

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	if (FAILED(Ready_Font()))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Render_Begin();

	m_pGameInstance->Draw_Engine();

	

	m_pGameInstance->Render_End();

	return S_OK;
}

void CMainApp::Final_Update()
{
	m_pGameInstance->FinalUpdate_Engine();
}


HRESULT CMainApp::Ready_Prototype_Component_Static()
{
	
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;	

	///* For.Prototype_Component_Shader_VtxNorTex */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	//	return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eStartLevelID)
{	
	/* 어떤 레벨을 선택하던 로딩 레벨로 선 진입한다.  */
	/* 로딩레벨에서 내가 선택한 레벨에 필요한 자원을 준비한다. */
	/* 로딩 레벨은 다음레벨(내가 선택한 레벨)이 누구인지를 인지해야한다. */
	if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_145"), TEXT("../Bin/Resources/Fonts/143ex.spritefont"))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
	
}

