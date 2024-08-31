#include "stdafx.h"
#include "..\Public\Loader.h"
#include "Monster.h"
#include "Terrain.h"
#include "FreeCamera.h"
#include "BackGround.h"
#include "GameInstance.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <codecvt>




#include "Aid_props.h"
#include "Building.h"

#include "Player.h"
#include "Body_Player.h"
#include "Weapon_Player.h"

#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"
#include "Channel.h"

#include "Spider.h"

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

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	return S_OK;
}


HRESULT CLoader::Load_Terrain()
{
	switch (m_eNextLevelID) {
	case Client::LEVEL_STATIC:
		break;
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_GAMEPLAY:
	{
		FILE* fin = fopen("../Bin/Map_Data.bin", "rb");
		if (fin == nullptr) {
			MSG_BOX(TEXT("���� �б⸦ �����߾��.."));
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

			lstrcpy(m_szLoadingText, TEXT("����(��) �ε����Դϴ�."));

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

		lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
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
	FILE* file;

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_STATIC:
		break;
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_GAMEPLAY:
	{
		fopen_s(&file, "../Bin/Anim_Model_Data.bin", "rb");
		if (!file)    // ���� �����ٸ�
		{
			MSG_BOX(TEXT("���� �б⸦ �����߾��.."));
			return E_FAIL;
		}

		_uint iObjectCount = {};
		_uint iObjectType = (_uint)OBJECT_CHECK_TYPE::OBJECT_CHECK_TYPE_END;

		string strMyPrototypeName = "";
		wstring wstrMyPrototypeName = L"";

		string strMyLayerName = "";
		wstring wstrMyLayerName = L"";

		string strMyModelPrototypeName = "";
		wstring wstrMyModelPrototypeName = L"";

		_uint iModelType = 0;

		string strMyModelFilePath = "";

		string strMaterialTexturePath = "";
		wstring wstrMaterialTexturePath[10][AI_TEXTURE_TYPE_MAX] = { L"" };

		_uint iMeshesCount = 0;
		_uint iMaterialsCount = 0;

		_float4x4 PreTransformMatrix = {};

		fread(&iObjectCount, sizeof(iObjectCount), 1, file);

		m_iNumLoadingAnimObject[LEVEL_GAMEPLAY] = iObjectCount;
		m_pLoadingAnimObjectInfo[LEVEL_GAMEPLAY] = new LOADING_OBJECT_INFO[iObjectCount];

		for (size_t i = 0; i < iObjectCount; i++)
		{
			// ��ü���� //
			fread(&iObjectType, sizeof(iObjectType), 1, file);

			// ������Ÿ�� �̸� �б�
			ReadString(file, strMyPrototypeName);
			wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

			// ���̾� �̸� �б�
			ReadString(file, strMyLayerName);
			wstrMyLayerName = stringToWstring(strMyLayerName);

			if (FAILED(Create_PrototypeAnimObject(wstrMyPrototypeName, wstrMyLayerName)))
				return E_FAIL;

			// �� //
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

			if (true == m_pGameInstance->IsFind_Model(LEVEL_GAMEPLAY, wstrMyModelPrototypeName))
			{
				int a = 0;
			}
			else
			{
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstrMyModelPrototypeName,
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
					_wstring msg = to_wstring(i) + L"��°" + L"���� �ε尡 �ȴ�";
					MSG_BOX(msg.c_str());
					return E_FAIL;
				}
			}

			// ���� ��� �б�
			_float4x4 WorldMatrix;
			fread(&WorldMatrix, sizeof(WorldMatrix), 1, file);

			m_pLoadingAnimObjectInfo[LEVEL_GAMEPLAY][i].strLayerName = wstrMyLayerName;
			m_pLoadingAnimObjectInfo[LEVEL_GAMEPLAY][i].strPrototypeName = wstrMyPrototypeName;
			m_pLoadingAnimObjectInfo[LEVEL_GAMEPLAY][i].strModelPrototypeName = wstrMyModelPrototypeName;
			XMStoreFloat4x4(&m_pLoadingAnimObjectInfo[LEVEL_GAMEPLAY][i].vWorldMatrix, XMLoadFloat4x4(&WorldMatrix));

			for (size_t i = 0; i < iMeshesCount; i++)
			{
				delete[] pDesc[i].pAnimVertices;
				delete[] pDesc[i].pIndices;
			}
			delete[] pDesc;
		}

		fclose(file);
		MSG_BOX(TEXT("LEVEL_GAME_PLAY�� AnimObject ������ Load�߽��ϴ�."));
	}
	break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CLoader::Load_Player()
{
	FILE* file;

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_STATIC:
		break;
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_GAMEPLAY:
	{
		file = fopen("../Bin/Player_Model_Data.bin", "rb");
		if (!file) {
			cerr << "���� �б⸦ �����߾��.." << endl;
			return E_FAIL;
		}

		_uint					iObjectCount = {};

		_uint					iObjectType = (_uint)OBJECT_CHECK_TYPE::OBJECT_CHECK_TYPE_END;

		string					strMyPrototypeName = "";
		_wstring				wstrMyPrototypeName = L"";

		string					strMyLayerName = "";
		_wstring				wstrMyLayerName = L"";

		string					strMyModelPrototypeName = "";
		_wstring				wstrMyModelPrototypeName = L"";

		_uint					iModelType = 0;

		string					strMyModelFilePath = "";

		string					strMaterialTexturePath = "";
		_wstring				wstrMaterialTexturePath[10][AI_TEXTURE_TYPE_MAX] = { L"" };


		_uint					iMeshesCount = 0;
		_uint					iMaterialsCount = 0;

		_float4x4				PreTransformMatrix = {};



		
		// Player ������ 2���ϱ� �ϴ�
		m_pLoadingPlayerInfo[LEVEL_GAMEPLAY] = new LOADING_OBJECT_INFO[CPlayer::PARTID::PART_END];

		{
			// ��ü���� //
			fread(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType), 1, file);

			// ������Ÿ�� �̸� �б�
			ReadString(file, strMyPrototypeName);
			wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

			// ���̾� �̸� �б�
			ReadString(file, strMyLayerName);
			wstrMyLayerName = stringToWstring(strMyLayerName);

			if (FAILED(Create_PrototypePlayer(wstrMyPrototypeName, wstrMyLayerName)))
				return E_FAIL;

			// �� //
			// ���� ������Ÿ���̸� �����;���
			// ���� ��ִ� / �ִ� ���� Ÿ�� �����;���
			// ���� ��� �����;���

			fread(reinterpret_cast<char*>(&PreTransformMatrix), sizeof(PreTransformMatrix), 1, file);

			ReadString(file, strMyModelPrototypeName);
			wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

			// �� �̰� �� ������Ÿ�� �����ϴ°Ŵ�
			// �ٵ� �������� Ÿ�� �̹� �ճ�?��� ã������ �����ѵڿ� ã���ϱ� �����ɷ� + ���� + ������ �վ�
			// �����ϱ����� ����� Add �ؾ߰ٴµ� ?

			fread(reinterpret_cast<char*>(&iModelType), sizeof(iModelType), 1, file);

			ReadString(file, strMyModelFilePath); // �� fbx��� _char* �� �䱸�ϱ⿡ string �����ؾ���

			fread(reinterpret_cast<char*>(&iMeshesCount), sizeof(iMeshesCount), 1, file);
			fread(reinterpret_cast<char*>(&iMaterialsCount), sizeof(iMaterialsCount), 1, file);

			for (size_t i = 0; i < iMaterialsCount; i++)
			{
				for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				{
					ReadString(file, strMaterialTexturePath);
					wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // ���׸��� ���
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

				for (size_t i = 0; i < iNumModelBones; i++) // i�� 0�� ��Ʈ�ϱ� 
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

				// �� ��

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

				for (size_t i = 0; i < iNum_AffectedBones; i++) // �� ������ �´ٸ� �� ������� BoneIndices.size ���� ������� ���� 
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

			if (true == m_pGameInstance->IsFind_Model(LEVEL_GAMEPLAY, wstrMyModelPrototypeName)) // ���� �ҷ������� �ϴ� ���� �̹� �ҷ��Ծ�?
			{

			}
			// ���� �������� ?
			// ���� ������ �ʾƵ� �о��ֱ��ؾ���
			else
			{
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstrMyModelPrototypeName, // �� ������Ÿ�� �̸�
					CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType, // ��Ÿ��
						strMyModelFilePath.c_str(), // �� fbx ���
						*wstrMaterialTexturePath, // ���׸��� �ؽ��� ���
						iMeshesCount, // �޽� ����
						iMaterialsCount, // ���׸��� ����
						XMLoadFloat4x4(&PreTransformMatrix), // ����
						pDesc,
						reinterpret_cast<void*>(&Desc),
						reinterpret_cast<void*>(&AnimationDesc)))))
				{
					_wstring msg = to_wstring(CPlayer::PARTID::PART_BODY) + L"��°" + L"���� �ε尡 �ȴ�";
					MSG_BOX(msg.c_str());
					return E_FAIL;
				}
			}

			// ���� ��� �б�
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
			// ��ü���� //
			fread(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType), 1, file);

			// ������Ÿ�� �̸� �б�
			ReadString(file, strMyPrototypeName);
			wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

			// ���̾� �̸� �б�
			ReadString(file, strMyLayerName);
			wstrMyLayerName = stringToWstring(strMyLayerName);

			if (FAILED(Create_PrototypeObject(iObjectType, wstrMyPrototypeName, wstrMyLayerName)))
				return E_FAIL;

			// �� //
			fread(reinterpret_cast<char*>(&PreTransformMatrix), sizeof(PreTransformMatrix), 1, file);

			ReadString(file, strMyModelPrototypeName);
			wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

			fread(reinterpret_cast<char*>(&iModelType), sizeof(iModelType), 1, file);

			ReadString(file, strMyModelFilePath);  // �� fbx��� _char* �� �䱸�ϱ⿡ string ���� �ؾ���

			fread(reinterpret_cast<char*>(&iMeshesCount), sizeof(iMeshesCount), 1, file);
			fread(reinterpret_cast<char*>(&iMaterialsCount), sizeof(iMaterialsCount), 1, file);

			for (size_t i = 0; i < iMaterialsCount; i++)
			{
				for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				{
					ReadString(file, strMaterialTexturePath);
					wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // ���׸��� ���
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

			if (true == m_pGameInstance->IsFind_Model(LEVEL_GAMEPLAY, wstrMyModelPrototypeName))  // ���� �ҷ������� �ϴ� ���� �̹� �ҷ��Ծ�?
			{
				// ���� ã���� ���� ó��
			}
			else
			{
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstrMyModelPrototypeName, // �� ������Ÿ�� �̸�
					CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType,  // ��Ÿ��
						strMyModelFilePath.c_str(),  // �� fbx ���
						*wstrMaterialTexturePath,  // ���׸��� �ؽ��� ���
						iMeshesCount,  // �޽� ����
						iMaterialsCount,  // ���׸��� ����
						XMLoadFloat4x4(&PreTransformMatrix),  // ����
						pDesc))))  // �޽��� �̷�� ���� ����
				{
					_wstring msg = to_wstring(CPlayer::PARTID::PART_WEAPON) + L"��°" + L"���� �ε尡 �ȴ�";
						MSG_BOX(msg.c_str());
						return E_FAIL;
				}
			}

			// ���� ��� �б�
			_float4x4 WorldMatrix;
			fread(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix), 1, file);

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



		fclose(file);
		MSG_BOX(TEXT("LEVEL_GAME_PLAY�� Player ������ Load�߽��ϴ�."));

	}
	break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}


	return S_OK;
}


