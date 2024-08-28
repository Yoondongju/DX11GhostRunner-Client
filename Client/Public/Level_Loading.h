#pragma once

#include "Client_Defines.h"
#include "Level.h"

#include "Loader.h"

/* 1. 로딩화면을 구성하고 갱신하여 렌더링한다. */
/* 2. 스레드를 생성하여 다음레벨의 자원을 로드한다.(CLoader라를 객체가 수행한다.) */
/* 2-1. 위의 기능을 수행하는 로더 객체를 생성하고 삭제한다. */

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
public:
	typedef struct
	{
		_uint								  iDataSize = { 0 };
		const CLoader::LOADING_OBJECT_INFO*   pData = { nullptr };

		_uint								  iAnimDataSize = { 0 };
		const CLoader::LOADING_OBJECT_INFO*	  pAnimData = { nullptr };

		_uint								  iTerrainDataSize = { 0 };
		const CLoader::LOADING_TERRAIN_INFO*  pTerrainData = { nullptr };


		const CLoader::LOADING_OBJECT_INFO*   pPlayerData = { nullptr };

	}LOAD_DATA_DESC;

private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVELID						m_eNextLevelID = { LEVEL_END };
	CLoader*					m_pLoader = { nullptr };
		
private:
	HRESULT Ready_Layer_BackGround();

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END
