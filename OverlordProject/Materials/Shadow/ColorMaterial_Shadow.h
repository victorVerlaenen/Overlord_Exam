
#pragma once
class ColorMaterial_Shadow final : public Material<ColorMaterial_Shadow>
{
public:
	ColorMaterial_Shadow();
	~ColorMaterial_Shadow() override = default;

	ColorMaterial_Shadow(const ColorMaterial_Shadow& other) = delete;
	ColorMaterial_Shadow(ColorMaterial_Shadow&& other) noexcept = delete;
	ColorMaterial_Shadow& operator=(const ColorMaterial_Shadow& other) = delete;
	ColorMaterial_Shadow& operator=(ColorMaterial_Shadow&& other) noexcept = delete;

	void SetColor(const XMFLOAT4& color);

protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const override;
	
};

