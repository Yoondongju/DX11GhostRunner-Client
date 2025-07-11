#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "BlendObject.h"
#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

_uint		g_iSizeX = 8192;
_uint		g_iSizeY = 4608;



CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	_uint			iNumViewports = { 1 };

	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);


	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SRM"), _uint(ViewportDesc.Width), _uint(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PickDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Height"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PreFinal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Y"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_MotionBlur"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_OriginBloomEffect"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomBlur_X"), ViewportDesc.Width / 4.f, ViewportDesc.Height / 4.f, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomBlur_Y"), ViewportDesc.Width / 4.f, ViewportDesc.Height / 4.f, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomEffect"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_FinalBloomEffect"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	

	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_SRM"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_PickDepth"))))
		return E_FAIL;




	/* MRT_Lights */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL;

	///* MRT_Height */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Height"), TEXT("Target_Height"))))
	//	return E_FAIL;

	/* MRT_ShadowObj*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObj"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	/* MRT_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PreFinal"), TEXT("Target_PreFinal"))))
		return E_FAIL;


	/* MRT_Bloom*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomBlur_X"), TEXT("Target_BloomBlur_X"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomBlur_Y"), TEXT("Target_BloomBlur_Y"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomEffect"), TEXT("Target_BloomEffect"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_OriginBloomEffect"), TEXT("Target_OriginBloomEffect"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_FinalBloomEffect"), TEXT("Target_FinalBloomEffect"))))
		return E_FAIL;

	
	
	
	/* MRT_Blur*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_MotionBlur"), TEXT("Target_MotionBlur"))))
		return E_FAIL;


	/* MRT_Refraction */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Refraction"), TEXT("Target_Refraction"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;


	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f));

	if (FAILED(Ready_LightDepthStencilView()))
		return E_FAIL;


#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_SRM"), 300.f, 500.f, 200.f, 200.f)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Height"), ViewportDesc.Width - 100.f, 100.f, 200.f, 200.f)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), ViewportDesc.Width - 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Final"), 500.f, ViewportDesc.Height - 100, 200.f, 200.f)))
		return E_FAIL;

	
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_X"), 700.f, ViewportDesc.Height - 100, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_Y"), 900.f, ViewportDesc.Height - 100, 200.f, 200.f)))
	//	return E_FAIL;

	

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_OriginBloomEffect"), 700.f, ViewportDesc.Height - 300, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_FinalBloomEffect"), 900.f, ViewportDesc.Height - 300, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Refraction"), 1100.f, ViewportDesc.Height - 300, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Distortion"), 1100.f, ViewportDesc.Height - 500, 200.f, 200.f)))
		return E_FAIL;

	


	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BloomBlur_X"), 700.f, ViewportDesc.Height - 100, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BloomBlur_Y"), 900.f, ViewportDesc.Height - 100, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BloomEffect"), 1100.f, ViewportDesc.Height - 100, 200.f, 200.f)))
		return E_FAIL;

