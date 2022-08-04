#pragma once

class CharacterHUD : public GameObject
{
public:
	CharacterHUD();
	~CharacterHUD() override = default;

	CharacterHUD(const CharacterHUD& other) = delete;
	CharacterHUD(CharacterHUD&& other) noexcept = delete;
	CharacterHUD& operator=(const CharacterHUD& other) = delete;
	CharacterHUD& operator=(CharacterHUD&& other) noexcept = delete;

	void NextPlayerActivate();
	void PreviousPlayerActivate();

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	std::vector<XMFLOAT3> m_PlayerSelectLocations{};
	GameObject* m_pPlayerSelectCircle{};
	int m_SelectedPlayer{ 0 };
};

