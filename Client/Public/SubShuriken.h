#pragma once

#include "Client_Defines.h"
#include "Weapon_Player.h"





BEGIN(Client)

class CSubShuriken final : public CWeapon_Player
{
private:
	CSubShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSubShuriken(const CSubShuriken& Prototype);
	virtual ~CSubShuriken() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



private:
	HRESULT Ready_Components();

public:
	static CSubShuriken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END