#endif


	return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroupID, CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject ||
		eRenderGroupID >= RG_END)
		return E_FAIL;

	/* 실제 오브젝트 매니져에 추가된 객체의 주소를 여기에다가 공유했다. */
	m_RenderObjects[eRenderGroupID].emplace_back(pRenderObject);
	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	//if (FAILED(Render_Height()))
	//	return E_FAIL;

	
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;
	//if (FAILED(Render_ShadowObj()))
	//	return E_FAIL;
	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_BloomEffect()))
		return E_FAIL;

	if (FAILED(Render_Final()))				//  디퍼드연산까지 들어간 내 백버퍼를 Final 렌더타겟에 그린다.
		return E_FAIL;	

	// Final에서 불름효과를 먹인애들이 백버퍼와 함께 그려졌다  그 장면이 Final에 그려졌고  
	// 후에 모션블러와 리플렉션효과에 불름효과가 먹엇다

	if (true == m_isActiveBlur)
	{
		switch (m_eBlurType)
		{
		case Engine::CRenderer::GAUSSIAN_BLUR:
		{
			if (FAILED(Render_Blur()))
				return E_FAIL;
		}
		break;
		case Engine::CRenderer::MOTION_BLUR:
		{
			if (FAILED(Render_MotionBlur()))
				return E_FAIL;
		}
		break;
		case Engine::CRenderer::BLUR_END:
			break;
		default:
			break;
		}
	}


	if (0 == m_iMotionBlurCount % 2)
	{
		if (FAILED(Render_PreFinal()))
			return E_FAIL;
	}
	m_iMotionBlurCount = (m_iMotionBlurCount + 1) % 2;
	
	

	if (true == m_isActiveRefraction && FAILED(Render_Refraction()))	// 얘네들은 매프레임 호출될 필요가없어 내가 스킬을 쓸때만 호출하면되잖니
		return E_FAIL;

	
	if (true == m_isActiveDistortion && FAILED(Render_Distortion()))	// 얘네들은 매프레임 호출될 필요가없어 내가 스킬을 쓸때만 호출하면되잖니
		return E_FAIL;


	 
	if (FAILED(Render_NonLights()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

	
#ifdef _DEBUG
	static _bool bRenderTargetActive = true;
	if (m_pGameInstance->Get_KeyState(KEY::ENTER) == KEY_STATE::TAP)
	{
		if (true == bRenderTargetActive)
			bRenderTargetActive = false;
		else
			bRenderTargetActive = true;
	}

	if (true == bRenderTargetActive && FAILED(Render_Debug()))
		return E_FAIL;
#endif	

	return S_OK;
}

void CRenderer::ActiveRefraction(CTexture* pRefractionTex, REFRACTION_TYPE eRefractionType, class CTexture* pBlockMaskTex)
{
	m_isActiveRefraction = true;
	m_eRefractionType = eRefractionType;

	if (nullptr == m_pRefractionTex && nullptr != pRefractionTex)		
	{	
		m_pRefractionTex = pRefractionTex;
		Safe_AddRef(m_pRefractionTex);
	}

	if (nullptr == m_pBlockMaskTex && nullptr != pBlockMaskTex)
	{
		m_pBlockMaskTex = pBlockMaskTex;
		Safe_AddRef(m_pBlockMaskTex);
	}

}

void CRenderer::ActiveBlur(CTexture* pBlurMaskTex, BLUR_TYPE eBlurType)
{
	m_isActiveBlur = true;
	m_eBlurType = eBlurType;

	if (nullptr == m_pBlurMaskTex && nullptr != pBlurMaskTex)
	{	
		m_pBlurMaskTex = pBlurMaskTex;
		Safe_AddRef(m_pBlurMaskTex);
	}
}

void CRenderer::ActiveDistortion(CTexture* pDistortionTex, DISTORTION_TYPE eDistortionType, const _float4x4* pWorldMatrix)
{
	m_isActiveDistortion = true;
	m_eDistortionType = eDistortionType;

	if (nullptr == m_pDistortionTex && nullptr != pDistortionTex)
	{
		m_pDistortionTex = pDistortionTex;
		Safe_AddRef(m_pDistortionTex);
	}

	if (nullptr != pWorldMatrix)
	{
		m_pDistorionTexWorldMatrix = pWorldMatrix;
	}
}

void CRenderer::UnActiveRefraction()
{
	m_isActiveRefraction = false;
	m_fRefractAmount = 0.f;
	m_fRefractTime = 0.f;
	m_fStopTime = 0.f;
	m_isShutDownRefract = false;

	if (nullptr != m_pRefractionTex)
	{
		Safe_Release(m_pRefractionTex);
		m_pRefractionTex = nullptr;
	}	

	if (nullptr != m_pBlockMaskTex)
	{
		Safe_Release(m_pBlockMaskTex);
		m_pBlockMaskTex = nullptr;
	}
}

void CRenderer::UnActiveBlur()
{
	m_isActiveBlur = false;

	if (nullptr != m_pBlurMaskTex)
	{
		Safe_Release(m_pBlurMaskTex);
		m_pBlurMaskTex = nullptr;
	}
}

void CRenderer::UnActiveDistortion()
{
	m_isActiveDistortion = false;

	if (nullptr != m_pDistortionTex)
	{
		Safe_Release(m_pDistortionTex);
		m_pDistortionTex = nullptr;
	}

}



HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RG_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Height()
{
	return S_OK;
}

HRESULT CRenderer::Render_ShadowObj()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObj"), m_pLightDepthStencilView)))
		return E_FAIL;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Height = (_float)g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	for (auto& pGameObject : m_RenderObjects[RG_SHADOWOBJ])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_LightDepth();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_SHADOWOBJ].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.f;
	ViewPortDesc.Height = (_float)720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONBLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	/* Shade타겟을 바인딩하고 빛연산한 결과를 그려주겠다. */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;


	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 20.f, -15.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (KEY_STATE::TAP == m_pGameInstance->Get_KeyState(KEY::UP))
		m_pGameInstance->m_fToneTest += 0.05f;
	if (KEY_STATE::TAP == m_pGameInstance->Get_KeyState(KEY::DOWN))
		m_pGameInstance->m_fToneTest -= 0.05f;


	if (FAILED(m_pShader->Bind_RawValue("g_fToneTest", &m_pGameInstance->m_fToneTest,sizeof(_float))))
		return E_FAIL;
	

	if (FAILED(m_pGameInstance->Bind_HDRTexture(m_pShader)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_BRDFTexture(m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_LightDepth"), "g_LightDepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_SRM"), "g_SRMTexture")))
		return E_FAIL;


	m_pShader->Begin(3);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_PreFinal()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_PreFinal"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_BackTexture", m_pGameInstance->Get_BackBuffer_SRV())))
		return E_FAIL;

	m_pShader->Begin(4);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_BackTexture", m_pGameInstance->Get_BackBuffer_SRV())))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_FinalBloomEffect"), "g_BloomEffectTexture")))
		return E_FAIL;

	m_pShader->Begin(4);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;
	
	m_pShader->Begin(16);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();


	return S_OK;
}

