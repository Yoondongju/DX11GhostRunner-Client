#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"

#include "GameObject.h"

#include "../Public/KeyManager.h"
#include "../Public/UI_Manager.h"
#include "../Public/Event_Manager.h"
#include "../Public/PhysXManager.h"
#include "../Public/Light_Manager.h"
#include "../Public/Font_Manager.h"
#include "../Public/Target_Manager.h"
#include "../Public/SoundMager.h"
#include "../Public/Frustum.h"
#include "../Public/Sky_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

POINT CGameInstance::Get_MosePos()
{
	POINT		ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	return ptMouse;
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowsed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_hWnd = EngineDesc.hWnd;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;



	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	//m_pPicking = CPicking::Create(*ppDevice, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	//if (nullptr == m_pPicking)
	//	return E_FAIL;

	/* 여러가지 매니져를 초기화하낟. */
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;


	m_pEvent_Manager = CEvent_Manager::Create(m_pObject_Manager);
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	m_pKey_Manager = CKeyManager::Create(m_hWnd);
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pUI_Manager = CUI_Manager::Create();
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;


	m_pPhysX_Manager = CPhysXManager::Create();
	if (nullptr == m_pPhysX_Manager)
		return E_FAIL;

	m_pSky_Manager = CSky_Manager::Create();
	if (nullptr == m_pSky_Manager)
		return E_FAIL;


	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* 현재 키보드와 마우스의 상태를 받아올꺼야. */
	//m_pInput_Device->Update();

	m_pKey_Manager->Update();
	m_pPipeLine->Update();
	m_pFrustum->Update();

	_float fAdjustedDeltaTime = fTimeDelta;
	if (true == m_bTimeDelayActive)
	{
		fAdjustedDeltaTime *= 0.15f;

		if (-1.f != m_fTimeDelayDuration &&  m_fTimeDelayDuration <= 0.f)
		{
			fAdjustedDeltaTime = fTimeDelta;
			m_fTimeDelayDuration = -1.f;
			m_bTimeDelayActive = false;
		}

		if (-1.f != m_fTimeDelayDuration)
		{
			m_fTimeDelayDuration -= fTimeDelta;
		}
	}
		


	m_pObject_Manager->Priority_Update(fAdjustedDeltaTime);
	m_pObject_Manager->Update(fAdjustedDeltaTime);
	m_pObject_Manager->Late_Update(fAdjustedDeltaTime);
	

	m_pPhysX_Manager->Update(fTimeDelta);


	m_pLevel_Manager->Update(fTimeDelta);

	//UIManager는 항상 Render 되기 전에 호출
	m_pUI_Manager->Update();

}

void CGameInstance::FinalUpdate_Engine()
{
	m_pEvent_Manager->update();
}

HRESULT CGameInstance::Draw_Engine()
{
	if(1 != m_pLevel_Manager->Get_CurLevelIndex())
		m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	/* 특정 레벨을 위해 존재하는 자원들은 해당 레벨이 끝나면 삭제하자. */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 원형들도 레벨별로 관리했었다. */
	m_pComponent_Manager->Clear(iLevelIndex);

	return S_OK;
}

ID3D11ShaderResourceView* CGameInstance::Get_BackBuffer_SRV() const
{
	return m_pGraphic_Device->Get_BackBuffer_SRV();
}

void CGameInstance::Render_Begin()
{
	/*m_pGraphic_Device->Render_Begin();*/
	m_pGraphic_Device->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();
	
}

void CGameInstance::Render_End()
{
	/*m_pGraphic_Device->Render_End(hWnd);*/

	m_pGraphic_Device->Present();
}
#pragma region INPUT_DEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);	
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel * pNextLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNextLevel);	
}

_uint CGameInstance::Get_CurLevelIndex()
{
	return m_pLevel_Manager->Get_CurLevelIndex();
}

#pragma endregion

#pragma region OBJECT_MANAGER

HRESULT CGameInstance::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}


HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, const _wstring& strModelPrototypeTag , void* pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, strModelPrototypeTag, pArg);
}

CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);	
}

