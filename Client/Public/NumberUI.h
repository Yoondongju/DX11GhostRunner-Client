#pragma once
#include "Client_Defines.h"
#include "UIObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CNumberUI final : public CUIObject
{
private:
	CNumberUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNumberUI(const CNumberUI& Prototype);
	virtual ~CNumberUI() = default;


public:
	_float						Get_Alpha() { return m_fAlpha; }


	void						Set_Number(_int iNumber) { m_iNumber = iNumber; }

	


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float			m_fAlpha = { 0.8f };


	_float2			m_vTargetSize = {};
	_float2			m_vOriginSize = {};

	_int			m_iPreNumber = {};		// 이전 숫자
	_int			m_iNumber = {};

	_bool			m_bChangeEnter = { false };


private:
	HRESULT Ready_Components();


private:



public:
	static CNumberUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

