#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
public:
	typedef struct INIT_BONE_DESC
	{
		_int		iParentBoneIndex = {};
		_uint       iNumChildBone = { }; // 자식이 몇개니 ?

		_char		szName[MAX_PATH] = { "" };
		_float4x4   TransformationMatrix = {};
		_float4x4   CombinedTransformaionMatrix = {};

		vector<INIT_BONE_DESC> vecChildBones; // 자식 뼈들에 대한 정보

	}INIT_BONE_DESC;


private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationMatrix_Ptr() const {
		return &m_CombinedTransformationMatrix;
	}

	_matrix Get_TransformationMatrix() const {
		return  XMLoadFloat4x4(&m_TransformationMatrix);
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	_int	Get_ParentBoneIndex() { return m_iParentBoneIndex; }


	void	Set_NumChildBone(_uint iNum) { m_iNumChildBone = iNum; }
	_uint	Get_NumChildBone() { return m_iNumChildBone; }


public:
	HRESULT Initialize(_uint iNumChildBone, _char* pName , _float4x4 TransformationMarix, _int iParentBoneIndex);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);


private:
	_char				m_szName[MAX_PATH] = {};

	/* 부모기준으로 표현된 이 뼈만의 상태행렬. */
	_float4x4			m_TransformationMatrix = {};

	/* 이 뼈의 최종적인 상태행렬 ( 내 상태행렬 * 부모의 최종상태행렬 ) */
	/* m_TransformationMatrix * Parent`s m_CombinedTransformationMatrix */
	_float4x4			m_CombinedTransformationMatrix = {};
	_int				m_iParentBoneIndex = { -1 };			// 내 부모의 뼈 인덱스

	_uint				m_iNumChildBone = {};




public:
	static CBone* Create(_uint iNumChildBone, _char* pName , _float4x4 TransformationMarix  ,_int iParentBoneIndex);
	CBone* Clone();
	virtual void Free() override;
};

END