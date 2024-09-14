#pragma once


namespace Engine
{
	typedef struct
	{
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		bool			isWindowsed;
	}ENGINE_DESC;

	typedef struct
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE		eType;
		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHT_DESC;


	typedef struct
	{
		class CTexture* pMaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	typedef struct
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;
		_double			TrackPosition;
	}KEYFRAME;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;

		static const unsigned int	iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOS;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int	iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINT;

	typedef struct   ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	}VTXPOSTEX;


	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCUBETEX;


	typedef struct  ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	}VTXNORTEX;


	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int	iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices; /* 이 정점은 어떤 뼈(최대 4개 뼈의 인덱스)의 상태를 따라가야하는가? */
		XMFLOAT4		vBlendWeights; /* 위에서 정의한 네개의 뼈가 이 정점에게 각각 몇 퍼센트(0.f ~ 1.f)나 영향을 주는지? */

		static const unsigned int	iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMESH;


	typedef struct ENGINE_DLL
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT2		vLifeTime;

		static const unsigned int	iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	}VTXRECTINSTANCE;

	typedef struct ENGINE_DLL
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT2		vLifeTime;

		static const unsigned int	iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	}VTXPOINTINSTANCE;
}