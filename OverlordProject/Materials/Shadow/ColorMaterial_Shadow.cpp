#include "stdafx.h"
#include "ColorMaterial_Shadow.h"

ColorMaterial_Shadow::ColorMaterial_Shadow() :
	Material(L"Effects/Shadow/Color_Shadow.fx")
{
	
}

void ColorMaterial_Shadow::SetColor(const XMFLOAT4& color)
{
	SetVariable_Vector(L"gColor", color);
}

void ColorMaterial_Shadow::InitializeEffectVariables()
{
}

void ColorMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	const auto pShadowMapRenderer = ShadowMapRenderer::Get();
	//Update The Shader Variables
	//1. Update the LightWVP > Used to Transform a vertex into Light clipping space
		//LightWVP = model_world * light_viewprojection
		//(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	const auto modelWorld = XMLoadFloat4x4(&pModel->GetTransform()->GetWorld());
	const auto lightViewProjection = XMLoadFloat4x4(&pShadowMapRenderer->GetLightVP());
	const auto lightWVP = modelWorld * lightViewProjection;
	XMFLOAT4X4 loadedLightWVP{};
	XMStoreFloat4x4(&loadedLightWVP, lightWVP);
	SetVariable_Matrix(L"gWorldViewProj_Light", loadedLightWVP);
	//2. Update the ShadowMap texture
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());
	//3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	const auto lightDirection = XMFLOAT3{ &sceneContext.pLights->GetDirectionalLight().direction.x };
	SetVariable_Vector(L"gLightDirection", lightDirection);
}
