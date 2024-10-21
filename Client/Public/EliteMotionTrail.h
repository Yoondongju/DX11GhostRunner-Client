#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "VIBuffer_Trail.h"

BEGIN(Engine)
class CShader;
class CTexture;
END


BEGIN(Client)

class CEliteMotionTrail final : public CGameObject
{
public:
	typedef struct
	{
		_float4x4			WorldMatrix = {};
		_float4x4			BoneMatrices[3][g_iMaxMeshBones] = {};

		_float				fLifeTime = {};

	}MOTION_TRAIL_INFO;

private:
	CEliteMotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEliteMotionTrail(const CEliteMotionTrail& Prototype);
	virtual ~CEliteMotionTrail() = default;


public:
	_bool		Is_Active() { return m_bActive; }
	void		Set_Active(_bool b)
	{
		m_bActive = b;
	}


public:
	deque<MOTION_TRAIL_INFO>& Get_MotionTrailInfo() { return m_MotionTrailInfo; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModel = { nullptr };

	deque<MOTION_TRAIL_INFO>	m_MotionTrailInfo;


private:
	_bool				m_bActive = { false };
	_float				m_fAccTime = { 0.f };


private:
	HRESULT Ready_Components();

public:
	static CEliteMotionTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END