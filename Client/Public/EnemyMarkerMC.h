#pragma once
#include "Client_Defines.h"
#include "UIObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CEnemyMarkerMC final : public CUIObject
{
public:
	typedef struct : public CUIObject::UI_DESC
	{
		class CEnemy* pOwner = { nullptr };

	}ENEMYMARKERMC_DESC;

private:
	CEnemyMarkerMC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemyMarkerMC(const CEnemyMarkerMC& Prototype);
	virtual ~CEnemyMarkerMC() = default;


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
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CEnemy* m_pOwner = { nullptr };


	_float			m_fAlpha = { 1.f };
	_float2			m_vTargetSize = {};
	_float2			m_vOriginSize = {};


	_bool			m_bChangeEnter = true;
	_bool			m_isTaskDone = { false };
	_float4			m_clickedColor = _float4(1.0f, 0.f, 0.f, 1.0f);







private:
	HRESULT Ready_Components();


private:



public:
	static CEnemyMarkerMC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