HRESULT CLoader::Load_NonAnim_GameObject()
{
	FILE* file = fopen("../Bin/NonAnim_Model_Data.bin", "rb");

	switch (m_eNextLevelID)
	{
	case Client::LEVEL_STATIC:
		break;
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_GAMEPLAY:
	{
		if (!file) // ���� ���⸦ ������ ���
		{
			MSG_BOX(TEXT("���� �б⸦ �����߾��.."));
			return E_FAIL;
		}

		_uint					iObjectCount = {};

		_uint					iObjectType = (_uint)OBJECT_CHECK_TYPE::OBJECT_CHECK_TYPE_END;

		string					strMyPrototypeName = "";
		_wstring				wstrMyPrototypeName = L"";

		string					strMyLayerName = "";
		_wstring				wstrMyLayerName = L"";

		string					strMyModelPrototypeName = "";
		_wstring				wstrMyModelPrototypeName = L"";

		_uint					iModelType = 0;

		string					strMyModelFilePath = "";

		string					strMaterialTexturePath = "";
		_wstring				wstrMaterialTexturePath[10][AI_TEXTURE_TYPE_MAX] = { L"" };
	

		_uint					iMeshesCount = 0;
		_uint					iMaterialsCount = 0;

		_float4x4				PreTransformMatrix = {};

		fread(&iObjectCount, sizeof(iObjectCount), 1, file);

		// �ش� ������ ����� ������Ʈ�� ������ ũ�⸦ �Ҵ��Ѵ�.
		m_iNumLoadingObject[LEVEL_GAMEPLAY] = iObjectCount;
		m_pLoadingObjectInfo[LEVEL_GAMEPLAY] = new LOADING_OBJECT_INFO[iObjectCount];
		//ZeroMemory(m_pLoadingObjectInfo[LEVEL_GAMEPLAY], sizeof(LOADING_OBJECT_INFO) * iObjectCount); // wstring�� ����ü�� ����� ���ϱ� ���� ����� ����: �Ƿηΰ� �˷���

		for (size_t i = 0; i < iObjectCount; i++)
		{
			// ��ü���� //
			fread(&iObjectType, sizeof(iObjectType), 1, file);

			// ������Ÿ�� �̸� �б�
			ReadString(file, strMyPrototypeName);
			wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

			// ���̾� �̸� �б�
			ReadString(file, strMyLayerName);
			wstrMyLayerName = stringToWstring(strMyLayerName);

			if (FAILED(Create_PrototypeObject(iObjectType, wstrMyPrototypeName, wstrMyLayerName)))
				return E_FAIL;

			// �� //
			fread(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, file);

			ReadString(file, strMyModelPrototypeName);
			wstrMyModelPrototypeName = stringToWstring(strMyModelPrototypeName);

			fread(&iModelType, sizeof(iModelType), 1, file);
			ReadString(file, strMyModelFilePath); // �� fbx��� _char* �� �䱸�ϱ⿡ string ���� �ؾ���

			fread(&iMeshesCount, sizeof(iMeshesCount), 1, file);
			fread(&iMaterialsCount, sizeof(iMaterialsCount), 1, file);

			for (size_t i = 0; i < iMaterialsCount; i++)
			{
				for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				{
					ReadString(file, strMaterialTexturePath);
					wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // ���׸��� ���
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

			if (true == m_pGameInstance->IsFind_Model(LEVEL_GAMEPLAY, wstrMyModelPrototypeName)) // ���� �ҷ������� �ϴ� ���� �̹� �ҷ��Ծ�?
			{
				// ���� ã���� ���� ó��
			}
			else
			{
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstrMyModelPrototypeName, // �� ������Ÿ�� �̸�
					CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iModelType, // ��Ÿ��
						strMyModelFilePath.c_str(), // �� fbx ���
						*wstrMaterialTexturePath, // ���׸��� �ؽ��� ���
						iMeshesCount, // �޽� ����
						iMaterialsCount, // ���׸��� ����
						XMLoadFloat4x4(&PreTransformMatrix), // ����
						pDesc)))) // �޽��� �̷�� ���� ����
				{
					_wstring msg = to_wstring(i) + L"��°" + L"���� �ε尡 �ȴ�";
						MSG_BOX(msg.c_str());
						return E_FAIL;
				}
			}

			// ���� ��� �б�
			_float4x4 WorldMatrix;
			fread(&WorldMatrix, sizeof(WorldMatrix), 1, file);

			m_pLoadingObjectInfo[LEVEL_GAMEPLAY][i].strLayerName = wstrMyLayerName;
			m_pLoadingObjectInfo[LEVEL_GAMEPLAY][i].strPrototypeName = wstrMyPrototypeName;
			m_pLoadingObjectInfo[LEVEL_GAMEPLAY][i].strModelPrototypeName = wstrMyModelPrototypeName;
			m_pLoadingObjectInfo[LEVEL_GAMEPLAY][i].vWorldMatrix = WorldMatrix;

			for (size_t i = 0; i < iMeshesCount; i++)
			{
				Safe_Delete_Array(pDesc[i].pVertices);
				Safe_Delete_Array(pDesc[i].pIndices);
			}
			Safe_Delete_Array(pDesc);
		}

		fclose(file);
		
		//MSG_BOX(TEXT("LEVEL_GAME_PLAY�� NonAnimObject ������ Load�߽��ϴ�."));

	}
	break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}


	return S_OK;
}






