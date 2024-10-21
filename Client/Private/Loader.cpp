#include "stdafx.h"
#include "..\Public\Loader.h"
#include "Terrain.h"
#include "FreeCamera.h"


#include "LogoPanel.h"
#include "LogoInnerRing.h"
#include "LogoOutRing.h"
#include "LogoName.h"

#include "GameInstance.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <codecvt>


#include "Sky.h"


#include "Decorative_Object.h"
#include "Static_Object.h"
#include "Dynamic_Object.h"

#include "Player.h"
#include "Body_Player.h"
#include "Weapon_Player.h"
#include "SubShuriken.h"
#include "Wire_Player.h"

#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"
#include "Channel.h"


#include "Spider.h"

#include "Elite.h"
#include "Weapon_Elite.h"

#include "Hel.h"
#include "Weapon_Hel.h"

#include "Jetpack.h"
#include "Weapon_Jetpack.h"
#include "BackPack_Jetpack.h"

#include "Mira.h"

#include "Pistol.h"
#include "Weapon_Pistol.h"

#include "Sniper.h"
#include "Weapon_Sniper.h"


#include "GrapplingPointUI.h"
#include "IconUI.h"
#include "NumberUI.h"
#include "EnemyMarkerUI.h"
#include "EnemyMarkerMC.h"

#include "CrossHairUI.h"
#include "EventNotify.h"
#include "EnemyFindUI.h"
#include "MiniMapPanel.h"
#include "MiniMapMarker.h"
#include "MiniMapPlayer.h"

#include "BossHpPanel.h"
#include "BossHpMain.h"
#include "BossHpEnergy.h"
#include "KillCountUI.h"


#include "SwordTrail.h"
#include "EliteSwordTrail.h"
#include "HelSwordTrail.h"

#include "ShurikenTrail.h"
#include "Particle_ShurikenEffect.h"
#include "Particle_Block.h"
#include "Particle_Nami.h"
#include "Particle_CutAll.h"



#include "Particle_Blood.h"
#include "Particle_EliteBlock.h"
#include "Particle_EliteDashBlock.h"

#include "Particle_Explosion.h"
#include "Particle_ShockWave.h"

#include "Particle_Piece.h"
#include "Particle_Jump.h"
#include "Particle_BigSmoke.h"
#include "Particle_Swirl.h"
#include "Particle_Attack.h"

#include "Particle_Electric.h"
#include "Particle_BigElectric.h"

#include "Particle_Rain.h"

#include "Monster_Bullet.h"

#include "HelMotionTrail.h"
#include "EliteMotionTrail.h"



#pragma warning (disable : 4996)

std::istream& operator>>(std::istream& is, _float4x4& mat) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			is >> mat.m[i][j];
		}
	}
	return is;
}

std::string wstringToString(const std::wstring& wstr) {
	// Note: std::wstring_convert is deprecated in C++17 and removed in C++20.
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}

std::wstring stringToWstring(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(str);
}


CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 1;
		
	return 0;
}


HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	g_CurLevel = m_eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
	 

	return S_OK;
}


