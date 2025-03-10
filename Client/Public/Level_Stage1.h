#pragma once

#include "Client_Defines.h"


#include "Level.h"

BEGIN(Client)

class CLevel_Stage1 final : public CLevel
{
private:
	CLevel_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Stage1() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();



	HRESULT Ready_Layer_BackGround(void* pArg);

	HRESULT Ready_Layer_Effect();
	HRESULT Ready_Layer_MapObject(void* pArg);
	HRESULT Ready_Layer_Monster(void* pArg);


	HRESULT Ready_Layer_Player(void* pArg);

	HRESULT Ready_Layer_UI();


public:
	static CLevel_Stage1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END
