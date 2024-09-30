#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
public:
	typedef struct LOADING_OBJECT_INFO
	{
		_wstring	strPrototypeName = L"";
		_wstring	strLayerName = L"";
		_wstring	strModelPrototypeName = L"";

		_float4x4   vWorldMatrix = {};

		

		vector<LOADING_OBJECT_INFO> PartsInfo;


		MODEL_CHECK_LIST eModelType = {MODEL_CHECK_LIST::MODEL_CHECK_TYPE_END };

	}LOADING_OBJECT_INFO;

	typedef struct
	{
		_wstring	strPrototypeName = L"";
		_wstring	strModelPrototypeName = L"";
		_float4x4   vWorldMatrix = {};

	}LOADING_TERRAIN_INFO;


private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;



public:
	_bool isFinished() const {
		return m_isFinished;
	}


	const LOADING_OBJECT_INFO* Get_Loading_DecorativeObject_Info(LEVELID eLevelID) { return m_pLoadingDecorativeObjectInfo[eLevelID]; }
	const _uint				   Get_Loading_DecorativeObject_Size(LEVELID eLevelID) { return m_iNumLoadingDecorativeObject[eLevelID]; }

	const LOADING_OBJECT_INFO* Get_Loading_StaticObject_Info(LEVELID eLevelID) { return m_pLoadingStaticObjectInfo[eLevelID]; }
	const _uint				   Get_Loading_StaticObject_Size(LEVELID eLevelID) { return m_iNumLoadingStaticObject[eLevelID]; }

	const LOADING_OBJECT_INFO* Get_Loading_DynamicObject_Info(LEVELID eLevelID) { return m_pLoadingDynamicObjectInfo[eLevelID]; }
	const _uint				   Get_Loading_DynamicObject_Size(LEVELID eLevelID) { return m_iNumLoadingDynamicObject[eLevelID]; }





	const LOADING_OBJECT_INFO* Get_Loading_AnimObject_Info(LEVELID eLevelID) { return m_pLoadingAnimObjectInfo[eLevelID]; }
	const _uint				   Get_Loading_AnimObject_Size(LEVELID eLevelID) { return m_iNumLoadingAnimObject[eLevelID]; }


	const LOADING_TERRAIN_INFO* Get_Loading_Terrain_Info(LEVELID eLevelID) { return m_pLoadingTerrainInfo[eLevelID]; }
	const _uint				    Get_Loading_Terrain_Size(LEVELID eLevelID) { return m_iNumLoadingTerrain[eLevelID]; }

	const LOADING_OBJECT_INFO*  Get_Loading_Player_Info(LEVELID eLevelID) { return m_pLoadingPlayerInfo[eLevelID]; }


public:
	HRESULT Initialize(LEVELID eNextLevelID);
	HRESULT Loading();
	void Draw_LoadingText();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CGameInstance*				m_pGameInstance = { nullptr };

	LEVELID						m_eNextLevelID = { LEVEL_END };

	HANDLE						m_hThread = {};
	CRITICAL_SECTION			m_CriticalSection = {};
	_bool						m_isFinished = { false };
	_tchar						m_szLoadingText[MAX_PATH] = {};


						
	_uint						m_iNumLoadingDecorativeObject[LEVELID::LEVEL_END] = { 0 };
	LOADING_OBJECT_INFO*		m_pLoadingDecorativeObjectInfo[LEVELID::LEVEL_END] = { nullptr };  

	_uint						m_iNumLoadingStaticObject[LEVELID::LEVEL_END] = { 0 };
	LOADING_OBJECT_INFO*		m_pLoadingStaticObjectInfo[LEVELID::LEVEL_END] = { nullptr };

	_uint						m_iNumLoadingDynamicObject[LEVELID::LEVEL_END] = { 0 };
	LOADING_OBJECT_INFO*		m_pLoadingDynamicObjectInfo[LEVELID::LEVEL_END] = { nullptr };




	_uint						m_iNumLoadingAnimObject[LEVELID::LEVEL_END] = { 0 };
	LOADING_OBJECT_INFO*		m_pLoadingAnimObjectInfo[LEVELID::LEVEL_END] = { nullptr };  // 내가 지정한레벨에 로딩될 애니오브젝트 갯수만큼 Info를 들고있는다.

	_uint						m_iNumLoadingTerrain[LEVELID::LEVEL_END] = { 0 };
	LOADING_TERRAIN_INFO*		m_pLoadingTerrainInfo[LEVELID::LEVEL_END] = { nullptr };	 // 내가 지정한레벨에 로딩될 터레인 갯수만큼 Info를 들고있는다.

	LOADING_OBJECT_INFO*		m_pLoadingPlayerInfo[LEVELID::LEVEL_END] = { nullptr };


private:
	HRESULT Ready_Resources_For_LogoLevel();
	HRESULT Ready_Resources_For_GamePlayLevel();


private:
	HRESULT Load_Terrain();

	HRESULT Load_NonAnim_GameObject();

	HRESULT Load_Anim_GameObject();

	HRESULT Load_Player();
	HRESULT	Load_OtherModel();



	HRESULT Create_PrototypeObject();
	HRESULT Create_PrototypeAnimObject();
	HRESULT Create_PrototypePlayer(const _wstring& strPrototypeName, const _wstring& strLayerName);

	HRESULT Create_UI();

	HRESULT	Create_Sky();



	HRESULT Create_Particle();
	HRESULT Create_SwordTrail();				// 소드 트레일
	HRESULT Create_ShurikenTrail();				// 수리켄 트레일


	HRESULT Create_ShurikenEffect();			// 수리켄 이펙트
	HRESULT Create_KatanaBlockEffect();			// 막을때 이펙트
	HRESULT Create_CutAllEffect();				// Cutall 이펙트
	HRESULT Create_NamiEffect();


	HRESULT Create_Blood();
	HRESULT Create_ShockWave();


	HRESULT Create_EliteBlockEffect();
	HRESULT Create_ExplosionEffect();


private:
	void	ReadString(FILE* file, string& str);


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END