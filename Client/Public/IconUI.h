#pragma once
#include "Client_Defines.h"
#include "UIObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CIconUI final : public CUIObject
{
public:
	enum ICON_TYPE { BLOCK, SHURIKEN, DASH, MINDCONTROL, NAMI, CUTALL, TIMESTOP, ICON_TYPE_END };
	
	typedef struct : public CUIObject::UI_DESC
	{
		ICON_TYPE	eType = { ICON_TYPE_END };

	}ICON_DESC;


private:
	CIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIconUI(const CIconUI& Prototype);
	virtual ~CIconUI() = default;


public:
	_float						Get_Alpha() { return m_fAlpha; }


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

	_float			m_fAlpha = { 1.f };

	ICON_TYPE		m_eType = { ICON_TYPE_END };

	class CPlayer*  m_pPlayer = { nullptr };


	_float2			m_vTargetSize = {};
	_float2			m_vOriginSize = {};
	
	_bool			m_bChangeEnter = { false };
	_float4			m_clickedColor = _float4(1.0f, 0.6f, 0.f, 1.0f);

private:
	HRESULT Ready_Components();


private:



public:
	static CIconUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