HRESULT CLoader::Load_Terrain()
{
	lstrcpy(m_szLoadingText, TEXT("Terrain을 로딩중.."));

	switch (m_eNextLevelID) {
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_GAMEPLAY:
	{
		FILE* fin = fopen("../Bin/Map_Data.bin", "rb");
		if (fin == nullptr) {
			MSG_BOX(TEXT("파일 읽기를 실패했어요.."));
			return E_FAIL;
		}

		_uint iNumTerrain = 0;
		_uint iVerticesX = 0;
		_uint iVerticesZ = 0;
		_uint iTextureIndex = 0;

		fread(&iNumTerrain, sizeof(iNumTerrain), 1, fin);

		m_iNumLoadingTerrain[LEVEL_GAMEPLAY] = iNumTerrain;
		m_pLoadingTerrainInfo[LEVEL_GAMEPLAY] = new LOADING_TERRAIN_INFO[iNumTerrain];

		for (size_t i = 0; i < iNumTerrain; i++) {
			fread(&iVerticesX, sizeof(iVerticesX), 1, fin);
			fread(&iVerticesZ, sizeof(iVerticesZ), 1, fin);
			fread(&iTextureIndex, sizeof(iTextureIndex), 1, fin);

			_float4x4 WorldMatrix;
			fread(&WorldMatrix, sizeof(WorldMatrix), 1, fin);
		

			// For. Prototype_Component_VIBuffer_Terrain
			std::wstring TerrainBuffer_Prototype = L"Prototype_Component_VIBuffer_Terrain_";
			TerrainBuffer_Prototype += (std::to_wstring(iVerticesX) + L"_" + std::to_wstring(iVerticesZ));

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TerrainBuffer_Prototype.c_str(),
				CVIBuffer_Terrain::Create(m_pDevice, m_pContext, iVerticesX, iVerticesZ, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
			{
				fclose(fin);
				return E_FAIL;
			}

			// For. Prototype_GameObject_Terrain
			std::wstring Terrain_Prototype = L"Prototype_GameObject_Terrain_";
			Terrain_Prototype += (std::to_wstring(iVerticesX) + L"_" + std::to_wstring(iVerticesZ));

			if (FAILED(m_pGameInstance->Add_Prototype(Terrain_Prototype.c_str(),
				CTerrain::Create(m_pDevice, m_pContext, iVerticesX, iVerticesZ, iTextureIndex))))
			{
				fclose(fin);
				return E_FAIL;
			}

			m_pLoadingTerrainInfo[LEVEL_GAMEPLAY][i].vWorldMatrix = WorldMatrix;
			m_pLoadingTerrainInfo[LEVEL_GAMEPLAY][i].strPrototypeName = Terrain_Prototype;
			m_pLoadingTerrainInfo[LEVEL_GAMEPLAY][i].strModelPrototypeName = TerrainBuffer_Prototype;
		}

		fclose(fin);


		// For. Prototype_Component_Texture_Terrain
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/TileTest.png"), 1))))
		{
			return E_FAIL;
		}
	}
	break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CLoader::Load_Anim_GameObject()
{
	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("애니메이션 모델을 로딩중..."));

	FILE* file = nullptr;

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_GAMEPLAY:
	{
		
	}
	break;

	case Client::LEVEL_STAGE1:
	{
		fopen_s(&file, "../Bin/Stage1/Anim_Model_Data.bin", "rb");
	}
		break;
	case Client::LEVEL_STAGE1_BOSS:
	{
		fopen_s(&file, "../Bin/Stage1_BossMap/Anim_Model_Data.bin", "rb");
	}
		break;
	case Client::LEVEL_STAGE2_BOSS:
	{
		fopen_s(&file, "../Bin/Stage2_BossMap/Anim_Model_Data.bin", "rb");
	}
		break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}


	if (!file)    // 파일 열었다면
	{
		MSG_BOX(TEXT("파일 읽기를 실패했어요.."));
		return E_FAIL;
	}

	_uint iObjectCount = {};
	_uint iObjectType = (_uint)MODEL_CHECK_LIST::MODEL_CHECK_TYPE_END;

	string strMyPrototypeName = "";
	wstring wstrMyPrototypeName = L"";

	string strMyLayerName = "";
	wstring wstrMyLayerName = L"";

	string strMyModelPrototypeName = "";
	wstring wstrMyModelPrototypeName = L"";

	_uint iModelType = 0;

	string strMyModelFilePath = "";

	string strMaterialTexturePath = "";
	wstring wstrMaterialTexturePath[15][AI_TEXTURE_TYPE_MAX] = { L"" };

	_uint iMeshesCount = 0;
	_uint iMaterialsCount = 0;

	_float4x4 PreTransformMatrix = {};

	fread(&iObjectCount, sizeof(iObjectCount), 1, file);

	m_iNumLoadingAnimObject[m_eNextLevelID] = iObjectCount;
	m_pLoadingAnimObjectInfo[m_eNextLevelID] = new LOADING_OBJECT_INFO[iObjectCount];

	for (size_t i = 0; i < iObjectCount; i++)
	{
		// 객체원형 //
		fread(&iObjectType, sizeof(iObjectType), 1, file);

		// 프로토타입 이름 읽기
		ReadString(file, strMyPrototypeName);
		wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

		// 레이어 이름 읽기
		ReadString(file, strMyLayerName);
		wstrMyLayerName = stringToWstring(strMyLayerName);



		// 모델 //
		fread(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, file);

		ReadString(file, strMyModelPrototypeName);
		wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

		fread(&iModelType, sizeof(iModelType), 1, file);

		ReadString(file, strMyModelFilePath);

		fread(&iMeshesCount, sizeof(iMeshesCount), 1, file);
		fread(&iMaterialsCount, sizeof(iMaterialsCount), 1, file);

		for (size_t i = 0; i < iMaterialsCount; i++)
		{
			for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			{
				ReadString(file, strMaterialTexturePath);
				wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath);
			}
		}

		CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[iMeshesCount];

		CBone::INIT_BONE_DESC Desc = {};
		CAnimation::ANIMATION_DESC AnimationDesc = {};
		vector<vector<_uint>> vecKeyFrameIndices;

		for (size_t i = 0; i < iMeshesCount; i++)
		{
			fread(&pDesc[i].iMaterialIndex, sizeof(pDesc[i].iMaterialIndex), 1, file);
			fread(&pDesc[i].iNumVertices, sizeof(pDesc[i].iNumVertices), 1, file);
			fread(&pDesc[i].iVertexStride, sizeof(pDesc[i].iVertexStride), 1, file);

			pDesc[i].pAnimVertices = new VTXANIMMESH[pDesc[i].iNumVertices];
			fread(pDesc[i].pAnimVertices, sizeof(VTXANIMMESH) * pDesc[i].iNumVertices, 1, file);

			fread(&pDesc[i].iNumIndices, sizeof(pDesc[i].iNumIndices), 1, file);
			fread(&pDesc[i].iIndexStride, sizeof(pDesc[i].iIndexStride), 1, file);

			pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
			fread(pDesc[i].pIndices, sizeof(_uint) * pDesc[i].iNumIndices, 1, file);

			fread(&pDesc[i].eIndexFormat, sizeof(pDesc[i].eIndexFormat), 1, file);
			fread(&pDesc[i].eTopology, sizeof(pDesc[i].eTopology), 1, file);

			fread(&pDesc[i].vMinPos, sizeof(pDesc[i].vMinPos), 1, file);
			fread(&pDesc[i].vMaxPos, sizeof(pDesc[i].vMaxPos), 1, file);

			_uint iNumModelBones = {};
			fread(&iNumModelBones, sizeof(iNumModelBones), 1, file);

			Desc.vecChildBones.resize(iNumModelBones);

			for (size_t i = 0; i < iNumModelBones; i++)
			{
				_int iParentBoneIndex = {};
				_uint iNumChildBone = {};

				_char* szName = nullptr;
				size_t nameLength = {};

				_float4x4 TransformationMatrix = {};
				_float4x4 CombinedTransformationMatrix = {};

				fread(&iParentBoneIndex, sizeof(iParentBoneIndex), 1, file);
				fread(&iNumChildBone, sizeof(iNumChildBone), 1, file);

				fread(&nameLength, sizeof(nameLength), 1, file);
				szName = new char[nameLength + 1];
				fread(szName, nameLength, 1, file);
				szName[nameLength] = '\0';

				fread(&TransformationMatrix, sizeof(TransformationMatrix), 1, file);
				fread(&CombinedTransformationMatrix, sizeof(CombinedTransformationMatrix), 1, file);

				Desc.vecChildBones[i].iParentBoneIndex = iParentBoneIndex;
				Desc.vecChildBones[i].iNumChildBone = iNumChildBone;

				strcpy(Desc.vecChildBones[i].szName, szName);

				Desc.vecChildBones[i].TransformationMatrix = TransformationMatrix;
				Desc.vecChildBones[i].CombinedTransformaionMatrix = CombinedTransformationMatrix;

				delete[] szName;
			}

			_char* szName = nullptr;
			size_t nameLength = {};

			fread(&nameLength, sizeof(nameLength), 1, file);
			szName = new char[nameLength + 1];
			fread(szName, nameLength, 1, file);
			szName[nameLength] = '\0';

			_uint iNum_AffectedBones = {};
			fread(&iNum_AffectedBones, sizeof(iNum_AffectedBones), 1, file);

			vector<_uint> vecIndices;
			vecIndices.resize(iNum_AffectedBones);

			vector<_float4x4> vecOffsetMatrix;
			vecOffsetMatrix.resize(iNum_AffectedBones);

			for (size_t i = 0; i < iNum_AffectedBones; i++)
			{
				fread(&vecIndices[i], sizeof(vecIndices[i]), 1, file);
				fread(&vecOffsetMatrix[i], sizeof(vecOffsetMatrix[i]), 1, file);
			}

			strcpy(pDesc[i].pName, szName);
			pDesc[i].vecIndices.resize(iNum_AffectedBones);
			pDesc[i].vecOffsetMatrix.resize(iNum_AffectedBones);

			pDesc[i].iNumAffectBones = iNum_AffectedBones;
			pDesc[i].vecIndices = vecIndices;
			pDesc[i].vecOffsetMatrix = vecOffsetMatrix;

			delete[] szName;

			_uint iNumAnimation = {};
			fread(&iNumAnimation, sizeof(iNumAnimation), 1, file);

			_uint iNumChannel = {};

			AnimationDesc.vecAnimationDesc.resize(iNumAnimation);

			for (size_t i = 0; i < iNumAnimation; i++)
			{
				_char* szName = nullptr;
				size_t nameLength = {};

				fread(&nameLength, sizeof(nameLength), 1, file);
				szName = new char[nameLength + 1];
				fread(szName, nameLength, 1, file);
				szName[nameLength] = '\0';

				_double Duration = {};
				_double SpeedPerSec = {};

				fread(&Duration, sizeof(Duration), 1, file);
				fread(&SpeedPerSec, sizeof(SpeedPerSec), 1, file);
				fread(&iNumChannel, sizeof(iNumChannel), 1, file);

				AnimationDesc.vecAnimationDesc[i].vecChannelDesc.resize(iNumChannel);

				for (size_t j = 0; j < iNumChannel; j++)
				{
					_char* szName = nullptr;
					size_t nameLength = {};

					_uint iBoneIndex = {};

					_uint iNumKeyFrame = {};

					fread(&nameLength, sizeof(nameLength), 1, file);
					szName = new char[nameLength + 1];
					fread(szName, nameLength, 1, file);
					szName[nameLength] = '\0';

					fread(&iBoneIndex, sizeof(iBoneIndex), 1, file);
					fread(&iNumKeyFrame, sizeof(iNumKeyFrame), 1, file);

					strcpy(AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].pName, szName);
					AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].iBoneIndex = iBoneIndex;
					AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].iNumKeyFrames = iNumKeyFrame;

					delete[] szName;

					AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames.resize(iNumKeyFrame);

					for (size_t k = 0; k < iNumKeyFrame; k++)
					{
						_float3 vScale = {};
						_float4 vRotation = {};
						_float3 vTranslation = {};
						_double vTarckPosition = {};

						fread(&vScale, sizeof(vScale), 1, file);
						fread(&vRotation, sizeof(vRotation), 1, file);
						fread(&vTranslation, sizeof(vTranslation), 1, file);
						fread(&vTarckPosition, sizeof(vTarckPosition), 1, file);

						AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames[k].vScale = vScale;
						AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames[k].vRotation = vRotation;
						AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames[k].vTranslation = vTranslation;
						AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames[k].TrackPosition = vTarckPosition;
					}
				}

				strcpy(AnimationDesc.vecAnimationDesc[i].szName, szName);
				AnimationDesc.vecAnimationDesc[i].Duration = Duration;
				AnimationDesc.vecAnimationDesc[i].SpeedPerSec = SpeedPerSec;
				AnimationDesc.vecAnimationDesc[i].iNumChannels = iNumChannel;
				delete[] szName;
			}
		}

		if (true == m_pGameInstance->IsFind_Model(m_eNextLevelID, wstrMyModelPrototypeName))
		{
			int a = 0;
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, wstrMyModelPrototypeName,
				CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType,
					strMyModelFilePath.c_str(),
					*wstrMaterialTexturePath,
					iMeshesCount,
					iMaterialsCount,
					XMLoadFloat4x4(&PreTransformMatrix),
					pDesc,
					reinterpret_cast<void*>(&Desc),
					reinterpret_cast<void*>(&AnimationDesc)))))
			{
				_wstring msg = to_wstring(i) + L"번째" + L"모델이 로드가 안댐";
				MSG_BOX(msg.c_str());
				return E_FAIL;
			}
		}

		// 월드 행렬 읽기
		_float4x4 WorldMatrix;
		fread(&WorldMatrix, sizeof(WorldMatrix), 1, file);


		if (MODEL_CHECK_LIST::ELITE == iObjectType ||
			MODEL_CHECK_LIST::JETPACK == iObjectType ||
			MODEL_CHECK_LIST::PISTOL == iObjectType ||
			MODEL_CHECK_LIST::SNIPER == iObjectType ||
			MODEL_CHECK_LIST::HEL == iObjectType)
		{
			fread(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType), 1, file);

			// 프로토타입 이름 읽기
			ReadString(file, strMyPrototypeName);
			wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

			// 레이어 이름 읽기
			ReadString(file, strMyLayerName);
			wstrMyLayerName = stringToWstring(strMyLayerName);

			_uint iPartSize = {};
			fread(reinterpret_cast<char*>(&iPartSize), sizeof(iPartSize), 1, file);

			for (_uint k = 0; k < iPartSize; k++)
			{
				_uint iMeshesCount = {};
				_uint iMaterialsCount = {};

				// 모델 //
				fread(reinterpret_cast<char*>(&PreTransformMatrix), sizeof(PreTransformMatrix), 1, file);

				ReadString(file, strMyModelPrototypeName);
				wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

				fread(reinterpret_cast<char*>(&iModelType), sizeof(iModelType), 1, file);

				ReadString(file, strMyModelFilePath);  // 모델 fbx경로 _char* 를 요구하기에 string 으로 해야함

				fread(reinterpret_cast<char*>(&iMeshesCount), sizeof(iMeshesCount), 1, file);
				fread(reinterpret_cast<char*>(&iMaterialsCount), sizeof(iMaterialsCount), 1, file);

				for (size_t i = 0; i < iMaterialsCount; i++)
				{
					for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
					{
						ReadString(file, strMaterialTexturePath);
						wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // 머테리얼 경로
					}
				}

				CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[iMeshesCount];

				for (size_t i = 0; i < iMeshesCount; i++)
				{
					fread(reinterpret_cast<char*>(&pDesc[i].iMaterialIndex), sizeof(pDesc[i].iMaterialIndex), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].iNumVertices), sizeof(pDesc[i].iNumVertices), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].iVertexStride), sizeof(pDesc[i].iVertexStride), 1, file);

					pDesc[i].pVertices = new VTXMESH[pDesc[i].iNumVertices];
					fread(reinterpret_cast<char*>(pDesc[i].pVertices), sizeof(VTXMESH) * pDesc[i].iNumVertices, 1, file);

					fread(reinterpret_cast<char*>(&pDesc[i].iNumIndices), sizeof(pDesc[i].iNumIndices), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].iIndexStride), sizeof(pDesc[i].iIndexStride), 1, file);

					pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
					fread(reinterpret_cast<char*>(pDesc[i].pIndices), sizeof(_uint) * pDesc[i].iNumIndices, 1, file);

					fread(reinterpret_cast<char*>(&pDesc[i].eIndexFormat), sizeof(pDesc[i].eIndexFormat), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].eTopology), sizeof(pDesc[i].eTopology), 1, file);

					fread(reinterpret_cast<char*>(&pDesc[i].vMinPos), sizeof(pDesc[i].vMinPos), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].vMaxPos), sizeof(pDesc[i].vMaxPos), 1, file);
				}

				if (true == m_pGameInstance->IsFind_Model(m_eNextLevelID, wstrMyModelPrototypeName))  // 내가 불러오고자 하는 모델이 이미 불러왔어?
				{
					// 모델을 찾았을 때의 처리
				}
				else
				{
					if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, wstrMyModelPrototypeName, // 모델 프로토타입 이름
						CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType,  // 모델타입
							strMyModelFilePath.c_str(),  // 모델 fbx 경로
							*wstrMaterialTexturePath,  // 머테리얼 텍스쳐 경로
							iMeshesCount,  // 메쉬 갯수
							iMaterialsCount,  // 머테리얼 갯수
							XMLoadFloat4x4(&PreTransformMatrix),  // 더미
							pDesc))))  // 메쉬를 이루기 위한 정보
					{
						MSG_BOX(TEXT("AnimModel의 Part가 로드가 안댐요"));
						return E_FAIL;
					}
				}


				// 머라고해야하지
				//m_pLoadingAnimObjectInfo[LEVEL_GAMEPLAY][i].PartsInfo[k].strLayerName = wstrMyLayerName;
				//m_pLoadingAnimObjectInfo[LEVEL_GAMEPLAY][i].PartsInfo[k].strPrototypeName = wstrMyPrototypeName;
				//m_pLoadingAnimObjectInfo[LEVEL_GAMEPLAY][i].PartsInfo[k].strModelPrototypeName = wstrMyModelPrototypeName;
				//
				//XMStoreFloat4x4(&m_pLoadingAnimObjectInfo[LEVEL_GAMEPLAY][i].PartsInfo[k].vWorldMatrix, XMMatrixIdentity());

				for (size_t i = 0; i < iMeshesCount; i++)
				{
					Safe_Delete_Array(pDesc[i].pVertices);
					Safe_Delete_Array(pDesc[i].pIndices);
				}
				Safe_Delete_Array(pDesc);

			}


			// 월드 행렬 읽기
			_float4x4 WorldMatrix;
			fread(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix), 1, file);


		}



		m_pLoadingAnimObjectInfo[m_eNextLevelID][i].strLayerName = wstrMyLayerName;
		m_pLoadingAnimObjectInfo[m_eNextLevelID][i].strPrototypeName = wstrMyPrototypeName;
		m_pLoadingAnimObjectInfo[m_eNextLevelID][i].strModelPrototypeName = wstrMyModelPrototypeName;
		XMStoreFloat4x4(&m_pLoadingAnimObjectInfo[m_eNextLevelID][i].vWorldMatrix, XMLoadFloat4x4(&WorldMatrix));

		m_pLoadingAnimObjectInfo[m_eNextLevelID][i].eModelType = (MODEL_CHECK_LIST)iObjectType;


		for (size_t i = 0; i < iMeshesCount; i++)
		{
			delete[] pDesc[i].pAnimVertices;
			delete[] pDesc[i].pIndices;
		}
		delete[] pDesc;
	}

	fclose(file);

	return S_OK;
}

