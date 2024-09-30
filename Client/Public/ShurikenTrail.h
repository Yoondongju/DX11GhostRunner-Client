#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "VIBuffer_Trail.h"

BEGIN(Engine)
class CShader;
class CTexture;
END


BEGIN(Client)

class CShurikenTrail final : public CPartObject
{
public:
	enum SHURIKEN_TYPE { MAIN , SUB0, SUB1 , SHURIKEN_TYPE_END};


public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		SHURIKEN_TYPE    eType = { SHURIKEN_TYPE_END };

	}EFFECT_DESC;



private:
	CShurikenTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShurikenTrail(const CShurikenTrail& Prototype);
	virtual ~CShurikenTrail() = default;


public:
	void		Set_Active(_bool b) { m_bActive = b; }

public:
	deque<CVIBuffer_Trail::TRAIL_INFO>&			Get_Trail() { return m_Trail; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta , const _float4x4* pWeaponMatrix);
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader*		 m_pShaderCom = { nullptr };
	CTexture*		 m_pTextureCom = { nullptr };
	CTexture*		 m_pDissolveTextureCom = { nullptr };


	CVIBuffer_Trail* m_pVIBufferCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };

	deque<CVIBuffer_Trail::TRAIL_INFO>	m_Trail;


private:
	_bool				m_bActive = { false };
	_float3				m_StartPos = {};		// 처음 활성화되고 켜진 거리


	_float3				m_ShurikenStartLocal = _float3(-0.0199559722 , -0.481096238 , -0.517797530);
	_float3				m_ShurikenEndLocal = _float3(0.0199559722 , 0.481096268 , 0.517797470);

	_float				m_fDissolveAmount = { 0.f };




private:
	HRESULT Ready_Components(SHURIKEN_TYPE eType);

public:
	static CShurikenTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END