CGameObject* CGameInstance::Clone_GameObject(const _wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Object(iLevelIndex,strLayerTag,iIndex);
}

CGameObject* CGameInstance::Find_Player(_uint iLevelIndex)
{
	return m_pObject_Manager->Find_Player(iLevelIndex);
}

CGameObject* CGameInstance::Find_Camera(_uint iLevelIndex)
{
	return m_pObject_Manager->Find_Camera(iLevelIndex);
}

CGameObject* CGameInstance::Find_Notify(_uint iLevelIndex)
{
	return m_pObject_Manager->Find_Notify(iLevelIndex);
}

list<class CGameObject*>& CGameInstance::Get_GameObjects(_uint iLevelIndx, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_GameObjects(iLevelIndx, strLayerTag);
}

#pragma endregion

#pragma region COMPONENT_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag, CComponent * pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

CComponent* CGameInstance::Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	return m_pComponent_Manager->Find_Prototype(iLevelIndex, strPrototypeTag);
}

_bool CGameInstance::IsFind_Model(_uint iLevelIndex, const _wstring& strModelPrototypeName)
{
	return m_pComponent_Manager->IsFind_Model(iLevelIndex, strModelPrototypeName);
}

#pragma endregion

#pragma region TIMER_MANAGER

HRESULT CGameInstance::Add_Timer(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion



void CGameInstance::Set_HDRTexture(CTexture* pTexture)
{
	return m_pSky_Manager->Set_HDRTexture(pTexture);
}

void CGameInstance::Set_BRDFTexture(CTexture* pTexture)
{
	return m_pSky_Manager->Set_BRDFTexture(pTexture);
}

void CGameInstance::Set_DissolveTexture(CTexture* pTexture)
{
	return m_pSky_Manager->Set_DissolveTexture(pTexture);
}

void CGameInstance::Set_NoiseTexture(CTexture* pTexture)
{
	return m_pSky_Manager->Set_NoiseTexture(pTexture);
}

HRESULT CGameInstance::Bind_HDRTexture(CShader* pShader)
{
	return m_pSky_Manager->Bind_HDRTexture(pShader);
}

HRESULT CGameInstance::Bind_BRDFTexture(CShader* pShader)
{
	return m_pSky_Manager->Bind_BRDFTexture(pShader);
}

HRESULT CGameInstance::Bind_DissolveTexture(CShader* pShader)
{
	return m_pSky_Manager->Bind_DissolveTexture(pShader);
}

HRESULT CGameInstance::Bind_NoiseTexture(CShader* pShader, const _char* pConstantName)
{
	return m_pSky_Manager->Bind_NoiseTexture(pShader, pConstantName);
}






#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject * pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}

list<class CGameObject*>& CGameInstance::Get_RenderList(CRenderer::RENDERGROUP eGroup)
{
	return m_pRenderer->Get_RenderList(eGroup);
}

void CGameInstance::ActiveRefraction(CTexture* pRefractionTex ,CRenderer::REFRACTION_TYPE eRefractionType, class CTexture* pBlockMaskTex)
{
	m_pRenderer->ActiveRefraction(pRefractionTex, eRefractionType , pBlockMaskTex);
}

void CGameInstance::ActiveBlur(CTexture* pBlurMaskTex , CRenderer::BLUR_TYPE eBlurType)
{
	m_pRenderer->ActiveBlur(pBlurMaskTex , eBlurType);
}

void CGameInstance::ActiveDistortion(CTexture* pDistortionTex, CRenderer::DISTORTION_TYPE eDistortionType, const _float4x4* pWorldMatrix)
{
	m_pRenderer->ActiveDistortion(pDistortionTex, eDistortionType, pWorldMatrix);
}

void CGameInstance::UnActiveRefraction()
{
	m_pRenderer->UnActiveRefraction();
}

void CGameInstance::UnActiveBlur()
{
	m_pRenderer->UnActiveBlur();
}

void CGameInstance::UnActiveDistortion()
{
	m_pRenderer->UnActiveDistortion();
}


#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugObject(CComponent* pDebugObject)
{
	return m_pRenderer->Add_DebugObject(pDebugObject);
}
#endif
#pragma endregion





KEY_STATE CGameInstance::Get_KeyState(KEY _eKey)
{
	return m_pKey_Manager->Get_KeyState(_eKey);
}

void CGameInstance::Delete(_uint iLevelIndex, CRenderer::RENDERGROUP eRenderGroup, CGameObject* pObj)
{
	CEvent_Manager::EVENT eEvent{};
	eEvent.eEven = CEvent_Manager::EVENT_TYPE::DELETE_OBJECT;
	eEvent.lParam = (DWORD_PTR)pObj;
	eEvent.wParam = iLevelIndex;

	m_pEvent_Manager->AddEvent(eEvent);
}





#pragma region PIPELINE
void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}
_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}
_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}
_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