HRESULT CLoader::Load_NonAnim_GameObject()
{
	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("정적 오브젝트를 로딩중..."));

	FILE* file = nullptr;

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_GAMEPLAY:
	{
		
	}
	break;

	case Client::LEVEL_STAGE1:
	{
		fopen_s(&file, "../Bin/Stage1/NonAnim_Model_Data.bin", "rb");
	}
	break;
	case Client::LEVEL_STAGE1_BOSS:
	{
		fopen_s(&file, "../Bin/Stage1_BossMap/NonAnim_Model_Data.bin", "rb");
	}
	break;
	case Client::LEVEL_STAGE2_BOSS:
	{
		fopen_s(&file, "../Bin/Stage2_BossMap/NonAnim_Model_Data.bin", "rb");
	}
	break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}

	if (!file) // 파일 열기를 실패한 경우
	{
		MSG_BOX(TEXT("파일 읽기를 실패했어요.."));
		return E_FAIL;
	}

	_uint					iObjectCount = {};

	_uint					iObjectType = (_uint)MODEL_CHECK_LIST::MODEL_CHECK_TYPE_END;

	string					strMyPrototypeName = "";
	_wstring				wstrMyPrototypeName = L"";

	string					strMyLayerName = "";
	_wstring				wstrMyLayerName = L"";

	string					strMyModelPrototypeName = "";
	_wstring				wstrMyModelPrototypeName = L"";

	_uint					iModelType = 0;

	string					strMyModelFilePath = "";

	string					strMaterialTexturePath = "";
	_wstring				wstrMaterialTexturePath[15][AI_TEXTURE_TYPE_MAX] = { L"" };


	_uint					iMeshesCount = 0;
	_uint					iMaterialsCount = 0;

	_float4x4				PreTransformMatrix = {};



	fread(&iObjectCount, sizeof(iObjectCount), 1, file);
	m_iNumLoadingDecorativeObject[m_eNextLevelID] = iObjectCount;
	m_pLoadingDecorativeObjectInfo[m_eNextLevelID] = new LOADING_OBJECT_INFO[iObjectCount];
	for (size_t i = 0; i < iObjectCount; i++)
	{
		// 객체원형 //
		fread(&iObjectType, sizeof(iObjectType), 1, file);

		// 프로토타입 이름 읽기
		ReadString(file, strMyPrototypeName);
		wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

		// 레이어 이름 읽기
		ReadString(file, strMyLayerName);
		wstrMyLayerName = stringToWstring(strMyLayerName);


		// 모델 //
		fread(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, file);

		ReadString(file, strMyModelPrototypeName);
		wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

		fread(&iModelType, sizeof(iModelType), 1, file);
		ReadString(file, strMyModelFilePath); // 모델 fbx경로 _char* 를 요구하기에 string 으로 해야함

		fread(&iMeshesCount, sizeof(iMeshesCount), 1, file);
		fread(&iMaterialsCount, sizeof(iMaterialsCount), 1, file);

		for (size_t i = 0; i < iMaterialsCount; i++)
		{
			for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			{
				ReadString(file, strMaterialTexturePath);
				wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // 머테리얼 경로
			}
		}

		CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[iMeshesCount];
		//ZeroMemory(pDesc, sizeof(CMesh::MESH_DESC) * iMeshesCount);

		for (size_t i = 0; i < iMeshesCount; i++)
		{
			fread(&pDesc[i].iMaterialIndex, sizeof(pDesc[i].iMaterialIndex), 1, file);
			fread(&pDesc[i].iNumVertices, sizeof(pDesc[i].iNumVertices), 1, file);
			fread(&pDesc[i].iVertexStride, sizeof(pDesc[i].iVertexStride), 1, file);

			pDesc[i].pVertices = new VTXMESH[pDesc[i].iNumVertices];
			fread(pDesc[i].pVertices, sizeof(VTXMESH) * pDesc[i].iNumVertices, 1, file);

			fread(&pDesc[i].iNumIndices, sizeof(pDesc[i].iNumIndices), 1, file);
			fread(&pDesc[i].iIndexStride, sizeof(pDesc[i].iIndexStride), 1, file);

			pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
			fread(pDesc[i].pIndices, sizeof(_uint) * pDesc[i].iNumIndices, 1, file);

			fread(&pDesc[i].eIndexFormat, sizeof(pDesc[i].eIndexFormat), 1, file);
			fread(&pDesc[i].eTopology, sizeof(pDesc[i].eTopology), 1, file);

			fread(&pDesc[i].vMinPos, sizeof(pDesc[i].vMinPos), 1, file);
			fread(&pDesc[i].vMaxPos, sizeof(pDesc[i].vMaxPos), 1, file);
		}

		if (true == m_pGameInstance->IsFind_Model(m_eNextLevelID, wstrMyModelPrototypeName)) // 내가 불러오고자 하는 모델이 이미 불러왔어?
		{
			// 모델을 찾았을 때의 처리
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, wstrMyModelPrototypeName, // 모델 프로토타입 이름
				CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType, // 모델타입
					strMyModelFilePath.c_str(), // 모델 fbx 경로
					*wstrMaterialTexturePath, // 머테리얼 텍스쳐 경로
					iMeshesCount, // 메쉬 갯수
					iMaterialsCount, // 머테리얼 갯수
					XMLoadFloat4x4(&PreTransformMatrix), // 더미
					pDesc)))) // 메쉬를 이루기 위한 정보
			{
				_wstring msg = to_wstring(i) + L"번째" + L"모델이 로드가 안댐";
				MSG_BOX(msg.c_str());
				return E_FAIL;
			}
		}

		// 월드 행렬 읽기
		_float4x4 WorldMatrix;
		fread(&WorldMatrix, sizeof(WorldMatrix), 1, file);

		m_pLoadingDecorativeObjectInfo[m_eNextLevelID][i].strLayerName = wstrMyLayerName;
		m_pLoadingDecorativeObjectInfo[m_eNextLevelID][i].strPrototypeName = wstrMyPrototypeName;
		m_pLoadingDecorativeObjectInfo[m_eNextLevelID][i].strModelPrototypeName = wstrMyModelPrototypeName;
		m_pLoadingDecorativeObjectInfo[m_eNextLevelID][i].vWorldMatrix = WorldMatrix;

		m_pLoadingDecorativeObjectInfo[m_eNextLevelID][i].eModelType = (MODEL_CHECK_LIST)iObjectType;



		for (size_t i = 0; i < iMeshesCount; i++)
		{
			Safe_Delete_Array(pDesc[i].pVertices);
			Safe_Delete_Array(pDesc[i].pIndices);
		}
		Safe_Delete_Array(pDesc);
	}



	fread(&iObjectCount, sizeof(iObjectCount), 1, file);
	m_iNumLoadingStaticObject[m_eNextLevelID] = iObjectCount;
	m_pLoadingStaticObjectInfo[m_eNextLevelID] = new LOADING_OBJECT_INFO[iObjectCount];
	for (size_t i = 0; i < iObjectCount; i++)
	{
		// 객체원형 //
		fread(&iObjectType, sizeof(iObjectType), 1, file);

		// 프로토타입 이름 읽기
		ReadString(file, strMyPrototypeName);
		wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

		// 레이어 이름 읽기
		ReadString(file, strMyLayerName);
		wstrMyLayerName = stringToWstring(strMyLayerName);


		// 모델 //
		fread(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, file);

		ReadString(file, strMyModelPrototypeName);
		wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

		fread(&iModelType, sizeof(iModelType), 1, file);
		ReadString(file, strMyModelFilePath); // 모델 fbx경로 _char* 를 요구하기에 string 으로 해야함

		fread(&iMeshesCount, sizeof(iMeshesCount), 1, file);
		fread(&iMaterialsCount, sizeof(iMaterialsCount), 1, file);

		for (size_t i = 0; i < iMaterialsCount; i++)
		{
			for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			{
				ReadString(file, strMaterialTexturePath);
				wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // 머테리얼 경로
			}
		}

		CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[iMeshesCount];
		//ZeroMemory(pDesc, sizeof(CMesh::MESH_DESC) * iMeshesCount);

		for (size_t i = 0; i < iMeshesCount; i++)
		{
			fread(&pDesc[i].iMaterialIndex, sizeof(pDesc[i].iMaterialIndex), 1, file);
			fread(&pDesc[i].iNumVertices, sizeof(pDesc[i].iNumVertices), 1, file);
			fread(&pDesc[i].iVertexStride, sizeof(pDesc[i].iVertexStride), 1, file);

			pDesc[i].pVertices = new VTXMESH[pDesc[i].iNumVertices];
			fread(pDesc[i].pVertices, sizeof(VTXMESH) * pDesc[i].iNumVertices, 1, file);

			fread(&pDesc[i].iNumIndices, sizeof(pDesc[i].iNumIndices), 1, file);
			fread(&pDesc[i].iIndexStride, sizeof(pDesc[i].iIndexStride), 1, file);

			pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
			fread(pDesc[i].pIndices, sizeof(_uint) * pDesc[i].iNumIndices, 1, file);

			fread(&pDesc[i].eIndexFormat, sizeof(pDesc[i].eIndexFormat), 1, file);
			fread(&pDesc[i].eTopology, sizeof(pDesc[i].eTopology), 1, file);

			fread(&pDesc[i].vMinPos, sizeof(pDesc[i].vMinPos), 1, file);
			fread(&pDesc[i].vMaxPos, sizeof(pDesc[i].vMaxPos), 1, file);
		}

		if (true == m_pGameInstance->IsFind_Model(m_eNextLevelID, wstrMyModelPrototypeName)) // 내가 불러오고자 하는 모델이 이미 불러왔어?
		{
			// 모델을 찾았을 때의 처리
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, wstrMyModelPrototypeName, // 모델 프로토타입 이름
				CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType, // 모델타입
					strMyModelFilePath.c_str(), // 모델 fbx 경로
					*wstrMaterialTexturePath,	// 머테리얼 텍스쳐 경로
					iMeshesCount,				// 메쉬 갯수
					iMaterialsCount,			// 머테리얼 갯수
					XMLoadFloat4x4(&PreTransformMatrix), // 더미
					pDesc))))					// 메쉬를 이루기 위한 정보
			{
				_wstring msg = to_wstring(i) + L"번째" + L"모델이 로드가 안댐";
				MSG_BOX(msg.c_str());
				return E_FAIL;
			}
		}

		// 월드 행렬 읽기
		_float4x4 WorldMatrix;
		fread(&WorldMatrix, sizeof(WorldMatrix), 1, file);

		m_pLoadingStaticObjectInfo[m_eNextLevelID][i].strLayerName = wstrMyLayerName;
		m_pLoadingStaticObjectInfo[m_eNextLevelID][i].strPrototypeName = wstrMyPrototypeName;
		m_pLoadingStaticObjectInfo[m_eNextLevelID][i].strModelPrototypeName = wstrMyModelPrototypeName;
		m_pLoadingStaticObjectInfo[m_eNextLevelID][i].vWorldMatrix = WorldMatrix;

		m_pLoadingStaticObjectInfo[m_eNextLevelID][i].eModelType = (MODEL_CHECK_LIST)iObjectType;



		for (size_t i = 0; i < iMeshesCount; i++)
		{
			Safe_Delete_Array(pDesc[i].pVertices);
			Safe_Delete_Array(pDesc[i].pIndices);
		}
		Safe_Delete_Array(pDesc);
	}



	fread(&iObjectCount, sizeof(iObjectCount), 1, file);
	m_iNumLoadingDynamicObject[m_eNextLevelID] = iObjectCount;
	m_pLoadingDynamicObjectInfo[m_eNextLevelID] = new LOADING_OBJECT_INFO[iObjectCount];
	for (size_t i = 0; i < iObjectCount; i++)
	{
		// 객체원형 //
		fread(&iObjectType, sizeof(iObjectType), 1, file);

		// 프로토타입 이름 읽기
		ReadString(file, strMyPrototypeName);
		wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

		// 레이어 이름 읽기
		ReadString(file, strMyLayerName);
		wstrMyLayerName = stringToWstring(strMyLayerName);


		// 모델 //
		fread(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, file);

		ReadString(file, strMyModelPrototypeName);
		wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

		fread(&iModelType, sizeof(iModelType), 1, file);
		ReadString(file, strMyModelFilePath); // 모델 fbx경로 _char* 를 요구하기에 string 으로 해야함

		fread(&iMeshesCount, sizeof(iMeshesCount), 1, file);
		fread(&iMaterialsCount, sizeof(iMaterialsCount), 1, file);

		for (size_t i = 0; i < iMaterialsCount; i++)
		{
			for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			{
				ReadString(file, strMaterialTexturePath);
				wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // 머테리얼 경로
			}
		}

		CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[iMeshesCount];
		//ZeroMemory(pDesc, sizeof(CMesh::MESH_DESC) * iMeshesCount);

		for (size_t i = 0; i < iMeshesCount; i++)
		{
			fread(&pDesc[i].iMaterialIndex, sizeof(pDesc[i].iMaterialIndex), 1, file);
			fread(&pDesc[i].iNumVertices, sizeof(pDesc[i].iNumVertices), 1, file);
			fread(&pDesc[i].iVertexStride, sizeof(pDesc[i].iVertexStride), 1, file);

			pDesc[i].pVertices = new VTXMESH[pDesc[i].iNumVertices];
			fread(pDesc[i].pVertices, sizeof(VTXMESH) * pDesc[i].iNumVertices, 1, file);

			fread(&pDesc[i].iNumIndices, sizeof(pDesc[i].iNumIndices), 1, file);
			fread(&pDesc[i].iIndexStride, sizeof(pDesc[i].iIndexStride), 1, file);

			pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
			fread(pDesc[i].pIndices, sizeof(_uint) * pDesc[i].iNumIndices, 1, file);

			fread(&pDesc[i].eIndexFormat, sizeof(pDesc[i].eIndexFormat), 1, file);
			fread(&pDesc[i].eTopology, sizeof(pDesc[i].eTopology), 1, file);

			fread(&pDesc[i].vMinPos, sizeof(pDesc[i].vMinPos), 1, file);
			fread(&pDesc[i].vMaxPos, sizeof(pDesc[i].vMaxPos), 1, file);
		}

		if (true == m_pGameInstance->IsFind_Model(m_eNextLevelID, wstrMyModelPrototypeName)) // 내가 불러오고자 하는 모델이 이미 불러왔어?
		{
			// 모델을 찾았을 때의 처리
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, wstrMyModelPrototypeName, // 모델 프로토타입 이름
				CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType, // 모델타입
					strMyModelFilePath.c_str(), // 모델 fbx 경로
					*wstrMaterialTexturePath, // 머테리얼 텍스쳐 경로
					iMeshesCount, // 메쉬 갯수
					iMaterialsCount, // 머테리얼 갯수
					XMLoadFloat4x4(&PreTransformMatrix), // 더미
					pDesc)))) // 메쉬를 이루기 위한 정보
			{
				_wstring msg = to_wstring(i) + L"번째" + L"모델이 로드가 안댐";
				MSG_BOX(msg.c_str());
				return E_FAIL;
			}
		}

		// 월드 행렬 읽기
		_float4x4 WorldMatrix;
		fread(&WorldMatrix, sizeof(WorldMatrix), 1, file);

		m_pLoadingDynamicObjectInfo[m_eNextLevelID][i].strLayerName = wstrMyLayerName;
		m_pLoadingDynamicObjectInfo[m_eNextLevelID][i].strPrototypeName = wstrMyPrototypeName;
		m_pLoadingDynamicObjectInfo[m_eNextLevelID][i].strModelPrototypeName = wstrMyModelPrototypeName;
		m_pLoadingDynamicObjectInfo[m_eNextLevelID][i].vWorldMatrix = WorldMatrix;

		m_pLoadingDynamicObjectInfo[m_eNextLevelID][i].eModelType = (MODEL_CHECK_LIST)iObjectType;



		for (size_t i = 0; i < iMeshesCount; i++)
		{
			Safe_Delete_Array(pDesc[i].pVertices);
			Safe_Delete_Array(pDesc[i].pIndices);
		}
		Safe_Delete_Array(pDesc);
	}

	fclose(file);

	return S_OK;
}

