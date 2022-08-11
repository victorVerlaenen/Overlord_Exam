#pragma once


class Aura : public GameObject
{
public:
	Aura(const XMFLOAT3& position, float radius);
	~Aura() override = default;

	Aura(const Aura& other) = delete;
	Aura(Aura&& other) noexcept = delete;
	Aura& operator=(const Aura& other) = delete;
	Aura& operator=(Aura&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	void KillPlayer(GameObject* pPlayerObject, PxTriggerAction triggerAction);

	RigidBodyComponent* m_pRigidBody{};
	GameObject* m_pColliderObject{};
	XMFLOAT3 m_Position{};
	float m_Radius{};
};