HRESULT CLoader::Create_PrototypeObject(_uint iObjectType, const _wstring& strPrototypeName, const _wstring& strLayerName)
{
	CGameObject* pPrototype = nullptr;

	switch ((OBJECT_CHECK_TYPE)iObjectType)
	{
	case Client::KIT:
		pPrototype = CAid_props::Create(m_pDevice, m_pContext);
		pPrototype->Set_LayerName(strLayerName);
		break;
	case Client::DEBRIS:
		pPrototype = CAid_props::Create(m_pDevice, m_pContext);
		pPrototype->Set_LayerName(strLayerName);
		break;
	case Client::SIGN:
		pPrototype = CAid_props::Create(m_pDevice, m_pContext);
		pPrototype->Set_LayerName(strLayerName);
		break;
	case Client::WALL:
		pPrototype = CAid_props::Create(m_pDevice, m_pContext);
		pPrototype->Set_LayerName(strLayerName);
		break;
	case Client::RUSSIAN_SIGN:
		pPrototype = CAid_props::Create(m_pDevice, m_pContext);
		pPrototype->Set_LayerName(strLayerName);
		break;

	case Client::CITY:
		pPrototype = CBuilding::Create(m_pDevice, m_pContext);
		pPrototype->Set_LayerName(strLayerName);
		break;
	case Client::BUILDING:
		pPrototype = CBuilding::Create(m_pDevice, m_pContext);
		pPrototype->Set_LayerName(strLayerName);
		break;
		
	case Client::ETC:
		break;
	case Client::OBJECT_CHECK_TYPE_END:
		break;
	default:
		break;
	}


	m_pGameInstance->Add_Prototype(strPrototypeName, pPrototype);		// ���⼭ ��¥�� �̹� �ִ¾ֵ��̸� �����༭ �� ��������

	return S_OK;
}

