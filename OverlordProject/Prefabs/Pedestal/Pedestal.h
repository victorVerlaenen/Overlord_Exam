#pragma once

class Player;

class Pedestal : public GameObject
{
public:
	Pedestal(const XMFLOAT2& healthPosition, bool isTeamOne);
	~Pedestal() override = default;

	Pedestal(const Pedestal& other) = delete;
	Pedestal(Pedestal&& other) noexcept = delete;
	Pedestal& operator=(const Pedestal& other) = delete;
	Pedestal& operator=(Pedestal&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext&) override;

private:
	void Score(GameObject* pPlayerObject, PxTriggerAction triggerAction);

	int m_Health{ 100 };
	SpriteFont* m_pFont{};
	std::wstring m_Text{};
	XMFLOAT2 m_HealthPosition{};

	bool m_IsTeamOne{};
};