#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "VIBuffer_Trail.h"

BEGIN(Engine)
class CShader;
class CTexture;
END


BEGIN(Client)

class CSwordTrail final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };

	}EFFECT_DESC;



private:
	CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwordTrail(const CSwordTrail& Prototype);
	virtual ~CSwordTrail() = default;


public:
	void		Set_Active(_bool b) { m_bActive = b; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader*					 m_pShaderCom = { nullptr };
	CTexture*					 m_pTextureCom = { nullptr };
	CVIBuffer_Trail*			 m_pVIBufferCom = { nullptr };

private:
	const _float4x4*	m_pSocketMatrix = { nullptr };

	deque<CVIBuffer_Trail::TRAIL_INFO>	m_Trail;


private:
	_bool				m_bActive = { false };
	_float3				m_StartPos = {};		// 처음 활성화되고 켜진 거리

	_float3				m_KatanaStartLocal = _float3(-0.0776796862, -0.202111647, 0.839834869);
	_float3				m_KatanaEndLocal = _float3(0.0776796862, 1.43591797, 9.31558323);


	_float3				m_ShurikenStartLocal = _float3(0.f,0.f,0.f);  //_float3(-0.0199559722 , -0.481096238 , -0.517797530);
	_float3				m_ShurikenEndLocal = _float3(0.f, 0.f, 2.f);  //_float3(0.0199559722 , 0.481096268 , 0.517797470);

	_float				m_fAccTime = { 0.f };




private:
	HRESULT Ready_Components();

public:
	static CSwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END