#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CFreeCamera final : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{
		_float		fSensor;
	}CAMERA_FREE_DESC;
private:
	CFreeCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFreeCamera(const CFreeCamera& Prototype);
	virtual ~CFreeCamera() = default;

public:
	_float4	 Get_OffsetByPlayer() { return m_OffsetByPlayer; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void				Set_FollowPlayer(_bool b) { m_isActiveFollowPlayer = b; }

	void				FollowElite(class CElite* pElite);
	_float4&			Get_OffsetByEliteRef() { return m_OffsetByElite; }

	void				FollowHel(class CHel* pHel);
	_float4&			Get_OffsetByHelRef() { return m_OffsetByHel; }



public:
	void    Set_Shake(_uint iShakeCount, _float fForce, _fvector vAsix)
	{
		if (iShakeCount % 2 != 0)
		{
			iShakeCount += 1;
		}
		else if (iShakeCount == 0)
			iShakeCount = 2;


		m_iShakingCount = iShakeCount;
		m_iOriginShakingCount = m_iShakingCount;

		m_bShaking = true;
		m_fForce = fForce;
		XMStoreFloat3(&m_ShakeAsix, vAsix);
	}
	void	Active_Shake(_float fForce, _fvector vAsix);


private:
	_bool				m_bShaking = false;
	_uint				m_iOriginShakingCount = { 0 };
	_int				m_iShakingCount = 0;

	_float				m_fForce = { 0.f };			// 쉐이킹에서 사용할 회전힘의 강도
	_float				m_fCurRotation = { 0.f };	// 쉐이킹에서 사용할 회전값
	_float				m_fPreRotation = { 0.f };	// 쉐이킹에서 사용할 회전값

	_float3				m_ShakeAsix = {};


private:
	_float				m_fSensor = { 0.f };

private: 
	_float4				m_OffsetByPlayer = {};
	_float4				m_OffsetByElite = {};
	_float4				m_OffsetByHel = {};


	_float4				m_OffsetByShuriken = {};


	_bool				m_isActiveFollowPlayer = { true };


private:
	void				FollowPlayer(class CPlayer* pPlayer);
	



public:
	static CFreeCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END