void CGameInstance::Light_Clear()
{
	m_pLight_Manager->Clear();
}

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _float fScale, _fvector vColor, _float fRadian, _fvector vPivot)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, fScale ,  vColor, fRadian, vPivot);
}


HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag , pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strTargetTag, pConstantName);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_MRT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render(strMRTTag, pShader, pVIBuffer);
}
#endif

#pragma endregion



PxPhysics* CGameInstance::Get_Physics()
{
	return m_pPhysX_Manager->Get_Physics();
}

PxScene* CGameInstance::Get_Scene()
{
	return m_pPhysX_Manager->Get_Scene();
}

PxMaterial* CGameInstance::Get_Material()
{
	return m_pPhysX_Manager->Get_Material();
}

HRESULT CGameInstance::Add_WalkAble_Mesh(const CPhysXManager::PLAYER_WALKABLE_MESH& WalkAbleMesh)
{
	return m_pPhysX_Manager->Add_WalkAble_Mesh(WalkAbleMesh);
}

_bool CGameInstance::CollisionUpdate_PlayerToTriangleMeshGeometry(PxVec3* pOutDir, PxReal* pOutDepth, PxShape* pPlayerShape, PxTransform* pPlayerTransform, CGameObject** pCollTarget)
{
	return m_pPhysX_Manager->CollisionUpdate_PlayerToTriangleMeshGeometry(pOutDir, pOutDepth, pPlayerShape, pPlayerTransform, pCollTarget);
}

void CGameInstance::Phys_Clear()
{
	m_pPhysX_Manager->Clear();
	
}


void CGameInstance::Release_Engine()
{		
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);

	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPipeLine);
	//Safe_Release(m_pPicking_Manager);
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);

	Safe_Release(m_pSky_Manager);

	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);

	
	CGameInstance::Get_Instance()->Destroy_Instance();	


	Safe_Release(m_pPhysX_Manager);

}

void CGameInstance::Free()
{	
	__super::Free();

}

_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->isIn_WorldSpace(vPosition, fRadius);
}

_bool CGameInstance::isIn_Frustum_LocalSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->isIn_LocalSpace(vPosition, fRadius);
}

void CGameInstance::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	return m_pFrustum->Transform_ToLocalSpace(WorldMatrix);
}

void CGameInstance::Play_Sound(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
     m_pSound_Manager->PlaySound(pSoundKey, eID, fVolume);
}

void CGameInstance::Play_Sound(const TCHAR* pSoundKey, _uint eID, _float* pVolume)
{
	m_pSound_Manager->PlaySound(pSoundKey, eID, pVolume);
}

void CGameInstance::Play_SoundRepeat(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
	m_pSound_Manager->Play_SoundRepeat(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(const TCHAR* pSoundKey, _float fVolume)
{
	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(_uint eID)
{
	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(_uint eID, _float fVolume)
{
	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

void CGameInstance::SetPlayeSpeed(_uint eID, _float fSpeedRatio)
{
	m_pSound_Manager->SetPlayeSpeed(eID, fSpeedRatio);
}

void CGameInstance::Pause(_uint eID)
{
	m_pSound_Manager->Pause(eID);
}

_bool CGameInstance::Check_IsPlaying(_uint eID)
{
	return m_pSound_Manager->Check_IsPlaying(eID);
}
