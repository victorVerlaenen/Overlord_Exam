#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter) :
	m_pMeshFilter{ pMeshFilter }
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		passedTicks = fmod(passedTicks, m_CurrentClip.duration);

		//2. 
		//IF m_Reversed is true
		if (m_Reversed == true)
		{
			//	Subtract passedTicks from m_TickCount
			m_TickCount -= passedTicks;
			//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
			if (m_TickCount < 0)
			{
				m_TickCount += m_CurrentClip.duration;
			}
		}
		//ELSE
		else
		{
			//	Add passedTicks to m_TickCount
			m_TickCount += passedTicks;
			//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
			if (m_TickCount > m_CurrentClip.duration)
			{
				m_TickCount -= m_CurrentClip.duration;
			}
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB{m_CurrentClip.duration};
		//Iterate all the keys of the clip and find the following keys:
		for (const auto key : m_CurrentClip.keys)
		{
			//keyA > Closest Key with Tick before/smaller than m_TickCount
			if (keyA.boneTransforms.size() == 0.f ||
				(key.tick < m_TickCount && key.tick >= keyA.tick))
			{
				keyA = key;
			}
			//keyB > Closest Key with Tick after/bigger than m_TickCount
			if ((key.tick > m_TickCount &&
				key.tick <= keyB.tick &&
				key.tick > keyA.tick))
			{
				keyB = key;
			}
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		float timeBetweenKeys = keyB.tick - keyA.tick;
		float blendFactor = (m_TickCount - keyA.tick) / timeBetweenKeys;

		//Clear the m_Transforms vector
		//m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		for (int i{ 0 }; i < m_pMeshFilter->m_BoneCount; ++i)
		{
			//	Retrieve the transform from keyA (transformA)
			auto transformA = XMLoadFloat4x4(&keyA.boneTransforms[i]);
			// 	Retrieve the transform from keyB (transformB)
			auto transformB = XMLoadFloat4x4(&keyB.boneTransforms[i]);
			//	Decompose both transforms
			XMVECTOR positionA, scaleA, rotationA;
			XMVECTOR positionB, scaleB, rotationB;
			//	Lerp between all the transformations (Position, Scale, Rotation)
			XMMatrixDecompose(&scaleA, &rotationA, &positionA, transformA);
			XMMatrixDecompose(&scaleB, &rotationB, &positionB, transformB);
			//	Compose a transformation matrix with the lerp-results
			auto scaleMatrix = XMMatrixScalingFromVector(XMVectorLerp(scaleA, scaleB, blendFactor));
			auto rotationMatrix = XMMatrixRotationQuaternion(XMQuaternionSlerp(rotationA, rotationB, blendFactor));
			auto translationMatrix = XMMatrixTranslationFromVector(XMVectorLerp(positionA, positionB, blendFactor));
			auto lerpedMatrix = rotationMatrix * scaleMatrix * translationMatrix;

			//	Add the resulting matrix to the m_Transforms vector
			XMStoreFloat4x4(&m_Transforms[i], lerpedMatrix);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	AnimationClip clip{};
	for (auto animationClip : m_pMeshFilter->m_AnimationClips)
	{
		if (clipName == animationClip.name)
		{
			clip = animationClip;
			break;
		}
	}
	//If found,
	if (clip.name == clipName)
	{
		//	Call SetAnimation(Animation Clip) with the found clip
		SetAnimation(clip);
	}
	//Else
	else
	{
		//	Call Reset
		Reset();
		//	Log a warning with an appropriate message
		Logger::LogError(L"Current clip {} could not be found", clipName);
	}
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	//If not,
	if (clipNumber >= m_pMeshFilter->m_AnimationClips.size())
	{
		//	Call Reset
		Reset();
		//	Log a warning with an appropriate message
		Logger::LogWarning(L"ClipNumber is higher than clip amount");
		//	return
		return;
	}
	//else
	else
	{
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
	}
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;

	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	if (pause == true)
	{
		m_IsPlaying = false;
	}

	//Set m_TickCount to zero
	m_TickCount = 0.f;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.f;

	//If m_ClipSet is true
	if (m_ClipSet == true)
	{
		//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		auto boneTransform = m_CurrentClip.keys[0].boneTransforms;
		//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		m_Transforms.assign(boneTransform.begin(), boneTransform.begin() + m_pMeshFilter->m_BoneCount);
	}
	//Else
	else
	{
		//	Create an IdentityMatrix 
		XMFLOAT4X4 identity{};
		XMStoreFloat4x4(&identity, XMMatrixIdentity());

		//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identity);
	}
}
