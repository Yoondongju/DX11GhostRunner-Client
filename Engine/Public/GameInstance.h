#pragma once

#include "Component_Manager.h"
#include "Renderer.h"

#include "PipeLine.h"
#include "PhysXManager.h"

/* 1. 엔진과 클라이언트의 소통을 위한 클래스읻. */
/* 2. 엔진에서 클라이언트에 보여주고 싶은 함수들을 모아놓는다. */
/* 3. 함수들 -> 클래스의 멤버함수. 객체가 필요하다! 그래서 기능응ㄹ 제공해주기위한 클래스 객체를 보관한다. */
/* 4. 객체들을 보관하기위해 Initialize_Engine() 함수 안에서 기능을 제공해주기위한 클래스들을 객체화한다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	POINT Get_MosePos();

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);

	void FinalUpdate_Engine();

	HRESULT Draw_Engine();
	HRESULT Clear(_uint iLevelIndex);
	
#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin();
	void Render_End();
#pragma endregion

#pragma region GRAPHIC_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);	
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, const _wstring& strModelPrototypeTag ,  void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Clone_GameObject(const _wstring & strPrototypeTag, void* pArg = nullptr);

	class CGameObject* Find_Player();
	class CGameObject* Find_Camera();

	list<class CGameObject*>& Get_GameObjects(_uint iLevelIndx, const _wstring & strLayerTag);

#pragma endregion


#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr );

	_bool	IsFind_Model(_uint iLevelIndex, const _wstring & strModelPrototypeName);
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);	
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	list<class CGameObject*>& Get_RenderList(CRenderer::RENDERGROUP eGroup);
#pragma endregion


#pragma region KEY_MANAGER
	KEY_STATE	Get_KeyState(KEY _eKey);
#pragma endregion

#pragma region EVENT_MANAGER
	void Delete(_uint iLevelIndex, CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pObj);
#pragma endregion


#pragma region PIPELINE
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;

	_float4 Get_CamPosition_Float4() const;
	_vector Get_CamPosition_Vector() const;
#pragma endregion

#pragma region PICKING
	//void Transform_MouseRay_ToLocalSpace(const _float4x4& WorldMatrix);	
	//_bool isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);

#pragma endregion

#pragma region PHYSX_MANAGER
	PxPhysics* Get_Physics();
	PxScene* Get_Scene();
	PxMaterial* Get_Material();

	HRESULT	Add_WalkAble_Mesh(const CPhysXManager::PLAYER_WALKABLE_MESH & WalkAbleMesh);
	_bool CollisionUpdate_PlayerToTriangleMeshGeometry(PxVec3 * pOutDir, PxReal * pOutDepth, PxShape * pPlayerShape, PxTransform * pPlayerTransform);
#pragma endregion



private:
	HWND							m_hWnd = {};

	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };



	/* 여기서부터 우리가 추가한 기능들*/
	class CKeyManager*				m_pKey_Manager = { nullptr };
	class CEvent_Manager*			m_pEvent_Manager = { nullptr };
	class CUI_Manager*				m_pUI_Manager = { nullptr };
	//class CPickingManager*			m_pPicking_Manager = { nullptr };

	class CPhysXManager*			m_pPhysX_Manager = { nullptr };


public:	
	void Release_Engine();
	virtual void Free() override;
};

END