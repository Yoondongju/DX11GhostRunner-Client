#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* �� = �޽� + �޽� + �޽� .. */
/* �޽÷� �����ϴ� ���� : ������ ��ü�� �����ϰ� ������ֱ� ���ؼ�.*/
/* �޽� = �������� + �ε��� */

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
public:
	typedef struct
	{
		_uint		iMaterialIndex = 0;
		_uint		iNumVertices = 0;
		_uint		iVertexStride = 0;

		VTXMESH*		pVertices = nullptr;
		VTXANIMMESH*    pAnimVertices = nullptr;
		_uint*			pIndices = nullptr;

		_uint		iNumIndices = 0;
		_uint		iIndexStride = 0;
		_uint		eIndexFormat = {};
		_uint		eTopology = {};

		_float3		vMinPos = {};
		_float3		vMaxPos = {};

		//  �Ʒ����� Anim Mesh�� ���� ����

		_char		 pName[MAX_PATH] = { "" };
		_uint		 iNumAffectBones = {};

		vector<_uint> vecIndices;
		vector<_float4x4> vecOffsetMatrix;
	}MESH_DESC;


private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;	



public:
	VTXMESH* Get_Vertices() { return m_pVertices; }
	_uint* Get_Indices() { return m_pIndices; }


public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }


public:
	const _float3& Get_MinPos() const { return m_vMinPos; }
	const _float3& Get_MaxPos() const { return m_vMaxPos; }




public:
	virtual HRESULT Initialize_Prototype(CModel* pModel, CModel::TYPE eModelType,_fmatrix PreTransformMatrix , void* pArg);
	virtual HRESULT Initialize(void* pArg) override;


public:
	HRESULT Bind_BoneMatrices(const CModel* pModel, class CShader* pShader, const _char* pConstantName);


private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iMaterialIndex = { 0 };


	_uint				m_iNumBones = { 0 };		// ���� ��ü�� ������ �ƴ� !!!!! �� �޽��� ������ �ִ� ���� ���� !!!!!!!

	_uint				m_iNumBoneWeight = {};		// �ش� ���� ��� �������� ������ �ִ� ?


													// m_iNumBones�� ���Ǫ�� ���� �޾ƿ��� �����ϱ� ���ͷ� ���� ���Ŷ� ũ�� �±�����
	vector<_uint>		m_BoneIndices;				// �� ���� ��� �մ� ��ü���߿��� �� �޽ð� ������ �޴� ���� �̸��� ���� ���� �ε���   // ���⼭ �̸��� ���� ���̶� ���� ��ü���� �մ³༮��

	_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};

	vector<_float4x4>	m_OffsetMatrices;

	_float3				m_vMinPos = {};
	_float3				m_vMaxPos = {};


private:
	VTXMESH* m_pVertices = { nullptr }; // �����س�����	 ( NonAnim�ֵ鸸 �����Ұ��� ������Shape���鶧 �ʿ��ؼ�)
	_uint*	 m_pIndices = { nullptr };  // �����س�����




private:
	HRESULT		Ready_VertexBuffer_NonAnim(void* pArg , _fmatrix PreTransformMatrix);
	HRESULT		Ready_VertexBuffer_Anim(void* pArg , const CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , CModel* pModel ,CModel::TYPE eModelType , _fmatrix PreTransformMatrix , void* pArg);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END