HRESULT CLoader::Load_OtherModel()
{
	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("인스턴싱 모델을 로딩중..."));

	FILE* fin = nullptr;

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_GAMEPLAY:
	{
		
	}
	break;
	case Client::LEVEL_STAGE1:
	{
		fopen_s(&fin, "../Bin/Stage1/Ohter_Model_Data.bin", "rb");		// 총알 ,불렛 .. 
	}
	break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}


	if (!fin)    // 파일 열기에 실패했다면
	{
		MSG_BOX(TEXT("파일 읽기를 실패했어요.."));
		return E_FAIL;
	}

	CModel* pModel = nullptr;

	_uint iModelSize = 0;
	fread(&iModelSize, sizeof(iModelSize), 1, fin);

	for (_uint i = 1; i < iModelSize + 1; i++)
	{
		// PreTransformMatrix 읽기
		_float4x4 PreTransformMatrix;
		fread(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fin);

		// Prototype Name 크기 읽기
		_uint modelPrototypeNameSize = 0;
		fread(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fin);

		// Prototype Name 읽기
		std::string modelPrototypeName(modelPrototypeNameSize, '\0');
		fread(&modelPrototypeName[0], modelPrototypeNameSize, 1, fin);

		// Model File Path 크기 읽기
		_uint modelFilePathSize = 0;
		fread(&modelFilePathSize, sizeof(modelFilePathSize), 1, fin);

		// Model File Path 읽기
		std::string modelFilePath(modelFilePathSize, '\0');
		fread(&modelFilePath[0], modelFilePathSize, 1, fin);

		// Mesh 및 Material 정보 읽기
		_uint meshCount = 0;
		_uint materialsCount = 0;
		fread(&meshCount, sizeof(meshCount), 1, fin);
		fread(&materialsCount, sizeof(materialsCount), 1, fin);

		string					strMaterialTexturePath = "";
		_wstring				wstrMaterialTexturePath[200][AI_TEXTURE_TYPE_MAX] = { L"" };			// << 이거 최종맵데이터는 메테리얼갯수많아

		// Material Texture Path 읽기
		for (size_t i = 0; i < materialsCount; i++)
		{
			for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			{
				ReadString(fin, strMaterialTexturePath);
				wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // 머테리얼 경로
			}
		}


		_wstring ModelPrototypeName = stringToWstring(modelPrototypeName);

		CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[meshCount]; 
		CVIBuffer_Mesh_Instance::MESHINSTANCE_DESC			Desc{};
		if (i == 1)		// 피
		{
			pDesc->isInstanceObject = true;
			pDesc->InstanceBufferPrototypeTag = ModelPrototypeName + L"Instance";

			
			Desc.iNumInstance = 60;
			Desc.vCenter = _float3(0.f, 0.f, 0.f);
			Desc.vRange = _float3(1.f, 2.5f, 1.f);
			Desc.vSize = _float2(0.5f, 2.f);
			Desc.vPivot = _float3(0.f, 0.f, 0.f);
			Desc.vSpeed = _float2(10.f, 15.f);
			Desc.vLifeTime = _float2(0.5f, 1.5f);
			Desc.isLoop = false;

			pDesc->MeshInstanceDesc = &Desc;
		}
		else if (i == 2)	// 유리조각
		{
			pDesc->isInstanceObject = true;
			pDesc->InstanceBufferPrototypeTag = ModelPrototypeName + L"Instance";

			
			Desc.iNumInstance = 70;
			Desc.vCenter = _float3(0.f, 0.f, 0.f);
			Desc.vRange = _float3(1.f, 1.f, 1.f);
			Desc.vSize = _float2(3.f, 6.f);
			Desc.vPivot = _float3(0.f, 0.f, 0.f);
			Desc.vSpeed = _float2(5.f, 10.f);
			Desc.vLifeTime = _float2(0.5f, 1.5f);
			Desc.isLoop = false;
			

			pDesc->MeshInstanceDesc = &Desc;
		}
		else if (i == 5)		// Electric
		{
			pDesc->isInstanceObject = true;
			pDesc->InstanceBufferPrototypeTag = ModelPrototypeName + L"Instance";

			
			Desc.iNumInstance = 70;
			Desc.vCenter = _float3(0.f, 0.f, 0.f);
			Desc.vRange = _float3(1.f, 1.f, 1.f);
			Desc.vSize = _float2(0.1f, 0.5f);
			Desc.vPivot = _float3(0.f, 0.f, 0.f);
			Desc.vSpeed = _float2(2.f, 7.f);
			Desc.vLifeTime = _float2(0.5f, 1.5f);
			Desc.isLoop = false;


			pDesc->MeshInstanceDesc = &Desc;
		}
		else if (i == 6)		// BigElectric
		{
			pDesc->isInstanceObject = true;
			pDesc->InstanceBufferPrototypeTag = ModelPrototypeName + L"Instance";

			
			Desc.iNumInstance = 80;
			Desc.vCenter = _float3(0.f, 0.f, 0.f);
			Desc.vRange = _float3(1.f, 0.f, 1.f);
			Desc.vSize = _float2(1.f, 4.f);
			Desc.vPivot = _float3(0.f, 0.f, 0.f);
			Desc.vSpeed = _float2(1.f, 90.f);
			Desc.vLifeTime = _float2(0.5f, 2.f);
			Desc.isLoop = false;
			Desc.isStartFromOrigin = true;

			pDesc->MeshInstanceDesc = &Desc;
		}
		

		for (size_t i = 0; i < meshCount; i++)
		{
			fread(&pDesc[i].iMaterialIndex, sizeof(pDesc[i].iMaterialIndex), 1, fin);
			fread(&pDesc[i].iNumVertices, sizeof(pDesc[i].iNumVertices), 1, fin);
			fread(&pDesc[i].iVertexStride, sizeof(pDesc[i].iVertexStride), 1, fin);

			pDesc[i].pVertices = new VTXMESH[pDesc[i].iNumVertices];
			fread(pDesc[i].pVertices, sizeof(VTXMESH) * pDesc[i].iNumVertices, 1, fin);

			fread(&pDesc[i].iNumIndices, sizeof(pDesc[i].iNumIndices), 1, fin);
			fread(&pDesc[i].iIndexStride, sizeof(pDesc[i].iIndexStride), 1, fin);

			pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
			fread(pDesc[i].pIndices, sizeof(_uint) * pDesc[i].iNumIndices, 1, fin);

			fread(&pDesc[i].eIndexFormat, sizeof(pDesc[i].eIndexFormat), 1, fin);
			fread(&pDesc[i].eTopology, sizeof(pDesc[i].eTopology), 1, fin);

			fread(&pDesc[i].vMinPos, sizeof(pDesc[i].vMinPos), 1, fin);
			fread(&pDesc[i].vMaxPos, sizeof(pDesc[i].vMaxPos), 1, fin);
		}

		if (true == m_pGameInstance->IsFind_Model(LEVEL_GAMEPLAY, ModelPrototypeName)) // 내가 불러오고자 하는 모델이 이미 불러왔어?
		{
			// 모델을 찾았을 때의 처리
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelPrototypeName, // 모델 프로토타입 이름
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE::TYPE_NONANIM, // 모델타입
					modelFilePath.c_str(), // 모델 fbx 경로
					*wstrMaterialTexturePath, // 머테리얼 텍스쳐 경로
					meshCount, // 메쉬 갯수
					materialsCount, // 머테리얼 갯수
					XMLoadFloat4x4(&PreTransformMatrix), // 더미
					pDesc)))) // 메쉬를 이루기 위한 정보
			{
				_wstring msg = to_wstring(i) + L"번째" + L"모델이 로드가 안댐";
				MSG_BOX(msg.c_str());
				return E_FAIL;
			}
		}


		for (size_t i = 0; i < meshCount; i++)
		{
			Safe_Delete_Array(pDesc[i].pVertices);
			Safe_Delete_Array(pDesc[i].pIndices);
		}
		Safe_Delete_Array(pDesc);
	}

	fclose(fin);

	return S_OK;
}

