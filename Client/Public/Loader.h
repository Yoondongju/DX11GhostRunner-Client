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
	typedef struct
	{
		_wstring	strPrototypeName = L"";
		_wstring	strLayerName = L"";
		_wstring	strModelPrototypeName = L"";

		_float4x4   vWorldMatrix = {};

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


	const LOADING_OBJECT_INFO* Get_Loading_Object_Info(LEVELID eLevelID) { return m_pLoadingObjectInfo[eLevelID]; }
	const _uint				   Get_Loading_Object_Size(LEVELID eLevelID) { return m_iNumLoadingObject[eLevelID]; }

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


						
	_uint						m_iNumLoadingObject[LEVELID::LEVEL_END] = { 0 };
	LOADING_OBJECT_INFO*		m_pLoadingObjectInfo[LEVELID::LEVEL_END] = { nullptr };  // 내가 지정한레벨에 로딩될 오브젝트 갯수만큼 Info를 들고있는다.

	_uint						m_iNumLoadingAnimObject[LEVELID::LEVEL_END] = { 0 };
	LOADING_OBJECT_INFO*		m_pLoadingAnimObjectInfo[LEVELID::LEVEL_END] = { nullptr };  // 내가 지정한레벨에 로딩될 애니오브젝트 갯수만큼 Info를 들고있는다.

	_uint						m_iNumLoadingTerrain[LEVELID::LEVEL_END] = { 0 };
	LOADING_TERRAIN_INFO*		m_pLoadingTerrainInfo[LEVELID::LEVEL_END] = { nullptr };  // 내가 지정한레벨에 로딩될 터레인 갯수만큼 Info를 들고있는다.

	LOADING_OBJECT_INFO*		m_pLoadingPlayerInfo[LEVELID::LEVEL_END] = { nullptr };


private:
	HRESULT Ready_Resources_For_LogoLevel();
	HRESULT Ready_Resources_For_GamePlayLevel();


private:
	HRESULT Load_Terrain();

	HRESULT Load_NonAnim_GameObject();

	HRESULT Load_Anim_GameObject();

	HRESULT Load_Player();



	HRESULT Create_PrototypeObject(_uint iObjectType, const _wstring& strPrototypeName , const _wstring& strLayerName);

	HRESULT Create_PrototypeAnimObject(const _wstring& strPrototypeName, const _wstring& strLayerName);

	HRESULT Create_PrototypePlayer(const _wstring& strPrototypeName, const _wstring& strLayerName);

private:
	void	ReadString(FILE* file, string& str);


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END