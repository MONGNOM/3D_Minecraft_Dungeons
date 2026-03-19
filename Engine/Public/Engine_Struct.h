#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HINSTANCE	hInstance;
		HWND		hWnd;
		WINMODE		eWinMode;
		unsigned int iNumLevels;
		unsigned int iViewportWidth;
		unsigned int iViewportHeight;			
	}ENGINE_DESC;

	typedef struct tagKeyFrame
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;

		float		fTrackPosition; /* 현재 키프레임상태는 어떤 위치에서 표현되어야하는가? */

	}KEYFRAME;

	typedef struct tagLightDesc
	{
		LIGHT		eType;

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

	}LIGHT_DESC;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexcoord;

		static const unsigned int		iNumElements = { 2 };

		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
	}VTXTEX;

	typedef struct tagVertexPositionNormalTexcoord
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexcoord;

		static const unsigned int		iNumElements = { 3 };

		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
	}VTXNORTEX;


	typedef struct tagVertexMesh
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT3			vTangent;
		XMFLOAT2			vTexcoord;		

		static const unsigned int		iNumElements = { 4 };

		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
	}VTXMESH;

	typedef struct tagVertexAnimationMesh
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT3			vTangent;
		XMFLOAT2			vTexcoord;

		XMUINT4				vBlendIndex; /* 이 정점이 이용해야할 뼈들의 인덱스 들 */
		XMFLOAT4			vBlendWeight; /* 위에서 지정된 뼈들의 상태를 각각 몇 퍼센트	(0.f ~ 1.f)나 이용하면 되는지 */

		static const unsigned int		iNumElements = { 6 };

		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA,0 }, 
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
	}VTXANIMMESH;




}


#endif // Engine_Struct_h__