HRESULT CLoader::Load_FinalMap()
{
	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("맵 데이터를 로딩중..."));

	FILE* fin = nullptr;

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_GAMEPLAY:
	{

	}
	break;
	case Client::LEVEL_STAGE1:
	{
		fopen_s(&fin, "../Bin/Stage1/FinalMap_Data.bin", "rb");
	}
	break;
	case Client::LEVEL_STAGE1_BOSS:
	{
		fopen_s(&fin, "../Bin/Stage1_BossMap/FinalMap_Data.bin", "rb");
	}
	break;
	case Client::LEVEL_STAGE2_BOSS:
	{
		fopen_s(&fin, "../Bin/Stage2_BossMap/FinalMap_Data.bin", "rb");
	}
	break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}


	if (!fin)    // 파일 열기에 실패했다면
	{
		MSG_BOX(TEXT("파일 읽기를 실패했어요.."));
		return E_FAIL;
	}

	CModel* pModel = nullptr;

	_uint iModelSize = 0;
	fread(&iModelSize, sizeof(iModelSize), 1, fin);

	for (_uint i = 1; i < iModelSize + 1; i++)
	{
		// PreTransformMatrix 읽기
		_float4x4 PreTransformMatrix;
		fread(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fin);

		// Prototype Name 크기 읽기
		_uint modelPrototypeNameSize = 0;
		fread(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fin);

		// Prototype Name 읽기
		std::string modelPrototypeName(modelPrototypeNameSize, '\0');
		fread(&modelPrototypeName[0], modelPrototypeNameSize, 1, fin);

		// Model File Path 크기 읽기
		_uint modelFilePathSize = 0;
		fread(&modelFilePathSize, sizeof(modelFilePathSize), 1, fin);

		// Model File Path 읽기
		std::string modelFilePath(modelFilePathSize, '\0');
		fread(&modelFilePath[0], modelFilePathSize, 1, fin);

		// Mesh 및 Material 정보 읽기
		_uint meshCount = 0;
		_uint materialsCount = 0;
		fread(&meshCount, sizeof(meshCount), 1, fin);
		fread(&materialsCount, sizeof(materialsCount), 1, fin);

		string					strMaterialTexturePath = "";
		_wstring				wstrMaterialTexturePath[200][AI_TEXTURE_TYPE_MAX] = { L"" };			// << 이거 최종맵데이터는 메테리얼갯수많아

		// Material Texture Path 읽기
		for (size_t i = 0; i < materialsCount; i++)
		{
			for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			{
				ReadString(fin, strMaterialTexturePath);
				wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // 머테리얼 경로
			}
		}


		_wstring ModelPrototypeName = stringToWstring(modelPrototypeName);

		CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[meshCount];
		pDesc->isInstanceObject = false;
		//pDesc->InstanceBufferPrototypeTag = ModelPrototypeName + L"Instance";


		for (size_t i = 0; i < meshCount; i++)
		{
			fread(&pDesc[i].iMaterialIndex, sizeof(pDesc[i].iMaterialIndex), 1, fin);
			fread(&pDesc[i].iNumVertices, sizeof(pDesc[i].iNumVertices), 1, fin);
			fread(&pDesc[i].iVertexStride, sizeof(pDesc[i].iVertexStride), 1, fin);

			pDesc[i].pVertices = new VTXMESH[pDesc[i].iNumVertices];
			fread(pDesc[i].pVertices, sizeof(VTXMESH) * pDesc[i].iNumVertices, 1, fin);

			fread(&pDesc[i].iNumIndices, sizeof(pDesc[i].iNumIndices), 1, fin);
			fread(&pDesc[i].iIndexStride, sizeof(pDesc[i].iIndexStride), 1, fin);

			pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
			fread(pDesc[i].pIndices, sizeof(_uint) * pDesc[i].iNumIndices, 1, fin);

			fread(&pDesc[i].eIndexFormat, sizeof(pDesc[i].eIndexFormat), 1, fin);
			fread(&pDesc[i].eTopology, sizeof(pDesc[i].eTopology), 1, fin);

			fread(&pDesc[i].vMinPos, sizeof(pDesc[i].vMinPos), 1, fin);
			fread(&pDesc[i].vMaxPos, sizeof(pDesc[i].vMaxPos), 1, fin);
		}

		if (true == m_pGameInstance->IsFind_Model(g_CurLevel, ModelPrototypeName)) // 내가 불러오고자 하는 모델이 이미 불러왔어?
		{
			// 모델을 찾았을 때의 처리
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_Prototype(g_CurLevel, ModelPrototypeName, // 모델 프로토타입 이름
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE::TYPE_NONANIM, // 모델타입
					modelFilePath.c_str(), // 모델 fbx 경로
					*wstrMaterialTexturePath, // 머테리얼 텍스쳐 경로
					meshCount, // 메쉬 갯수
					materialsCount, // 머테리얼 갯수
					XMLoadFloat4x4(&PreTransformMatrix), // 더미
					pDesc)))) // 메쉬를 이루기 위한 정보
			{
				_wstring msg = to_wstring(i) + L"번째" + L"모델이 로드가 안댐";
				MSG_BOX(msg.c_str());
				return E_FAIL;
			}
		}


		for (size_t i = 0; i < meshCount; i++)
		{
			Safe_Delete_Array(pDesc[i].pVertices);
			Safe_Delete_Array(pDesc[i].pIndices);
		}
		Safe_Delete_Array(pDesc);
	}

	fclose(fin);

	return S_OK;
}

