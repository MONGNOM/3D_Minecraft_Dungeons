#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody_Skeleton : public CPartObject
{
public:
	typedef struct CBody_SkeletoionDesc : CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = nullptr;
	}CBODY_SKELETONDESC;

private:
	CBody_Skeleton(ID3D11Device* pDevice, ID3D11DeviceContext* Context);
	CBody_Skeleton(const CBody_Skeleton& Prototype);
	virtual ~CBody_Skeleton() = default;

public:
	const _float4x4* Get_SocketBoneMatrixPtr(const _char* pBoneName);
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	const _uint* m_pParentState = { nullptr };



private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	_bool Intersect_ToPlayer();


public:
	static CBody_Skeleton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END