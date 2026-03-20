#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumSRVs);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_NumShaderResource(_uint num) { m_iNumSRVs = num; }
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const _char* pConstantName, _uint iIndex);
	void Set_Texture(vector<ID3D11ShaderResourceView*> texture) { // 1. 기존에 내가 입고 있던 옷(텍스쳐)이 있다면, 반드시 정중하게 반납(Release)합니다.
		// (이걸 안 하면 예전 텍스쳐가 영원히 누수됩니다!)
		for (auto& pSRV : m_SRVs)
		{
			Safe_Release(pSRV);
		}
		m_SRVs.clear();

		// 2. 갤러리에서 전달받은 새로운 옷(images)으로 갈아입습니다.
		m_SRVs = texture;

		// 3.  가장 중요한 핵심! "나도 이 옷을 공유해서 입는다!" 라고 다이렉트X에 신고(AddRef)합니다.
		// (이걸 안 하면 방금 겪으신 이중 해제 크래시가 납니다!)
		for (auto& pSRV : m_SRVs)
		{
			Safe_AddRef(pSRV);
		}
	}
	vector<ID3D11ShaderResourceView*> Get_Texture() { return m_SRVs; }

private:
	_uint										m_iNumSRVs = {};
	vector<ID3D11ShaderResourceView*>			m_SRVs;
public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumSRVs);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END