HRESULT CLoader::Load_Player()
{
	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("플레이어를 로딩중.."));

	FILE* file;

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_GAMEPLAY:
		break;
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_STAGE1:
	{
		file = fopen("../Bin/Player_Model_Data.bin", "rb");
		if (!file) {
			cerr << "파일 읽기를 실패했어요.." << endl;
			return E_FAIL;
		}

		_uint					iObjectCount = {};

		_uint					iObjectType = (_uint)MODEL_CHECK_LIST::MODEL_CHECK_TYPE_END;

		string					strMyPrototypeName = "";
		_wstring				wstrMyPrototypeName = L"";

		string					strMyLayerName = "";
		_wstring				wstrMyLayerName = L"";

		string					strMyModelPrototypeName = "";
		_wstring				wstrMyModelPrototypeName = L"";

		_uint					iModelType = 0;

		string					strMyModelFilePath = "";

		string					strMaterialTexturePath = "";
		_wstring				wstrMaterialTexturePath[15][AI_TEXTURE_TYPE_MAX] = { L"" };


		_uint					iMeshesCount = 0;
		_uint					iMaterialsCount = 0;

		_float4x4				PreTransformMatrix = {};




		// Player 파츠가 2개니까 일단
		m_pLoadingPlayerInfo[LEVEL_GAMEPLAY] = new LOADING_OBJECT_INFO[CPlayer::PARTID::PART_END];

		{
			// 객체원형 //
			fread(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType), 1, file);

			// 프로토타입 이름 읽기
			ReadString(file, strMyPrototypeName);
			wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

			// 레이어 이름 읽기
			ReadString(file, strMyLayerName);
			wstrMyLayerName = stringToWstring(strMyLayerName);

			if (FAILED(Create_PrototypePlayer(wstrMyPrototypeName, wstrMyLayerName)))
				return E_FAIL;

			// 모델 //
			// 모델의 프로토타입이름 가져와야해
			// 모델의 논애니 / 애니 인지 타입 가져와야해
			// 모델의 경로 가져와야해

			fread(reinterpret_cast<char*>(&PreTransformMatrix), sizeof(PreTransformMatrix), 1, file);

			ReadString(file, strMyModelPrototypeName);
			wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

			// 아 이거 모델 프로토타입 생성하는거다
			// 근데 모델프로토 타입 이미 잇냐?라고 찾기전에 생성한뒤에 찾으니까 오래걸려 + 낭비 + 문제도 잇어
			// 생성하기전에 물어보고 Add 해야겟는디 ?

			fread(reinterpret_cast<char*>(&iModelType), sizeof(iModelType), 1, file);

			ReadString(file, strMyModelFilePath); // 모델 fbx경로 _char* 를 요구하기에 string 으로해야함

			fread(reinterpret_cast<char*>(&iMeshesCount), sizeof(iMeshesCount), 1, file);
			fread(reinterpret_cast<char*>(&iMaterialsCount), sizeof(iMaterialsCount), 1, file);

			for (size_t i = 0; i < iMaterialsCount; i++)
			{
				for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				{
					ReadString(file, strMaterialTexturePath);
					wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // 머테리얼 경로
				}
			}

			CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[iMeshesCount];

			CBone::INIT_BONE_DESC Desc = {};

			CAnimation::ANIMATION_DESC AnimationDesc = {};

			vector<vector<_uint>> vecKeyFrameIndices;

			for (size_t i = 0; i < iMeshesCount; i++)
			{
				fread(reinterpret_cast<char*>(&pDesc[i].iMaterialIndex), sizeof(pDesc[i].iMaterialIndex), 1, file);
				fread(reinterpret_cast<char*>(&pDesc[i].iNumVertices), sizeof(pDesc[i].iNumVertices), 1, file);
				fread(reinterpret_cast<char*>(&pDesc[i].iVertexStride), sizeof(pDesc[i].iVertexStride), 1, file);

				pDesc[i].pAnimVertices = new VTXANIMMESH[pDesc[i].iNumVertices];
				fread(reinterpret_cast<char*>(pDesc[i].pAnimVertices), sizeof(VTXANIMMESH) * pDesc[i].iNumVertices, 1, file);

				fread(reinterpret_cast<char*>(&pDesc[i].iNumIndices), sizeof(pDesc[i].iNumIndices), 1, file);
				fread(reinterpret_cast<char*>(&pDesc[i].iIndexStride), sizeof(pDesc[i].iIndexStride), 1, file);

				pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
				fread(reinterpret_cast<char*>(pDesc[i].pIndices), sizeof(_uint) * pDesc[i].iNumIndices, 1, file);

				fread(reinterpret_cast<char*>(&pDesc[i].eIndexFormat), sizeof(pDesc[i].eIndexFormat), 1, file);
				fread(reinterpret_cast<char*>(&pDesc[i].eTopology), sizeof(pDesc[i].eTopology), 1, file);

				fread(reinterpret_cast<char*>(&pDesc[i].vMinPos), sizeof(pDesc[i].vMinPos), 1, file);
				fread(reinterpret_cast<char*>(&pDesc[i].vMaxPos), sizeof(pDesc[i].vMaxPos), 1, file);

				_uint iNumModelBones = {};
				fread(reinterpret_cast<char*>(&iNumModelBones), sizeof(iNumModelBones), 1, file);

				Desc.vecChildBones.resize(iNumModelBones);

				for (size_t i = 0; i < iNumModelBones; i++) // i가 0은 루트니까 
				{
					_int iParentBoneIndex = {};
					_uint iNumChildBone = {};

					_char* szName = nullptr;
					size_t nameLength = {};

					_float4x4 TransformationMatrix = {};
					_float4x4 CombinedTransformationMatrix = {};

					fread(reinterpret_cast<char*>(&iParentBoneIndex), sizeof(iParentBoneIndex), 1, file);
					fread(reinterpret_cast<char*>(&iNumChildBone), sizeof(iNumChildBone), 1, file);

					fread(reinterpret_cast<char*>(&nameLength), sizeof(nameLength), 1, file);
					szName = new char[nameLength + 1];
					fread(szName, nameLength, 1, file);
					szName[nameLength] = '\0';

					fread(reinterpret_cast<char*>(&TransformationMatrix), sizeof(TransformationMatrix), 1, file);
					fread(reinterpret_cast<char*>(&CombinedTransformationMatrix), sizeof(CombinedTransformationMatrix), 1, file);

					Desc.vecChildBones[i].iParentBoneIndex = iParentBoneIndex;
					Desc.vecChildBones[i].iNumChildBone = iNumChildBone;

					strcpy(Desc.vecChildBones[i].szName, szName);

					Desc.vecChildBones[i].TransformationMatrix = TransformationMatrix;
					Desc.vecChildBones[i].CombinedTransformaionMatrix = CombinedTransformationMatrix;

					Safe_Delete(szName);
				}

				// 뼈 끗

				_char* szName = nullptr;
				size_t nameLength = {};

				fread(reinterpret_cast<char*>(&nameLength), sizeof(nameLength), 1, file);
				szName = new char[nameLength + 1];
				fread(szName, nameLength, 1, file);
				szName[nameLength] = '\0';

				_uint iNum_AffectedBones = {};
				fread(reinterpret_cast<char*>(&iNum_AffectedBones), sizeof(iNum_AffectedBones), 1, file);

				vector<_uint> vecIndices;
				vecIndices.resize(iNum_AffectedBones);

				vector<_float4x4> vecOffsetMatrix;
				vecOffsetMatrix.resize(iNum_AffectedBones);

				for (size_t i = 0; i < iNum_AffectedBones; i++) // 내 생각이 맞다면 이 사이즈와 BoneIndices.size 얘의 사이즈는 같음 
				{
					fread(reinterpret_cast<char*>(&vecIndices[i]), sizeof(vecIndices[i]), 1, file);
					fread(reinterpret_cast<char*>(&vecOffsetMatrix[i]), sizeof(vecOffsetMatrix[i]), 1, file);
				}

				strcpy(pDesc[i].pName, szName);
				pDesc[i].vecIndices.resize(iNum_AffectedBones);
				pDesc[i].vecOffsetMatrix.resize(iNum_AffectedBones);

				pDesc[i].iNumAffectBones = iNum_AffectedBones;
				pDesc[i].vecIndices = vecIndices;
				pDesc[i].vecOffsetMatrix = vecOffsetMatrix;

				Safe_Delete(szName);

				_uint iNumAnimation = {};
				fread(reinterpret_cast<char*>(&iNumAnimation), sizeof(iNumAnimation), 1, file);

				_uint iNumChannel = {};

				AnimationDesc.vecAnimationDesc.resize(iNumAnimation);

				for (size_t i = 0; i < iNumAnimation; i++)
				{
					_char* szName = nullptr;
					size_t nameLength = {};

					fread(reinterpret_cast<char*>(&nameLength), sizeof(nameLength), 1, file);
					szName = new char[nameLength + 1];
					fread(szName, nameLength, 1, file);
					szName[nameLength] = '\0';

					_double Duration = {};
					_double SpeedPerSec = {};

					fread(reinterpret_cast<char*>(&Duration), sizeof(Duration), 1, file);
					fread(reinterpret_cast<char*>(&SpeedPerSec), sizeof(SpeedPerSec), 1, file);
					fread(reinterpret_cast<char*>(&iNumChannel), sizeof(iNumChannel), 1, file);

					AnimationDesc.vecAnimationDesc[i].vecChannelDesc.resize(iNumChannel);

					for (size_t j = 0; j < iNumChannel; j++)
					{
						_char* szName = nullptr;
						size_t nameLength = {};

						_uint iBoneIndex = {};
						_uint iNumKeyFrame = {};

						fread(reinterpret_cast<char*>(&nameLength), sizeof(nameLength), 1, file);
						szName = new char[nameLength + 1];
						fread(szName, nameLength, 1, file);
						szName[nameLength] = '\0';

						fread(reinterpret_cast<char*>(&iBoneIndex), sizeof(iBoneIndex), 1, file);
						fread(reinterpret_cast<char*>(&iNumKeyFrame), sizeof(iNumKeyFrame), 1, file);

						strcpy(AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].pName, szName);
						AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].iBoneIndex = iBoneIndex;
						AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].iNumKeyFrames = iNumKeyFrame;

						Safe_Delete(szName);

						AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames.resize(iNumKeyFrame);

						for (size_t k = 0; k < iNumKeyFrame; k++)
						{
							_float3 vScale = {};
							_float4 vRotation = {};
							_float3 vTranslation = {};
							_double vTarckPosition = {};

							fread(reinterpret_cast<char*>(&vScale), sizeof(vScale), 1, file);
							fread(reinterpret_cast<char*>(&vRotation), sizeof(vRotation), 1, file);
							fread(reinterpret_cast<char*>(&vTranslation), sizeof(vTranslation), 1, file);
							fread(reinterpret_cast<char*>(&vTarckPosition), sizeof(vTarckPosition), 1, file);

							AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames[k].vScale = vScale;
							AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames[k].vRotation = vRotation;
							AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames[k].vTranslation = vTranslation;
							AnimationDesc.vecAnimationDesc[i].vecChannelDesc[j].vecKeyFrames[k].TrackPosition = vTarckPosition;
						}
					}

					strcpy(AnimationDesc.vecAnimationDesc[i].szName, szName);
					AnimationDesc.vecAnimationDesc[i].Duration = Duration;
					AnimationDesc.vecAnimationDesc[i].SpeedPerSec = SpeedPerSec;
					AnimationDesc.vecAnimationDesc[i].iNumChannels = iNumChannel;
					Safe_Delete(szName);
				}
			}

			if (true == m_pGameInstance->IsFind_Model(LEVEL_GAMEPLAY, wstrMyModelPrototypeName)) // 내가 불러오고자 하는 모델이 이미 불러왔어?
			{

			}
			// 모델을 만들어야해 ?
			// 모델을 만들지 않아도 읽어주긴해야지
			else
			{
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstrMyModelPrototypeName, // 모델 프로토타입 이름
					CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType, // 모델타입
						strMyModelFilePath.c_str(), // 모델 fbx 경로
						*wstrMaterialTexturePath, // 머테리얼 텍스쳐 경로
						iMeshesCount, // 메쉬 갯수
						iMaterialsCount, // 머테리얼 갯수
						XMLoadFloat4x4(&PreTransformMatrix), // 더미
						pDesc,
						reinterpret_cast<void*>(&Desc),
						reinterpret_cast<void*>(&AnimationDesc)))))
				{
					_wstring msg = to_wstring(CPlayer::PARTID::PART_BODY) + L"번째" + L"모델이 로드가 안댐";
					MSG_BOX(msg.c_str());
					return E_FAIL;
				}
			}

			// 월드 행렬 읽기
			_float4x4 WorldMatrix;
			fread(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix), 1, file);

			m_pLoadingPlayerInfo[LEVEL_GAMEPLAY][CPlayer::PARTID::PART_BODY].strLayerName = wstrMyLayerName;
			m_pLoadingPlayerInfo[LEVEL_GAMEPLAY][CPlayer::PARTID::PART_BODY].strPrototypeName = wstrMyPrototypeName;
			m_pLoadingPlayerInfo[LEVEL_GAMEPLAY][CPlayer::PARTID::PART_BODY].strModelPrototypeName = wstrMyModelPrototypeName;
			XMStoreFloat4x4(&m_pLoadingPlayerInfo[LEVEL_GAMEPLAY][CPlayer::PARTID::PART_BODY].vWorldMatrix, XMMatrixIdentity());

			for (size_t i = 0; i < iMeshesCount; i++)
			{
				Safe_Delete_Array(pDesc[i].pAnimVertices);
				Safe_Delete_Array(pDesc[i].pIndices);
			}
			Safe_Delete_Array(pDesc);
		}


		{
			// 무기
			// 객체원형 //
			fread(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType), 1, file);

			// 프로토타입 이름 읽기
			ReadString(file, strMyPrototypeName);
			wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

			// 레이어 이름 읽기
			ReadString(file, strMyLayerName);
			wstrMyLayerName = stringToWstring(strMyLayerName);


			for (_uint i = 0; i < CWeapon_Player::WEAPON_TYPE_END; i++)
			{
				// 모델 //
				fread(reinterpret_cast<char*>(&PreTransformMatrix), sizeof(PreTransformMatrix), 1, file);

				ReadString(file, strMyModelPrototypeName);
				wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

				fread(reinterpret_cast<char*>(&iModelType), sizeof(iModelType), 1, file);

				ReadString(file, strMyModelFilePath);  // 모델 fbx경로 _char* 를 요구하기에 string 으로 해야함

				fread(reinterpret_cast<char*>(&iMeshesCount), sizeof(iMeshesCount), 1, file);
				fread(reinterpret_cast<char*>(&iMaterialsCount), sizeof(iMaterialsCount), 1, file);

				for (size_t i = 0; i < iMaterialsCount; i++)
				{
					for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
					{
						ReadString(file, strMaterialTexturePath);
						wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // 머테리얼 경로
					}
				}

				CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[iMeshesCount];

				for (size_t i = 0; i < iMeshesCount; i++)
				{
					fread(reinterpret_cast<char*>(&pDesc[i].iMaterialIndex), sizeof(pDesc[i].iMaterialIndex), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].iNumVertices), sizeof(pDesc[i].iNumVertices), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].iVertexStride), sizeof(pDesc[i].iVertexStride), 1, file);

					pDesc[i].pVertices = new VTXMESH[pDesc[i].iNumVertices];
					fread(reinterpret_cast<char*>(pDesc[i].pVertices), sizeof(VTXMESH) * pDesc[i].iNumVertices, 1, file);

					fread(reinterpret_cast<char*>(&pDesc[i].iNumIndices), sizeof(pDesc[i].iNumIndices), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].iIndexStride), sizeof(pDesc[i].iIndexStride), 1, file);

					pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
					fread(reinterpret_cast<char*>(pDesc[i].pIndices), sizeof(_uint) * pDesc[i].iNumIndices, 1, file);

					fread(reinterpret_cast<char*>(&pDesc[i].eIndexFormat), sizeof(pDesc[i].eIndexFormat), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].eTopology), sizeof(pDesc[i].eTopology), 1, file);

					fread(reinterpret_cast<char*>(&pDesc[i].vMinPos), sizeof(pDesc[i].vMinPos), 1, file);
					fread(reinterpret_cast<char*>(&pDesc[i].vMaxPos), sizeof(pDesc[i].vMaxPos), 1, file);
				}

				if (true == m_pGameInstance->IsFind_Model(LEVEL_GAMEPLAY, wstrMyModelPrototypeName))  // 내가 불러오고자 하는 모델이 이미 불러왔어?
				{
					// 모델을 찾았을 때의 처리
				}
				else
				{
					if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstrMyModelPrototypeName, // 모델 프로토타입 이름
						CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType,  // 모델타입
							strMyModelFilePath.c_str(),  // 모델 fbx 경로
							*wstrMaterialTexturePath,  // 머테리얼 텍스쳐 경로
							iMeshesCount,  // 메쉬 갯수
							iMaterialsCount,  // 머테리얼 갯수
							XMLoadFloat4x4(&PreTransformMatrix),  // 더미
							pDesc))))  // 메쉬를 이루기 위한 정보
					{
						_wstring msg = to_wstring(CPlayer::PARTID::PART_WEAPON) + L"번째" + L"모델이 로드가 안댐";
						MSG_BOX(msg.c_str());
						return E_FAIL;
					}
				}


				m_pLoadingPlayerInfo[LEVEL_GAMEPLAY][CPlayer::PARTID::PART_WEAPON].strLayerName = wstrMyLayerName;
				m_pLoadingPlayerInfo[LEVEL_GAMEPLAY][CPlayer::PARTID::PART_WEAPON].strPrototypeName = wstrMyPrototypeName;
				m_pLoadingPlayerInfo[LEVEL_GAMEPLAY][CPlayer::PARTID::PART_WEAPON].strModelPrototypeName = wstrMyModelPrototypeName;
				XMStoreFloat4x4(&m_pLoadingPlayerInfo[LEVEL_GAMEPLAY][CPlayer::PARTID::PART_WEAPON].vWorldMatrix, XMMatrixIdentity());

				for (size_t i = 0; i < iMeshesCount; i++)
				{
					Safe_Delete_Array(pDesc[i].pVertices);
					Safe_Delete_Array(pDesc[i].pIndices);
				}
				Safe_Delete_Array(pDesc);

			}


			// 월드 행렬 읽기
			_float4x4 WorldMatrix;
			fread(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix), 1, file);

		}



		fclose(file);
		//MSG_BOX(TEXT("LEVEL_GAME_PLAY의 Player 정보를 Load했습니다."));
	}
	break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CLoader::Create_PrototypeObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Decorative_Object"),
		CDecorative_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Static_Object"),
		CStatic_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_Object"),
		CDynamic_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_PrototypeAnimObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spider"),
		CSpider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Elite"),
		CElite::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Elite"),
		CWeapon_Elite::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hel"),
		CHel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Hel"),
		CWeapon_Hel::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Jetpack"),
		CJetpack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Jetpack"),
		CWeapon_Jetpack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackPack_Jetpack"),
		CBackPack_Jetpack::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mira"),
		CMira::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pistol"),
		CPistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Pistol"),
		CWeapon_Pistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sniper"),
		CSniper::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Sniper"),
		CWeapon_Sniper::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Bullet"),
		CMonster_Bullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLoader::Create_PrototypePlayer(const _wstring& strPrototypeName, const _wstring& strLayerName)
{
	CGameObject* pPrototype = nullptr;

	pPrototype = CPlayer::Create(m_pDevice, m_pContext);
	m_pGameInstance->Add_Prototype(strPrototypeName, pPrototype);


	pPrototype = CBody_Player::Create(m_pDevice, m_pContext);
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"), pPrototype);


	pPrototype = CWeapon_Player::Create(m_pDevice, m_pContext);
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Player"), pPrototype);

	pPrototype = CSubShuriken::Create(m_pDevice, m_pContext);
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SubShuriken"), pPrototype);



	



	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Wire"),
		CVIBuffer_Wire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wire_Player"),
		CWire_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_UI()
{	
	/* For. Prototype_Component_Texture_GrapplingPointUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grappling_point_dot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GrapplingPoint/Grappling_point_dot.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grappling_point_frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GrapplingPoint/Grappling_point_frame.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grappling_point_outer_ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GrapplingPoint/Grappling_point_outer_ring.png"), 1))))
		return E_FAIL;


	/* For. Prototype_Component_Texture_IconUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Block"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/block.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Shuriken"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/shuriken.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Dash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/dash.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_MindControl"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/mindcontrol.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Nami"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/nami.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_CutAll"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/cutall.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_TimeStop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/timestop.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Sh_Skill"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/Sh_Skill.png"), 1))))
		return E_FAIL;



	/* For. Prototype_Component_Texture_NumberUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Number/DebugNumberStrip.png"), 1))))
		return E_FAIL;


	/* For. Prototype_Component_Texture_EnemyMarkerUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EnemyMarker"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/target.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EnemyMarkerMC"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/targetMC.png"), 1))))
		return E_FAIL;


	/* For. Prototype_Component_Texture_CrossHairUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CrossHair"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/CrossHair/crosshair.png"), 1))))
		return E_FAIL;

	
	/* For. Prototype_Component_Texture_EnemyFindUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EnemyFind"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Icon/EnemyFind.png"), 1))))
		return E_FAIL;
	
	/* For. Prototype_Component_Texture_MiniMap */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MiniMapPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GR/MiniMapPanel.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MiniMapMarker"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GR/MiniMapMarker.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MiniMapPlayer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GR/MiniMapPlayer.png"), 1))))
		return E_FAIL;



	/* For. Prototype_Component_Texture_KillCount*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_KillCount"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GR/KillCount.png"), 1))))
		return E_FAIL;


	/* For. Prototype_Component_Texture_BossHp*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossHp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GR/hp.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossEnergy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GR/energy.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/GR/separtor.png"), 1))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GrapplingPointUI"),
		CGrapplingPointUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IconUI"),
		CIconUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NumberUI"),
		CNumberUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyMarkerUI"),
		CEnemyMarker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyMarkerMC"),
		CEnemyMarkerMC::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CCrossHairUI"),
		CCrossHairUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EventNotify"),
		CEventNotify::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyFind"),
		CEnemyFindUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniMapPanel"),
		CMiniMapPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniMapMarker"),
		CMiniMapMarker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniMapPlayer"),
		CMiniMapPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossHpPanel"),
		CBossHpPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossHpMain"),
		CBossHpMain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossHpEnergy"),
		CBossHpEnergy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KillCount"),
		CKillCountUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	
	
	return S_OK;
}

HRESULT CLoader::Create_Sky()
{
	/* For. Prototype_Component_Texture_Sky */
	// TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds")

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky_Blue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Sky/Blue.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky_SkyBlue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Sky/SkyBlue.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky_Red"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Sky/Red.dds"), 1))))
		return E_FAIL;


	/* For. Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Create_Particle()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_DESC			ParticleRainDesc{};
	ZeroMemory(&ParticleRainDesc, sizeof ParticleRainDesc);
	ParticleRainDesc.iNumInstance = 3000;
	ParticleRainDesc.vCenter = _float3(0.f, 70.f, 0.f);
	ParticleRainDesc.vRange = _float3(600.f, 70.f, 600.f);
	ParticleRainDesc.vSize = _float2(1.2f, 150.f);
	ParticleRainDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleRainDesc.vSpeed = _float2(50.f, 300.f);
	ParticleRainDesc.vLifeTime = _float2(3.f, 6.f);
	ParticleRainDesc.isLoop = true;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Rain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/rain_drop.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rain"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleRainDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Rain"),
		CParticle_Rain::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	if (FAILED(Create_SwordTrail()))
		return E_FAIL;

	if (FAILED(Create_ShurikenTrail()))
		return E_FAIL;
	

	if (FAILED(Create_ShurikenEffect()))
		return E_FAIL;

	if (FAILED(Create_KatanaBlockEffect()))
		return E_FAIL;

	if (FAILED(Create_CutAllEffect()))
		return E_FAIL;

	if (FAILED(Create_NamiEffect()))
		return E_FAIL;

	if (FAILED(Create_Blood()))
		return E_FAIL;

	if (FAILED(Create_ShockWave()))
		return E_FAIL;

	if (FAILED(Create_EliteBlockEffect()))
		return E_FAIL;
	
	if (FAILED(Create_ExplosionEffect()))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Create_SwordTrail()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SwordTrail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_Fibers.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSwordTrail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxSwordTrail.hlsl"), VTXTRAILTEX::Elements, VTXTRAILTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_SwordTrail"),	 // 트레일 마다 공유해버리면 버텍스버퍼와 인덱스 버퍼를공유하기에 개별로 플토타입을 만들어줘야할것같다 ...
		CVIBuffer_Trail::Create(m_pDevice, m_pContext , 80))))	// Player SwordTrail
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_EliteSwordTrail"),	 // 트레일 마다 공유해버리면 버텍스버퍼와 인덱스 버퍼를공유하기에 개별로 플토타입을 만들어줘야할것같다 ...
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 150))))	// Elite SwordTrail
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_HelSwordTrail"),	 // 트레일 마다 공유해버리면 버텍스버퍼와 인덱스 버퍼를공유하기에 개별로 플토타입을 만들어줘야할것같다 ...
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 150))))	// Hel SwordTrail
		return E_FAIL;


	/* For. Prototype_GameObject_Particle_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwordTrail"),
		CSwordTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EliteSwordTrail"),
		CEliteSwordTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HelSwordTrail"),
		CHelSwordTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HelMotionTrail"),				//  모  션 트  레   일 !!!
		CHelMotionTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EliteMotionTrail"),				//  모  션 트  레   일 !!!
		CEliteMotionTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Create_ShurikenTrail()
{

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_MainShurikenTrail"),	 // 트레일 마다 공유해버리면 버텍스버퍼와 인덱스 버퍼를공유하기에 개별로 플토타입을 만들어줘야할것같다 ...
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 250))))		// 마지막인자: 최대 트레일을 만들갯수
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_SubShuriken0_Trail"),	 // 트레일 마다 공유해버리면 버텍스버퍼와 인덱스 버퍼를공유하기에 개별로 플토타입을 만들어줘야할것같다 ...
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 5))))		// 마지막인자: 최대 트레일을 만들갯수
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_SubShuriken1_Trail"),	 // 트레일 마다 공유해버리면 버텍스버퍼와 인덱스 버퍼를공유하기에 개별로 플토타입을 만들어줘야할것같다 ...
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 5))))		// 마지막인자: 최대 트레일을 만들갯수
		return E_FAIL;



	/* For. Prototype_GameObject_Particle_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShurikenTrail"),
		CShurikenTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_ShurikenEffect()
{
	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
	ParticleDesc.iNumInstance = 90;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(2.f, 2.f, 2.f);
	ParticleDesc.vSize = _float2(10.f, 15.f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(2.f, 10.f);
	ParticleDesc.vLifeTime = _float2(0.5f, 1.5f);		// x ~ y 중에서의 랜덤값이 x에 들어가고 y는 0이다 따라서 y가 x에 도달하는게 라이프타임임
	ParticleDesc.isLoop = true;
	ParticleDesc.isStartFromOrigin = true;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Lightning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Player/Particle/Lightning.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleShuriken"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Player/Particle/T_Star_Bright.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_ShurikenEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_ShurikenEffect"),
		CParticle_ShurikenEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Create_KatanaBlockEffect()
{
	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
	ParticleDesc.iNumInstance = 150;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(3.f, 3.f, 3.f);
	ParticleDesc.vSize = _float2(0.5f, 1.5f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(1.f, 4.f);
	ParticleDesc.vLifeTime = _float2(0.2f, 1.f);
	ParticleDesc.isLoop = false;
	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleBlock"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Player/Particle/Block.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_BlockEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_BlockEffect"),
		CParticle_Block::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_CutAllEffect()
{
	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
	ParticleDesc.iNumInstance = 150;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(2.f, 2.f, 2.f);
	ParticleDesc.vSize = _float2(5.f, 20.f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(1.f, 2.f);
	ParticleDesc.vLifeTime = _float2(0.5f, 2.f);		
	ParticleDesc.isLoop = false;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleCutAll"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Player/Particle/Cutall.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_CutAllEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_CutAllEffect"),
		CParticle_CutAll::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_NamiEffect()
{
	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
	ParticleDesc.iNumInstance = 150;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(5.f, 5.f, 0.f);
	ParticleDesc.vSize = _float2(10.f, 15.f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(5.f, 10.f);
	ParticleDesc.vLifeTime = _float2(3.f, 3.f);
	ParticleDesc.isLoop = false;
	ParticleDesc.isStartFromOrigin = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleNami"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Player/Particle/Nami.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_NamiEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_NamiEffect"),
		CParticle_Nami::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_Blood()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"), VTXMESHINSTANCE::Elements, VTXMESHINSTANCE::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Blood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Blood/T_Blood_Atlas_01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Blood"),
		CParticle_Blood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_ShockWave()
{
	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
	ParticleDesc.iNumInstance = 150;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(3.f, 3.f, 3.f);
	ParticleDesc.vSize = _float2(2.f, 6.f);
	ParticleDesc.vPivot = _float3(0.f, -2.f, 0.f);
	ParticleDesc.vSpeed = _float2(50.f, 90.f);
	ParticleDesc.vLifeTime = _float2(0.3f, 2.f);
	ParticleDesc.isLoop = false;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_ShockWave"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/ShockWave/ShockWave.dds"), 1))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_ShockWaveEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_ShockWave"),
		CParticle_ShockWave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_EliteBlockEffect()
{
	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	ZeroMemory(&ParticleDesc, sizeof(ParticleDesc));
	ParticleDesc.iNumInstance = 100;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(2.f, 2.f, 2.f);
	ParticleDesc.vSize = _float2(1.f, 2.2f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(7.f, 10.f);
	ParticleDesc.vLifeTime = _float2(0.5f, 1.f);
	ParticleDesc.isLoop = false;


	CVIBuffer_Instancing::INSTANCE_DESC			ParticlDasheDesc{};
	ZeroMemory(&ParticlDasheDesc, sizeof(ParticlDasheDesc));
	ParticlDasheDesc.iNumInstance = 100;
	ParticlDasheDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticlDasheDesc.vRange = _float3(1.f, 1.f, 1.f);
	ParticlDasheDesc.vSize = _float2(3.f, 5.f);
	ParticlDasheDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticlDasheDesc.vSpeed = _float2(7.f, 10.f);
	ParticlDasheDesc.vLifeTime = _float2(0.5f, 1.f);
	ParticlDasheDesc.isLoop = false;




	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleEliteBlock"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Player/Particle/EliteBlock.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_EliteBlockEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_EliteBlockEffect"),
		CParticle_EliteBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_EliteBlockDashEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticlDasheDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_EliteDashBlockEffect"),
		CParticle_EliteDashBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Create_ExplosionEffect()
{
	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
	ParticleDesc.iNumInstance = 80;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(170.f, 100.f, 170.f);
	ParticleDesc.vSize = _float2(80.f, 130.f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(7.f, 10.f);
	ParticleDesc.vLifeTime = _float2(0.5f, 1.f);
	ParticleDesc.isLoop = false;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleExplosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Monster/Jetpack/Particle/T_explosion_01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Explosion"),
		CParticle_Explosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_HelEffect()
{
	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("Hel파티클을 로딩중..."));

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Piece"),
		CParticle_Piece::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Electric"),
		CParticle_Electric::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_BigElectric"),
		CParticle_BigElectric::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	//CVIBuffer_Instancing::INSTANCE_DESC			ParticleRainDesc{};
	//ZeroMemory(&ParticleRainDesc, sizeof ParticleRainDesc);
	//ParticleRainDesc.iNumInstance = 3000;
	//ParticleRainDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//ParticleRainDesc.vRange = _float3(500.f, 50.f, 500.f);
	//ParticleRainDesc.vSize = _float2(0.1f, 20.f);
	//ParticleRainDesc.vPivot = _float3(0.f, 0.f, 0.f);
	//ParticleRainDesc.vSpeed = _float2(20.f, 30.f);
	//ParticleRainDesc.vLifeTime = _float2(4.f, 7.f);
	//ParticleRainDesc.isLoop = true;
	//
	//
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Rain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/rain_drop.dds"), 1))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rain"),
	//	CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleRainDesc))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Rain"),
	//	CParticle_Rain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;






	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
	ParticleDesc.iNumInstance = 150;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(5.f, 0.f, 5.f);
	ParticleDesc.vSize = _float2(100.f, 150.f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(180.f, 220.f);
	ParticleDesc.vLifeTime = _float2(1.f, 2.f);
	ParticleDesc.isLoop = false;
	ParticleDesc.isStartFromOrigin = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_HelJump"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_Smoke_8x4.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_HelJumpEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_HelJumpEffect"),
		CParticle_Jump::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	CVIBuffer_Instancing::INSTANCE_DESC			ParticlSmokeeDesc{};
	ZeroMemory(&ParticlSmokeeDesc, sizeof ParticlSmokeeDesc);
	ParticlSmokeeDesc.iNumInstance = 150;
	ParticlSmokeeDesc.vCenter = _float3(0.f, 2.f, 0.f);
	ParticlSmokeeDesc.vRange = _float3(10.f, 10.f, 10.f);
	ParticlSmokeeDesc.vSize = _float2(100.f, 150.f);
	ParticlSmokeeDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticlSmokeeDesc.vSpeed = _float2(30.f, 200.f);
	ParticlSmokeeDesc.vLifeTime = _float2(1.f, 2.f);
	ParticlSmokeeDesc.isLoop = false;
	ParticlSmokeeDesc.isStartFromOrigin = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_HelBigSmoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_Smoke_Dark_Cloud_01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_HelBigSmokeEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticlSmokeeDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_BigSmoke"),
		CParticle_BigSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	///////////////////////////////

	CVIBuffer_Instancing::INSTANCE_DESC			ParticlSwirlDesc{};
	ZeroMemory(&ParticlSwirlDesc, sizeof ParticlSwirlDesc);
	ParticlSwirlDesc.iNumInstance = 300;
	ParticlSwirlDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticlSwirlDesc.vRange = _float3(10.f, 0.f, 10.f);
	ParticlSwirlDesc.vSize = _float2(150.f, 200.f);
	ParticlSwirlDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticlSwirlDesc.vSpeed = _float2(10.f, 220.f);
	ParticlSwirlDesc.vLifeTime = _float2(1.f, 2.f);
	ParticlSwirlDesc.isLoop = false;
	ParticlSwirlDesc.isStartFromOrigin = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_HelSwirl"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_swirl_01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_HelSwirlEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticlSwirlDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Swirl"),
		CParticle_Swirl::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/////////////////////////

	CVIBuffer_Instancing::INSTANCE_DESC			ParticlAttackDesc{};
	ZeroMemory(&ParticlAttackDesc, sizeof ParticlAttackDesc);
	ParticlAttackDesc.iNumInstance = 80;
	ParticlAttackDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticlAttackDesc.vRange = _float3(10.f, 10.f, 10.f);
	ParticlAttackDesc.vSize = _float2(1.f, 4.f);
	ParticlAttackDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticlAttackDesc.vSpeed = _float2(20.f, 40.f);
	ParticlAttackDesc.vLifeTime = _float2(1.f, 2.f);
	ParticlAttackDesc.isLoop = false;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_HelAttack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_Sentry_Decal.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_HelAttackEffect"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticlAttackDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Attack"),
		CParticle_Attack::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}


void CLoader::ReadString(FILE* file, string& str)
{
	_uint length = 0;
	fread(&length, sizeof(length), 1, file);

	str.resize(length);
	fread(&str[0], length, 1, file);
}


HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Ready_Resources_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		//hr = Ready_Resources_For_GamePlayLevel();
		break;
	case LEVEL_STAGE1:
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	case LEVEL_STAGE1_BOSS:
		hr = Ready_Resources_For_Stage1_BossLevel();
		break;
	case LEVEL_STAGE2_BOSS:
		hr = Ready_Resources_For_Stage2_BossLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}


void CLoader::Draw_LoadingText()
{
	_float	fFontSize = 0.7f;
	_uint iTextLength = wcslen(m_szLoadingText);		// 글자 길이
	

	if (m_fTime >= 0.03f)
	{
		m_fTime = 0.f;
		if(m_iTextCount < iTextLength - 1 )
			++m_iTextCount;	
	}

	_tchar Test[2] = {};
	for (_uint i = 0; i < m_iTextCount; i++)
	{
		_float fPosX = (600 + (i * 23));

		Test[0] = m_szLoadingText[i];
		Test[1] = '\0';

		m_pGameInstance->Render_Text(TEXT("Font_145"), Test,
			XMVectorSet(fPosX, g_iWinSizeY - 200.f, 0.f, 1.f),
			fFontSize,
			XMVectorSet(1.f / 1.f, 1.f, 1.f, 1.f),
			0.f,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	m_fTime += 0.016f;
}




HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LogoPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Loading/T_GR_sword_ref.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LogoInnerRing"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Loading/Loading_innerrCirlcle.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LogoOutRing"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Loading/Loading_outerCircle.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LogoName"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/UI/Loading/gr_logo_clean.png"), 1))))
		return E_FAIL;

	
	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoPanel"),
		CLogoPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoInnerRing"),
		CLogoInnerRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoOutRing"),
		CLogoOutRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoName"),
		CLogoName::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	//if (FAILED(Load_Terrain()))
	//	return E_FAIL;

	if (FAILED(Load_Anim_GameObject()))
		return E_FAIL;

	if (FAILED(Load_NonAnim_GameObject()))
		return E_FAIL;

	if (FAILED(Load_Player()))
		return E_FAIL;
	
	if(FAILED(Load_OtherModel()))			// 인스턴싱할 오브젝트 (파티클 같은거)
		return E_FAIL;

	if (FAILED(Load_FinalMap()))			
		return E_FAIL;
	

	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중..."));
	if (FAILED(Create_PrototypeObject()))
		return E_FAIL;
	if (FAILED(Create_PrototypeAnimObject()))
		return E_FAIL;


	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("UI를 로딩중..."));
	if (FAILED(Create_UI()))
		return E_FAIL;

	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("하늘을 로딩중..."));
	if (FAILED(Create_Sky()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DeadNoiseTexture"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_Inky_Smoke_Tile.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CreateDissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/CreateDissolve.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BlurMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_NoiseNormal.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TimeStopRefraction"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_Shield_Lines_Normal.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MindControlRefraction"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_ripple_01.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BlockMaskRefraction"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_RadialMask.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WaterPuddleDistortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/Water_Normal_01.dds"), 1))))
		return E_FAIL;



	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("파티클을 로딩중..."));
	if(FAILED(Create_Particle()))
		return E_FAIL;

	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("FSM을 로딩중..."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Player_FSM"),
		CFsm::Create(m_pDevice, m_pContext, CPlayer::PLAYER_ANIMATIONID::PLAYER_ANIMATION_END))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Hel_FSM"),
		CFsm::Create(m_pDevice, m_pContext, CHel::HEL_ANIMATION::ANIM_END))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Elite_FSM"),
		CFsm::Create(m_pDevice, m_pContext, CElite::ELITE_ANIMATION::ANIM_END))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Sniper_FSM"),
		CFsm::Create(m_pDevice, m_pContext, CSniper::SNIPER_ANIMATION::ANIM_END))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Pistol_FSM"),
		CFsm::Create(m_pDevice, m_pContext, CPistol::PISTOL_ANIMATION::ANIM_END))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Mira_FSM"),
		CFsm::Create(m_pDevice, m_pContext, CMira::MIRA_ANIMATION::ANIM_END))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Jetpack_FSM"),
		CFsm::Create(m_pDevice, m_pContext, CJetpack::JETPACK_ANIMATION::ANIM_END))))
		return E_FAIL;


	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("RigidBody을 로딩중..."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("Collider를 로딩중..."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_PhysX"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_PHYSX))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;



	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중..."));
	/* For. Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_IconUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_IconUI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_IconUI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_NumberUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_NumberUI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_NumberUI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;



	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Stage1_BossLevel()
{
	if (FAILED(Load_Anim_GameObject()))
		return E_FAIL;

	if (FAILED(Load_NonAnim_GameObject()))
		return E_FAIL;

	if (FAILED(Load_FinalMap()))
		return E_FAIL;


	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Stage2_BossLevel()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlowMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/FlowMap_Swirl_Detailed_A.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE2_BOSS, TEXT("Prototype_Component_Texture_Circuit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_Circuit_board_01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE2_BOSS, TEXT("Prototype_Component_Texture_Mossaic"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/T_CS_Mossaic_01.dds"), 1))))
		return E_FAIL;												// 원래 모자이크 경로:  T_CS_Mossaic_01    // T_Architect_VFX_atlas


	if (FAILED(Load_Anim_GameObject()))
		return E_FAIL;

	if (FAILED(Load_NonAnim_GameObject()))
		return E_FAIL;

	if (FAILED(Load_FinalMap()))
		return E_FAIL;

	if (FAILED(Create_HelEffect()))
		return E_FAIL;







	m_iTextCount = 0;
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}



CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();


	for (size_t i = 0; i < LEVELID::LEVEL_END; i++)
	{
		Safe_Delete_Array(m_pLoadingDecorativeObjectInfo[i]);
		Safe_Delete_Array(m_pLoadingStaticObjectInfo[i]);
		Safe_Delete_Array(m_pLoadingDynamicObjectInfo[i]);


		Safe_Delete_Array(m_pLoadingAnimObjectInfo[i]);

		Safe_Delete_Array(m_pLoadingTerrainInfo[i]);


		Safe_Delete_Array(m_pLoadingPlayerInfo[i]);
	}


	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	
}