HRESULT CRenderer::Render_BloomEffect()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_OriginBloomEffect"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_BLOOM])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_BLOOM].clear();


	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;



	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomBlur_X"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_OriginBloomEffect"), "g_BloomEffectTexture")))
		return E_FAIL;

	m_pShader->Begin(12);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomBlur_Y"))))		
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_BloomBlur_X"), "g_BlurXTexture")))
		return E_FAIL;

	m_pShader->Begin(13);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomEffect"))))			//  << 이제 업샘플링가자 
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_BloomBlur_Y"), "g_BlurYTexture")))
		return E_FAIL;

	m_pShader->Begin(14);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_FinalBloomEffect"))))			//  << 이제 업샘플링가자 
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_OriginBloomEffect"), "g_BloomOriginEffectTexture")))
		return E_FAIL;							
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_BloomEffect"), "g_BloomEffectTexture")))	// 클리어하기전에 값을 던져준다면 ?
		return E_FAIL;


	m_pShader->Begin(15); 

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;

	m_pShader->Begin(5);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Blur_X"), "g_BlurXTexture")))
		return E_FAIL;

	m_pShader->Begin(6);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Blur_Y"), "g_BlurYTexture")))
		return E_FAIL;

	m_pShader->Begin(7);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_MotionBlur()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_MotionBlur"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_PreFinal"), "g_PreFinalTexture")))
		return E_FAIL;

	m_pShader->Begin(9);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	m_pShader->Begin(9);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Refraction()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Refraction"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;


	if (nullptr != m_pRefractionTex)
	{
		if (FAILED(m_pRefractionTex->Bind_ShadeResource(m_pShader, "g_RefractionTex", 0)))
			return E_FAIL;
	}
	


	_vector vCamLook = m_pGameInstance->Find_Camera(3)->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	if (FAILED(m_pShader->Bind_RawValue("g_CamLook", &vCamLook, sizeof(_vector))))
		return E_FAIL;


	_uint iPassNum = {0};

	switch (m_eRefractionType)
	{
	case Engine::CRenderer::TIMESTOP:
	{
		if (m_fRefractAmount < 1.f)
			m_fRefractAmount += 0.02;
		iPassNum = 8;
	}		
		break;
	case Engine::CRenderer::MINDCONTROL:
	{
		m_fRefractTime += 0.016;

		//if (m_fRefractTime >= 0.2f)
		{
			if (!m_isShutDownRefract)
			{
				// RefractAmount를 0.33씩 증가시키고 최대 1.0까지 올라감
				if (m_fRefractAmount < 1.f)
				{
					m_fRefractAmount += 0.1f;
				}
				else
				{
					// 최대에 도달하면 감소하는 단계로 전환
					m_isShutDownRefract = true;
				}
			}
			else
			{
				if (m_fRefractAmount > 0.f)
				{
					m_fRefractAmount -= 0.1f;
				}
				else
				{
					m_isShutDownRefract = false;
				}
			}
			m_fRefractTime = 0.f;
		}
		
		iPassNum = 10;
	}		
		break;
	case Engine::CRenderer::BLOCK:
	{
		if (m_fRefractAmount < 1.f)
			m_fRefractAmount += 0.02;

		if (FAILED(m_pBlockMaskTex->Bind_ShadeResource(m_pShader, "g_BlockMaskTex", 0)))
			return E_FAIL;

		iPassNum = 17;
	}
		break;
	case Engine::CRenderer::SCREENSPLIT:
	{
		if (m_fRefractAmount < 0.3f)
		{
			m_fRefractAmount += 0.02;	
		}
		else
		{
			if (m_fStopTime >= 0.3)
			{
				m_fRefractAmount += 0.05;
			}
			m_fStopTime += 0.02;
		}
			
		if (FAILED(m_pShader->Bind_RawValue("g_SplitAmount", &m_fRefractAmount, sizeof(_float))))
			return E_FAIL;

	
		iPassNum = 18;
	}
		break;
	case Engine::CRenderer::REFRACTION_END:
		break;
	default:
		break;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_RefractAmount", &m_fRefractAmount, sizeof(_float))))
		return E_FAIL;


	m_pShader->Begin(iPassNum);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	m_pShader->Begin(iPassNum);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
		return E_FAIL;

	if (nullptr != m_pDistortionTex)
	{
		if (FAILED(m_pDistortionTex->Bind_ShadeResource(m_pShader, "g_DistortionTex", 0)))
			return E_FAIL;
	}

	switch (m_eDistortionType)
	{
	case Engine::CRenderer::WATERPUDDLE:
	{
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pDistorionTexWorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;
	}
		break;
	case Engine::CRenderer::DISTORTION_END:
		break;
	default:
		break;
	}

	m_pShader->Begin(19);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL; 


	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	m_fUVTime += 0.016f;

	if (FAILED(m_pShader->Bind_RawValue("g_fUVTime", &m_fUVTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Distortion"), "g_DistortionTex")))
		return E_FAIL;
	
	m_pShader->Begin(20);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLights()
{
	for (auto& pGameObject : m_RenderObjects[RG_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONLIGHT].clear();

	return S_OK;
}

//_bool Compare(CGameObject* pSour, CGameObject* pDest)
//{
//	return dynamic_cast<CBlendObject*>(pSour)->Get_ViewZ() > dynamic_cast<CBlendObject*>(pDest)->Get_ViewZ();
//}

HRESULT CRenderer::Render_Blend()
{
	m_RenderObjects[RG_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return dynamic_cast<CBlendObject*>(pSour)->Get_ViewZ() > dynamic_cast<CBlendObject*>(pDest)->Get_ViewZ();
		});

	for (auto& pGameObject : m_RenderObjects[RG_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	/*m_RenderObjects[RG_UI].sort();*/

	for (auto& pGameObject : m_RenderObjects[RG_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Ready_LightDepthStencilView()
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugObjects)
	{
		pDebugCom->Render();

		Safe_Release(pDebugCom);
	}

	m_DebugObjects.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer);

	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_ShadowObj"), m_pShader, m_pVIBuffer);

	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Final"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Blur_X"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Blur_Y"), m_pShader, m_pVIBuffer);

	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Refraction"), m_pShader, m_pVIBuffer);

	
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_OriginBloomEffect"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_FinalBloomEffect"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Refraction"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Distortion"), m_pShader, m_pVIBuffer);
	

	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_BloomBlur_X"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_BloomBlur_Y"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_BloomEffect"), m_pShader, m_pVIBuffer);

	return S_OK;
}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pGameObject : RenderObjects)
			Safe_Release(pGameObject);

		RenderObjects.clear();
	}


	Safe_Release(m_pRefractionTex);
	Safe_Release(m_pBlockMaskTex);
	Safe_Release(m_pBlurMaskTex);
	Safe_Release(m_pDistortionTex);


	Safe_Release(m_pLightDepthStencilView);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
