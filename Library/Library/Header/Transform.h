#pragma once
#include "MyMath.h"
#include "Component.h"
class ExistObject;

class Transform : public Component
{
public:
	Transform() = default;
	~Transform() override = default;
	// transform�̃f�[�^����
	Transform(const Transform& transform) noexcept : Component(transform)
	{ data = transform.data; beforeData = transform.beforeData; }
	Transform(Transform&&) noexcept = default;

	void DebugUI() override;
	void Initialize();
	void Initialize(std::shared_ptr<ExistObject> object) override;

	// �f�[�^�̃Z�b�g
	void SetPosition(const Vector3 setPosition);
	void SetPosition(const float* setPosition); // ImGui��float[3]�����g���Ȃ�����
	void SetPosition(const float x, const float y, const float z);

	void SetScale(const Vector3 setScale);
	void SetScale(const float* setScale); // ImGui��float[3]�����g���Ȃ�����
	void SetScale(const float x, const float y, const float z);

	void SetModelSize(const Vector3 setModelSize);

	void SetEulerAngles(const Vector3 setAngles);
	void SetEulerAngles(const float* setAngles); // ImGui��float[3]�����g���Ȃ�����
	void SetEulerAngles(const float x, const float y, const float z);

	void SetRotation(const Quaternion quaternion);

	// ���݂̃f�[�^�̎擾
	Vector3 GetPosition() const noexcept;
	Vector3 GetScale() const noexcept;
	Vector3 GetModelSize() const noexcept;
	Vector3 GetEulerAngles() const noexcept;
	Vector3 GetRight() const noexcept;
	Vector3 GetUp() const noexcept;
	Vector3 GetForward() const noexcept;
	Quaternion GetRotation() const noexcept;
	Matrix GetWorldMatrix() const noexcept;

	// �X�V�O�̃f�[�^�̎擾
	Vector3 GetBeforePosition() const noexcept;
	Vector3 GetBeforeScale() const noexcept;
	Vector3 GetBeforeModelSize() const noexcept;
	Vector3 GetBeforeEulerAngles() const noexcept;
	Vector3 GetBeforeRight() const noexcept;
	Vector3 GetBeforeUp() const noexcept;
	Vector3 GetBeforeForward() const noexcept;
	Quaternion GetBeforeRotation() const noexcept;
	Matrix GetBeforeWorldMatrix() const noexcept;

	Vector3 GetDifferencePosition() const noexcept;

private:
	struct Data
	{
		// ���[���h�X�y�[�X��Transform�̈ʒu
		Vector3 position;
		// �I�C���[�p�Ƃ��Ă̊p�x
		Vector3 eulerAngles;
		// �N�I�[�^�j�I��
		Quaternion rotation;
		// ���[���h�X�y�[�X�ɂ�����X���̕����A��]�ɉe�������
		Vector3 right;
		// ���[���h�X�y�[�X�ɂ�����Y���̕����A��]�ɉe�������
		Vector3 up;
		// ���[���h�X�y�[�X�ɂ�����Z���̕����A��]�ɉe�������
		Vector3 forward;
		// �X�P�[��
		Vector3 scale;
		//�@���f���T�C�Y
		Vector3 modelSize = { 1,1,1 };

		Matrix worldMatrix = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		Matrix translationMatrix = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		Matrix rotationMatrix = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		Matrix scaleMatrix = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	};
	// ���݂̃f�[�^
	Data data;
	// �X�V�O�̃f�[�^
	Data beforeData;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive);

};

CEREAL_REGISTER_TYPE(Transform)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Transform)

template<class Archive>
void Transform::serialize(Archive& archive)
{
	archive(cereal::base_class<Component>(this),
		data.position, data.eulerAngles, data.rotation, data.right, data.up, data.forward, data.scale,
		data.worldMatrix, data.translationMatrix, data.rotationMatrix, data.scaleMatrix);
}