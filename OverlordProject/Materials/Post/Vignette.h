#pragma once

class Vignette : public PostProcessingMaterial
{
public:
	Vignette();
	~Vignette() override = default;
	Vignette(const Vignette& other) = delete;
	Vignette(Vignette&& other) noexcept = delete;
	Vignette& operator=(const Vignette& other) = delete;
	Vignette& operator=(Vignette&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {}

};

