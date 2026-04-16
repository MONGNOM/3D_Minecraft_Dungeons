#include "HpBar.h"
#include "GameInstance.h"
#include "Skeleton.h"

CHpBar::CHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{pDevice, pContext}, m_pOwner(nullptr)
{
}

CHpBar::CHpBar(const CHpBar& Prototype)
    :CUIObject(Prototype), m_pOwner(Prototype.m_pOwner)
{
}

HRESULT CHpBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHpBar::Initialize(void* pArg)
{
    HPBAR_DESC* pDesc = static_cast<HPBAR_DESC*>(pArg);
    m_iNumTexture = pDesc->iNumTexture;
    m_fSizeX = pDesc->fSizeX;
    m_fOriginalSizeX = pDesc->fSizeX;
    m_fSizeY = pDesc->fSizeY;
    m_fPos = pDesc->pos;
    m_pOwner = pDesc->owner;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
   
    return S_OK;
}

void CHpBar::Priority_Update(_float fTimeDelta)
{
}

void CHpBar::Update(_float fTimeDelta)
{
    //이거 삭제 판정을 어떻게 해줘야할까

    __super::Update_Transform();

    _vector vSkeletonPos = dynamic_cast<CTransform*>(m_pOwner->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION);

    // 머리 위로 올리기 위해 Y축(위쪽)으로 살짝 더해줌
    vSkeletonPos = XMVectorSetY(vSkeletonPos, XMVectorGetY(vSkeletonPos) + 2.2f);

    // 2. 알아낸 '진짜' 3D 좌표를 투영 함수에 넣습니다.
    _vector vScreenPos = XMVector3Project(
        vSkeletonPos, // 이제 0,0,0이 아니라 스켈레톤의 좌표가 들어갑니다!
        0.f, 0.f, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f,
        XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::PROJ)),
        XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::VIEW)),
        XMMatrixIdentity()
    );

    // 3. 투영된 픽셀 좌표를 직교 좌표로 변환
    _float3 BarPos;
    XMStoreFloat3(&BarPos, vScreenPos);

    fOrthoX =  BarPos.x - (g_iWinSizeX * 0.5f);
    fOrthoY = -BarPos.y + (g_iWinSizeY * 0.5f);
    
    CSkeleton* pSkeleton = dynamic_cast<CSkeleton*>(m_pOwner);
    if (*pSkeleton->Get_TakeHit())
    {
        m_fSizeX = m_fOriginalSizeX * m_pOwner->Get_HpRatio();
        pSkeleton->Set_TakeHit(false);
    }

    _float fLostWidth = m_fOriginalSizeX - m_fSizeX;
    _float fOffsetX = fLostWidth * 0.5f;
    m_fX = fOrthoX - fOffsetX;
    m_fY = fOrthoY;
    m_pTransformCom->SetUp_Scale(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX, m_fY, BarPos.z, 1.f));
}

void CHpBar::Late_Update(_float fTimeDelta)
{
    

    m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this);
}

HRESULT CHpBar::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_iNumTexture)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHpBar::Ready_Components()
{
    if (FAILED(__super::Add_Component(m_eSceneType, TEXT("Prototype_Component_Texture_HpBar"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CHpBar* CHpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHpBar* pInstance = new CHpBar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHpBar");
        Safe_Release(pInstance);
    }
    return pInstance;
}


CGameObject* CHpBar::Clone(void* pArg)
{
    CHpBar* pInstance = new CHpBar(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHpBar");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CHpBar::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
