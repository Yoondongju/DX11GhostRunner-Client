#pragma once

#include "Base.h"

/* 1. 객체들을 그리는 순서에 따라서 보관하고. 정해진 순서대로 객체들의 렌더함수를 호출한다.  */
/* 2. 3D라고 하더라도 객체들을 그리는 순서가 상당히 중요하다. */
/* 3. 하늘, 블렌딩, UI관련된 객체들은 그리는 순서의 관리가 필수적으로 필요하다. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RG_PRIORITY, RG_HEIGHT, RG_SHADOWOBJ, RG_NONBLEND, RG_NONLIGHT, RG_BLOOM , RG_BLEND, RG_UI, RG_END };
	enum BLUR_TYPE { GAUSSIAN_BLUR, MOTION_BLUR, BLUR_END};
	enum REFRACTION_TYPE { TIMESTOP, MINDCONTROL, BLOCK, SCREENSPLIT, REFRACTION_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	HRESULT Draw();

public:
	void	ActiveRefraction(class CTexture* pRefractionTex, REFRACTION_TYPE eRefractionType , class CTexture* pBlockMaskTex = nullptr);
	void	ActiveBlur(class CTexture* pBlurMaskTex , BLUR_TYPE eBlurType);

	void	UnActiveRefraction();
	void	UnActiveBlur();



public:
	list<class CGameObject*>& Get_RenderList(RENDERGROUP eRenderGroupID) { return m_RenderObjects[eRenderGroupID]; }

#ifdef _DEBUG
public:
	HRESULT Add_DebugObject(class CComponent* pDebugObject) {
		Safe_AddRef(pDebugObject);
		m_DebugObjects.push_back(pDebugObject);
		return S_OK;
	}
#endif

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11DepthStencilView*		m_pLightDepthStencilView = { nullptr };
	list<class CGameObject*>	m_RenderObjects[RG_END];
	class CGameInstance*		m_pGameInstance = { nullptr };

private:
	class CShader*				m_pShader = { nullptr };
	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };

	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

	
private:
	_bool						m_isActiveRefraction = { false };
	_bool						m_isActiveBlur = { false };

	_float						m_fRefractAmount = { 0.f };
	_float						m_fRefractTime = { 0.f };
	_float						m_fStopTime = { 0.f };

	_bool						m_isShutDownRefract = { false };

	class CTexture*				m_pRefractionTex = { nullptr };
	class CTexture*				m_pBlockMaskTex = { nullptr };

	class CTexture*				m_pBlurMaskTex = { nullptr };


	BLUR_TYPE					m_eBlurType = { BLUR_TYPE::BLUR_END };
	REFRACTION_TYPE				m_eRefractionType = { REFRACTION_TYPE::REFRACTION_END };


#ifdef _DEBUG
private:
	list<class CComponent*>		m_DebugObjects;
#endif




private:
	HRESULT Render_Priority();
	HRESULT Render_Height();
	HRESULT Render_ShadowObj();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_Deferred();


	HRESULT Render_PreFinal();
	HRESULT Render_Final();			// 디퍼드 연산까지 끝난 최종 나의 백버퍼를 Final 렌더타겟에 그림


	HRESULT Render_BloomEffect();	// Bloom 효과


	HRESULT Render_Blur();
	HRESULT Render_MotionBlur();
	HRESULT RenderScreenSplit();	// 반으로 갈라지는듯한 연출

	HRESULT Render_Refraction();	// 굴절 효과	



	HRESULT Render_NonLights();
	HRESULT Render_Blend();
	HRESULT Render_UI();

private:
	HRESULT Ready_LightDepthStencilView();



#ifdef _DEBUG
private:
	HRESULT Render_Debug();

#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END