HRESULT CLoader::Create_PrototypeAnimObject(const _wstring& strPrototypeName, const _wstring& strLayerName)
{
	CGameObject* pPrototype = nullptr;

	// �ٷ� �����̴��� �ƴ϶� �������غ����ҰŰ���
	pPrototype = CSpider::Create(m_pDevice, m_pContext);
	pPrototype->Set_LayerName(strLayerName);
	
	m_pGameInstance->Add_Prototype(strPrototypeName, pPrototype);

	return S_OK;
}

HRESULT CLoader::Create_PrototypePlayer(const _wstring& strPrototypeName, const _wstring& strLayerName)
{
	CGameObject* pPrototype = nullptr;

	pPrototype = CPlayer::Create(m_pDevice, m_pContext);
	pPrototype->Set_LayerName(strLayerName);

	m_pGameInstance->Add_Prototype(strPrototypeName, pPrototype);


	pPrototype = CBody_Player::Create(m_pDevice, m_pContext);
	pPrototype->Set_LayerName(strLayerName);

	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"), pPrototype);


	pPrototype = CWeapon_Player::Create(m_pDevice, m_pContext);
	pPrototype->Set_LayerName(strLayerName);

	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Player"), pPrototype);


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
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}




HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("����(��) �ε����Դϴ�."));


	lstrcpy(m_szLoadingText, TEXT("������(��) �ε����Դϴ�."));


	lstrcpy(m_szLoadingText, TEXT("��ü������(��) �ε����Դϴ�."));
	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	if (FAILED(Load_Terrain()))
		return E_FAIL;

	if (FAILED(Load_Anim_GameObject()))
		return E_FAIL;

	if (FAILED(Load_NonAnim_GameObject()))
		return E_FAIL;

	if (FAILED(Load_Player()))
		return E_FAIL;
	
	
	
	lstrcpy(m_szLoadingText, TEXT("FSM��(��) �ε����Դϴ�."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Player_FSM"),
		CFsm::Create(m_pDevice, m_pContext, CPlayer::PLAYER_ANIMATIONID::PLAYER_ANIMATION_END))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("RigidBody��(��) �ε����Դϴ�."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Collider��(��) �ε����Դϴ�."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("���̴���(��) �ε����Դϴ�."));
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


	lstrcpy(m_szLoadingText, TEXT("������(��) �ε����Դϴ�."));


	lstrcpy(m_szLoadingText, TEXT("��ü������(��) �ε����Դϴ�."));


	/* For. Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

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
		Safe_Delete_Array(m_pLoadingObjectInfo[i]);

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
