#pragma once
class Skybox : public GameObject
{
public:
	Skybox();
	~Skybox() override = default;

	Skybox(const Skybox & other) = delete;
	Skybox(Skybox && other) noexcept = delete;
	Skybox& operator=(const Skybox & other) = delete;
	Skybox& operator=(Skybox && other) noexcept = delete;

protected:
	void Initialize(const SceneContext & sceneContext) override;
};

