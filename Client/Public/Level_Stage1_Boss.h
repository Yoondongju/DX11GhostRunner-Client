#pragma once

#include "Client_Defines.h"


#include "Level.h"

BEGIN(Client)

class CLevel_Stage1_Boss final : public CLevel
{
private:
	CLevel_Stage1_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Stage1_Boss() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	_float			m_fTrainSoundAmountTime = { 0.f };
	_float			m_fTrainSoundAmount = { 0.f };

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(void* pArg);

	HRESULT Ready_Layer_MapObject(void* pArg);
	HRESULT Ready_Layer_Monster(void* pArg);




public:
	static CLevel_Stage1_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END
