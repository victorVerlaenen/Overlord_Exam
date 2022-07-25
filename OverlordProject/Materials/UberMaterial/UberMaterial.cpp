#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial() :
	Material<UberMaterial>(L"Effects/UberShader.fx")
{}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
}

void UberMaterial::SetNormalTexture(const std::wstring& assetFile)
{
	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
}

void UberMaterial::SetSpecularTexture(const std::wstring& assetFile)
{
	m_pSpecularTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureSpecularIntensity", m_pSpecularTexture);
}

void UberMaterial::SetEnvironmentTexture(const std::wstring& assetFile)
{
	m_pEnvironmentTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gCubeEnvironment", m_pEnvironmentTexture);
}

void UberMaterial::InitializeEffectVariables()
{
	//UseTransparency(false);
	SetVariable_Scalar(L"gUseTextureDiffuse", true);
	SetVariable_Scalar(L"gFlipGreenChannel", false);
	SetVariable_Scalar(L"gUseTextureNormal", true);
	SetVariable_Scalar(L"gUseEnvironmentMapping", false);
	SetVariable_Scalar(L"gUseTextureSpecularIntensity", true);
	SetVariable_Scalar(L"gUseSpecularPhong", true);
	SetVariable_Scalar(L"gReflectionStrength", 0.4f);
	SetVariable_Scalar(L"gRefractionStrength", 0.2f);
	SetVariable_Scalar(L"gRefractionIndex", 0.3f);
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Scalar(L"gFresnelPower", 2.0f);
	SetVariable_Scalar(L"gFresnelMultiplier", 1.5f);
	SetVariable_Scalar(L"gFresnelHardness", 2.0f);
	SetVariable_Scalar(L"gShininess", 15);
	SetVariable_Vector(L"gColorDiffuse", XMFLOAT4{ 1.f,1.f,1.f,1.f });
	SetVariable_Vector(L"gColorSpecular", XMFLOAT4{ .15f,.36f,.7f,1.f });
	SetVariable_Vector(L"gColorFresnel", XMFLOAT4{ 1.f,1.f,1.f,1.f });
	SetVariable_Vector(L"gLightDirection", XMFLOAT3{ -0.577f, -0.577f, 0.577f });
}

void UberMaterial::UseTransparency(bool enabled)
{
	SetTechnique(enabled ? L"TransparencyTech" : L"Default");
}
//
//void UberMaterial::SetColor(const XMFLOAT4& color) const
//{
//	SetVariable_Vector(L"gColor", color);
//}
