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



private:
	_float				m_fSensor = { 0.f };

private: 
	_float4				m_OffsetByPlayer = {};
	_float4				m_OffsetByShuriken = {};


private:
	void				FollowPlayer(class CPlayer* pPlayer);




public:
	static CFreeCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END