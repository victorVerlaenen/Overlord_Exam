#pragma once
class SpikyMaterial final : public Material<SpikyMaterial>
{
public:
	SpikyMaterial();
	~SpikyMaterial() override = default;

	SpikyMaterial(const SpikyMaterial& other) = delete;
	SpikyMaterial(SpikyMaterial&& other) noexcept = delete;
	SpikyMaterial& operator=(const SpikyMaterial& other) = delete;
	SpikyMaterial& operator=(SpikyMaterial&& other) noexcept = delete;

	//void SetDiffuseTexture(const std::wstring& assetFile);
	//void SetNormalTexture(const std::wstring& assetFile);
	//void SetSpecularTexture(const std::wstring& assetFile);
	//void SetEnvironmentTexture(const std::wstring& assetFile);
	//void UseTransparency(bool enabled);

protected:
	void InitializeEffectVariables() override;
private:
	//TextureData* m_pDiffuseTexture{};
	//TextureData* m_pNormalTexture{};
	//TextureData* m_pSpecularTexture{};
	//TextureData* m_pEnvironmentTexture{};
};