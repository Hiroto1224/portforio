#pragma once
#include <memory>
#include <DirectXMath.h>
#include <cereal/cereal.hpp>

#include <cereal/access.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
struct Vector2;
struct Vector3;
struct Vector4;
struct Matrix;
struct Quaternion;

//--------------------------------------
// Vector2
//--------------------------------------
struct Vector2 : public DirectX::XMFLOAT2
{
	Vector2() noexcept : DirectX::XMFLOAT2(0.0f, 0.0f) {}
	constexpr explicit Vector2(float _x) : DirectX::XMFLOAT2(_x, _x) {}
	constexpr Vector2(float _x, float _y) : DirectX::XMFLOAT2(_x, _y) {}
	explicit Vector2(_In_reads_(2) const float* pArray) : DirectX::XMFLOAT2(pArray) {}
	Vector2(DirectX::FXMVECTOR _v) { DirectX::XMStoreFloat2(this, _v); }
	Vector2(const DirectX::XMFLOAT2& _v) { this->x = _v.x; this->y = _v.y; }
	explicit Vector2(const DirectX::XMVECTORF32& _f) { this->x = _f.f[0]; this->y = _f[1]; }

	Vector2(const Vector2&) = default;
	Vector2& operator=(const Vector2&) = default;

	Vector2(Vector2&&) = default;
	Vector2& operator=(Vector2&&) = default;

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat2(this); }

	bool operator == (const Vector2& _v) const;
	bool operator != (const Vector2& _v) const;

	Vector2& operator= (const DirectX::XMVECTORF32& _f) { x = _f.f[0]; y = _f.f[1]; return *this; }
	Vector2& operator+= (const Vector2& _v);
	Vector2& operator-= (const Vector2& _v);
	Vector2& operator*= (const Vector2& _v);
	Vector2& operator*= (float _s);
	Vector2& operator/= (float _s);

	Vector2 operator+ () const { return *this; }
	Vector2 operator- () const { return Vector2(-x, -y); }

	float Length() const;
	float LengthSq() const;

	float Dot(const Vector2& _v) const;
	Vector2 Cross(const Vector2& _v) const;

	Vector2 Normalize();
	
	Vector2 Clamp(const Vector2& _min, const Vector2 _max);

	static float Distance(const Vector2& _v1, const Vector2& _v2);
	static float DistanceSq(const Vector2& _v1, const Vector2& _v2);

	static Vector2 Min(const Vector2& _v1, const Vector2& _v2);
	static Vector2 Max(const Vector2& _v1, const Vector2& _v2);

	static Vector2 Lerp(const Vector2& _v1, const Vector2& _v2, float _t);

	static Vector2 SmoothStep(const Vector2& _v1, const Vector2& _v2, float _t);
	
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(x), CEREAL_NVP(y));
	}

};

Vector2 operator+ (const Vector2& _v1, const Vector2& _v2);
Vector2 operator- (const Vector2& _v1, const Vector2& _v2);
Vector2 operator* (const Vector2& _v1, const Vector2& _v2);
Vector2 operator* (const Vector2& _v, float _s);
Vector2 operator* (float _s, const Vector2& _v);
Vector2 operator/ (const Vector2& _v1, const Vector2& _v2);
Vector2 operator/ (const Vector2& _v, float _s);

//--------------------------------------
// Vector3
//--------------------------------------
struct Vector3 : public DirectX::XMFLOAT3
{
	Vector3() noexcept : DirectX::XMFLOAT3(0.0f, 0.0f,0.0f) {}
	constexpr explicit Vector3(float _x) : DirectX::XMFLOAT3(_x, _x,_x) {}
	constexpr Vector3(float _x, float _y, float _z) : DirectX::XMFLOAT3(_x, _y, _z) {}
	explicit Vector3(_In_reads_(3) const float* pArray) : DirectX::XMFLOAT3(pArray) {}
	Vector3(DirectX::FXMVECTOR _v) { DirectX::XMStoreFloat3(this, _v); }
	Vector3(const DirectX::XMFLOAT3& _v) { this->x = _v.x; this->y = _v.y; this->z = _v.z; }
	explicit Vector3(const DirectX::XMVECTORF32& _f) { this->x = _f.f[0]; this->y = _f[1]; this->z = _f.f[2]; }

	Vector3(const Vector3&) = default;
	Vector3& operator=(const Vector3&) = default;

	Vector3(Vector3&&) = default;
	Vector3& operator=(Vector3&&) = default;

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat3(this); }

	bool operator == (const Vector3& _v) const;
	bool operator != (const Vector3& _v) const;

	Vector3& operator= (const DirectX::XMVECTORF32& _f) { x = _f.f[0]; y = _f.f[1]; z = _f.f[2]; return *this; }
	Vector3& operator+= (const Vector3& _v);
	Vector3& operator-= (const Vector3& _v);
	Vector3& operator*= (const Vector3& _v);
	Vector3& operator*= (float _s);
	Vector3& operator/= (float _s);

	Vector3 operator+ () const { return *this; }
	Vector3 operator- () const;

	float Length() const;
	float LengthSq() const;

	Vector3 Dot(const Vector3& v);
	Vector3 Cross(const Vector3& v);

	Vector3 Normalize();

	Vector3 Clamp(const Vector3& min, const Vector3 max);

	static float Distance(const Vector3& v1, const Vector3& v2);
	static float DistanceSq(const Vector3& v1, const Vector3& v2);

	static Vector3 Min(const Vector3& v1, const Vector3& v2);
	static Vector3 Max(const Vector3& v1, const Vector3& v2);

	static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float time);

	static Vector3 SmoothStep(const Vector3& v1, const Vector3& v2, float time);

	static Vector3 Transform(const Vector3& v1, const Matrix& v2);

	static Vector3 Dot(const Vector3& v1, const Vector3& v2);

	static Vector3 Cross(const Vector3& v1, const Vector3& v2);

	static Vector3 Normalize(const Vector3& v);

	static Vector3 Foward() { return Vector3(0, 0, 1); }
	static Vector3 Right() { return Vector3(1, 0, 0); }
	static Vector3 Up() { return Vector3(0, 1, 0); }

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z));
	}
};


Vector3 operator+ (const Vector3& _v1, const Vector3& _v2);
Vector3 operator- (const Vector3& _v1, const Vector3& _v2);
Vector3 operator* (const Vector3& _v1, const Vector3& _v2);
Vector3 operator* (const Vector3& v, float s);
Vector3 operator* (float s, const Vector3& v1);
Vector3 operator/ (const Vector3& _v1, const Vector3& _v2);
Vector3 operator/ (const Vector3& v, float s);



//--------------------------------------
// Vector4
//--------------------------------------
struct Vector4 : public DirectX::XMFLOAT4
{
	Vector4() noexcept : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f,0.0f) {}
	constexpr explicit Vector4(float _x) : DirectX::XMFLOAT4(_x, _x, _x,_x) {}
	constexpr Vector4(float _x, float _y, float _z, float _w) : DirectX::XMFLOAT4(_x, _y, _z, _w) {}
	explicit Vector4(_In_reads_(4) const float* pArray) : DirectX::XMFLOAT4(pArray) {}
	Vector4(DirectX::FXMVECTOR _v) { DirectX::XMStoreFloat4(this, _v); }
	Vector4(const DirectX::XMFLOAT4& _v) { this->x = _v.x; this->y = _v.y; this->z = _v.z; this->w = _v.w; }
	explicit Vector4(const DirectX::XMVECTORF32& _f) { this->x = _f.f[0]; this->y = _f[1]; this->z = _f.f[2]; this->w = _f.f[3]; }

	Vector4(const Vector4&) = default;
	Vector4& operator=(const Vector4&) = default;

	Vector4(Vector4&&) = default;
	Vector4& operator=(Vector4&&) = default;

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(this); }

	bool operator == (const Vector4& _v) const;
	bool operator != (const Vector4& _v) const;

	Vector4& operator= (const DirectX::XMVECTORF32& _f) { x = _f.f[0]; y = _f.f[1]; z = _f.f[2]; w = _f.f[3]; return *this; }
	Vector4& operator+= (const Vector4& _v);
	Vector4& operator-= (const Vector4& _v);
	Vector4& operator*= (const Vector4& _v);
	Vector4& operator*= (float _s);
	Vector4& operator/= (float _s);

	Vector4 operator+ () const { return *this; }
	Vector4 operator- () const;

	float Length() const;
	float LengthSq() const;

	float Dot(const Vector4& _v) const;
	Vector4 Cross(const Vector4& _v1, const Vector4& _v2);

	Vector4 Normalize();

	Vector4 Clamp(const Vector4& _min, const Vector4 _max);

	static float Distance(const Vector4& _v1, const Vector4& _v2);
	static float DistanceSq(const Vector4& _v1, const Vector4& _v2);

	static Vector4 Min(const Vector4& _v1, const Vector4& _v2);
	static Vector4 Max(const Vector4& _v1, const Vector4& _v2);

	static Vector4 Lerp(const Vector4& _v1, const Vector4& _v2, float _t);

	static Vector4 SmoothStep(const Vector4& _v1, const Vector4& _v2, float _t);

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z), CEREAL_NVP(w));
	}
};


Vector4 operator+ (const Vector4& _v1, const Vector4& _v2);
Vector4 operator- (const Vector4& _v1, const Vector4& _v2);
Vector4 operator* (const Vector4& _v1, const Vector4& _v2);
Vector4 operator* (const Vector4& v, float s);
Vector4 operator* (float s, const Vector4& v);
Vector4 operator/ (const Vector4& _v1, const Vector4& _v2);
Vector4 operator/ (const Vector4& v, float s);

//--------------------------------------
// Matrix
//--------------------------------------
struct Matrix : public DirectX::XMFLOAT4X4
{
	Matrix() noexcept : DirectX::XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f)
	{
	}

	XM_CONSTEXPR Matrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
		: DirectX::XMFLOAT4X4(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33)
	{
	}

	explicit Matrix(const Vector3& r0, const Vector3& r1, const Vector3& r2)
		: DirectX::XMFLOAT4X4(
			r0.x, r0.y, r0.z, 0.0f,
			r1.x, r1.y, r1.z, 0.0f,
			r2.x, r2.y, r2.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f)
	{
	}

	explicit Matrix(const Vector4& r0, const Vector4& r1, const Vector4& r2,const Vector4& r3)
		: DirectX::XMFLOAT4X4(
			r0.x, r0.y, r0.z, r0.w,
			r1.x, r1.y, r1.z, r1.w,
			r2.x, r2.y, r2.z, r2.w,
			r3.x, r3.y, r3.z, r3.w)
	{
	}

	Matrix(const DirectX::XMFLOAT4X4& _M)
	{
		memcpy_s(this, sizeof(float) * 16, &_M, sizeof(DirectX::XMFLOAT4X4));
	}
	Matrix(const DirectX::XMFLOAT3X3& _M);
	Matrix(const DirectX::XMFLOAT4X3& _M);

	explicit Matrix(_In_reads_(16) const float* pArray) :
		DirectX::XMFLOAT4X4(pArray) {}
	Matrix(DirectX::CXMMATRIX _M) { DirectX::XMStoreFloat4x4(this, _M); }

	Matrix(const Matrix&) = default;
	Matrix& operator=(const Matrix&) = default;

	Matrix(Matrix&&) = default;
	Matrix& operator=(Matrix&&) = default;

	operator DirectX::XMMATRIX() const { return DirectX::XMLoadFloat4x4(this); }

	bool operator == (const Matrix& _M) const;
	bool operator != (const Matrix& _M) const;

	Matrix& operator= (const DirectX::XMFLOAT3X3& _M);
	Matrix& operator= (const DirectX::XMFLOAT4X3& _M);
	Matrix& operator+= (const Matrix& _M);
	Matrix& operator-= (const Matrix& _M);
	Matrix& operator*= (const Matrix& _M);
	Matrix& operator*= (float _s);
	Matrix& operator/= (float _s);
	Matrix& operator/= (const Matrix& _M);

	Matrix operator+ () const { return *this; }
	Matrix operator- () const;

	Vector3 GetUp() const { return Vector3(_21, _22, _23); }

	/// <summary>
	///  _21 = v.x; _22 = v.y; _23 = v.z; 
	/// </summary>
	/// <param name="v"></param>
	void SetUp(const Vector3& v) { _21 = v.x; _22 = v.y; _23 = v.z; }

	Vector3 GetDown() const { return Vector3(-_21, -_22, -_23); }

	/// <summary>
	/// _21 = -v.x; _22 = -v.y; _23 = -v.z;
	/// </summary>
	/// <param name="v"></param>
	void SetDown(const Vector3& v) { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

	Vector3 GetRight() const { return Vector3(_11, _12, _13); }

	/// <summary>
	/// _11 = v.x , _12 = v.y , _13 = v.z
	/// </summary>
	/// <param name="v"></param>
	void SetRight(const Vector3& v) { _11 = v.x; _12 = v.y; _13 = v.z; }
	
	Vector3 GetLeft() const { return Vector3(-_11, -_12, -_13); }

	/// <summary>
	/// _11 = -v.x , _12 = -v.y , _13 = -v.z
	/// </summary>
	/// <param name="v"></param>
	void SetLeft(const Vector3& v) { _11 = -v.x; _12 = -v.y; _13 = -v.z; }
	
	Vector3 GetForward() const { return Vector3(-_31, -_32, -_33); }

	/// <summary>
	/// _31 = -v.x; _32 = -v.y; _33 = -v.z;
	/// </summary>
	/// <param name="v"></param>
	void SetForward(const Vector3& v) { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

	Vector3 GetBackWard() const { return Vector3(_31, _32, _33); }

	/// <summary>
	/// _31 = v.x; _32 = v.y; _33 = v.z;
	/// </summary>
	/// <param name="_v"></param>
	void SetBackWard(const Vector3& v) { _31 = v.x; _32 = v.y; _33 = v.z; }

	Vector3 GetTranslation() const { return Vector3(_41, _42, _43); }

	/// <summary>
	/// _41 = v.x; _42 = v.y; _43 = v.z;
	/// </summary>
	/// <param name="v"></param>
	void SetTranslation(const Vector3& v) { _41 = v.x; _42 = v.y; _43 = v.z; }

	bool Decompose(Vector3& _scale, Quaternion& _rotation, Vector3& _translation);

	// Get Transpose Matrix
	Matrix Transpose() const;
	
	// Get Inverse Matrix
	Matrix Invert() const;

	float Determinant() const;

	static Matrix CreateBillboard(const Vector3& _obj, const Vector3& _cameraPos,
		const Vector3& _cameraUp, _In_opt_ const Vector3* _cameraForward = nullptr);

	static Matrix CreateConstrainedBillboard(const Vector3& obj, const Vector3& _cameraPos, 
		const Vector3& _rotateAxis,_In_opt_ const Vector3* _cameraForward = nullptr,
		_In_opt_ const Vector3* _objForward = nullptr);

	static Matrix CreateTranslation(const Vector3& _pos);
	static Matrix CreateTranslation(float _x, float _y, float _z);

	static Matrix CreateScale(const Vector3& _scale);
	static Matrix CreateScale(float _x, float _y, float _z);
	static Matrix CreateScale(float scale);

	static Matrix CreateRotationX(float _radians);
	static Matrix CreateRotationY(float _radians);
	static Matrix CreateRotationZ(float _radians);

	static Matrix CreateFromAxisAngle(const Vector3& _axis, float _angle);

	static Matrix CreatePerspectiveFieldOfView(float _fov, float _aspectRatio,
		float _nearPlane, float _farPlane);
	static Matrix CreatePerspective(float _width, float _height,
		float _nearPlane, float _farPlane);
	static Matrix CreatePerspectiveOffCenter(float _left, float _right, 
		float _bottom, float _top, float _nearPlane, float _farPlane);
	static Matrix CreateOrthographic(float _width, float _height, 
		float _zNearPlane, float _zFarPlane);
	static Matrix CreateOrthographicOffCenter(float _left, float _right, 
		float _bottom, float _top, float _zNearPlane, float _zFarPlane);

	static Matrix CreateLookAt(const Vector3& _eye, const Vector3& _target, const Vector3& _up);
	static Matrix CreateWorld(const Vector3& _pos, const Vector3& _forward, const Vector3& _up);

	static Matrix CreateFromQuaternion(const Quaternion& _quaternion);

	static Matrix CreateFromYawPitchRoll(float _yaw, float _pitch, float _roll);

	static Matrix Lerp(const Matrix& _M1, const Matrix& _M2, float _t);

	static Matrix Transform(const Matrix& _M, const Quaternion& _rotation);

	static Matrix Identity();

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(_11), CEREAL_NVP(_12), CEREAL_NVP(_13), CEREAL_NVP(_14),
			CEREAL_NVP(_21), CEREAL_NVP(_22), CEREAL_NVP(_23), CEREAL_NVP(_24),
			CEREAL_NVP(_31), CEREAL_NVP(_32), CEREAL_NVP(_33), CEREAL_NVP(_34),
			CEREAL_NVP(_41), CEREAL_NVP(_42), CEREAL_NVP(_43), CEREAL_NVP(_44));
	}
};

Matrix operator+ (const Matrix& m1, const Matrix& m2);
Matrix operator- (const Matrix& m1, const Matrix& m2);
Matrix operator* (const Matrix& m1, const Matrix& m2);
Matrix operator* (const Matrix& m, float s);
Matrix operator* (float s, const Matrix& m);
Matrix operator/ (const Matrix& m, float s);
Matrix operator/ (const Matrix& m1, const Matrix& m2);



//--------------------------------------
// Quaternion
//--------------------------------------
struct Quaternion : public DirectX::XMFLOAT4
{
	Quaternion() noexcept : DirectX::XMFLOAT4(0, 0, 0, 1.f) {}
	XM_CONSTEXPR Quaternion(float _x, float _y, float _z, float _w) : DirectX::XMFLOAT4(_x, _y, _z, _w) {}
	Quaternion(const Vector3& _v, float _scalar) : DirectX::XMFLOAT4(_v.x, _v.y, _v.z, _scalar) {}
	explicit Quaternion(const Vector4& _v) : DirectX::XMFLOAT4(_v.x, _v.y, _v.z, _v.w) {}
	explicit Quaternion(_In_reads_(4) const float* pArray) : DirectX::XMFLOAT4(pArray) {}
	Quaternion(DirectX::FXMVECTOR _v) { XMStoreFloat4(this, _v); }
	Quaternion(const DirectX::XMFLOAT4& _q) { this->x = _q.x; this->y = _q.y; this->z = _q.z; this->w = _q.w; }
	explicit Quaternion(const DirectX::XMVECTORF32& _f) { this->x = _f.f[0]; this->y = _f.f[1]; this->z = _f.f[2]; this->w = _f.f[3]; }

	Quaternion(const Quaternion&) = default;
	Quaternion& operator=(const Quaternion&) = default;

	Quaternion(Quaternion&&) = default;
	Quaternion& operator=(Quaternion&&) = default;

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(this); }

	bool operator == (const Quaternion& _q) const;
	bool operator != (const Quaternion& _q) const;

	Quaternion& operator= (const DirectX::XMVECTORF32& _f) { 
		x = _f.f[0]; y = _f.f[1]; z = _f.f[2]; w = _f.f[3]; return *this; }
	Quaternion& operator+= (const Quaternion& _q);
	Quaternion& operator-= (const Quaternion& _q);
	Quaternion& operator*= (const Quaternion& _q);
	Quaternion& operator*= (float _s);
	Quaternion& operator/= (const Quaternion& _q);

	Quaternion operator+ () const { return *this; }
	Quaternion operator- () const;

	float Length() const;
	float LengthSquared() const;

	Quaternion Normalize();

	Quaternion Conjugate();

	Quaternion Inverse() const;

	float Dot(const Quaternion& _q) const;

	Vector3 GetEuler() const;
	void SetEuler(Vector3 _v);

	static Quaternion AngleAxis(const Vector3& _axis, float _angle);
	static Quaternion Euler(float _x, float _y, float _z);
	static Quaternion Euler(Vector3 _v);
	static Quaternion CreateFromRotationMatrix(const Matrix& _m);

	static Quaternion Lerp(const Quaternion& _q1, const Quaternion& _q2, float _t);

	static Quaternion Slerp(const Quaternion& _q1, const Quaternion& _q2, float _t);

	static Quaternion Concatenate(const Quaternion& _q1, const Quaternion& _q2);

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z), CEREAL_NVP(w));
	}
};

Quaternion operator+ (const Quaternion& _q1, const Quaternion& _q2);
Quaternion operator- (const Quaternion& _q1, const Quaternion& _q2);
Quaternion operator* (const Quaternion& _q1, const Quaternion& _q2);
Quaternion operator* (const Quaternion& _q, float _s);
Quaternion operator/ (const Quaternion& _q1, const Quaternion& _q2);
Quaternion operator* (float _s, const Quaternion& _q);

//--------------------------------------
// Quaternion
//--------------------------------------
inline bool Quaternion::operator == (const Quaternion& q) const
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&q);
	return DirectX::XMQuaternionEqual(q1, q2);
}

inline bool Quaternion::operator != (const Quaternion& q) const
{
	
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&q);
	return DirectX::XMQuaternionNotEqual(q1, q2);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline Quaternion& Quaternion::operator+= (const Quaternion& q)
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&q);
	DirectX::XMStoreFloat4(this, DirectX::XMVectorAdd(q1, q2));
	return *this;
}

inline Quaternion& Quaternion::operator-= (const Quaternion& q)
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&q);
	DirectX::XMStoreFloat4(this, DirectX::XMVectorSubtract(q1, q2));
	return *this;
}

inline Quaternion& Quaternion::operator*= (const Quaternion& q)
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&q);
	DirectX::XMStoreFloat4(this, DirectX::XMQuaternionMultiply(q1, q2));
	return *this;
}

inline Quaternion& Quaternion::operator*= (float S)
{
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(this);
	DirectX::XMStoreFloat4(this, DirectX::XMVectorScale(q, S));
	return *this;
}

inline Quaternion& Quaternion::operator/= (const Quaternion& q)
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&q);
	q2 = DirectX::XMQuaternionInverse(q2);
	DirectX::XMStoreFloat4(this, DirectX::XMQuaternionMultiply(q1, q2));
	return *this;
}

//------------------------------------------------------------------------------
// Urnary operators
//------------------------------------------------------------------------------

inline Quaternion Quaternion::operator- () const
{
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(this);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMVectorNegate(q));
	return R;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline Quaternion operator+ (const Quaternion& Q1, const Quaternion& Q2)
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&Q1);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&Q2);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMVectorAdd(q1, q2));
	return R;
}

inline Quaternion operator- (const Quaternion& Q1, const Quaternion& Q2)
{
	using namespace DirectX;
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&Q1);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&Q2);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMVectorSubtract(q1, q2));
	return R;
}

inline Quaternion operator* (const Quaternion& Q1, const Quaternion& Q2)
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&Q1);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&Q2);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionMultiply(q1, q2));
	return R;
}

inline Quaternion operator* (const Quaternion& Q, float S)
{
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&Q);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMVectorScale(q, S));
	return R;
}

inline Quaternion operator/ (const Quaternion& Q1, const Quaternion& Q2)
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&Q1);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&Q2);
	q2 = DirectX::XMQuaternionInverse(q2);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionMultiply(q1, q2));
	return R;
}

inline Quaternion operator* (float S, const Quaternion& Q)
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&Q);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMVectorScale(q1, S));
	return R;
}

inline float Quaternion::Length() const
{
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(this);
	return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(q));
}
inline float Quaternion::LengthSquared() const
{
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(this);
	return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(q));
}

inline Quaternion Quaternion::Normalize()
{
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(this);
	DirectX::XMStoreFloat4(this, DirectX::XMQuaternionNormalize(q));
}

inline Quaternion Quaternion::Conjugate()
{
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(this);
	DirectX::XMStoreFloat4(this, DirectX::XMQuaternionConjugate(q));
}

inline Quaternion Quaternion::Inverse() const
{
	Quaternion R;
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(this);
	DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionInverse(q));
}

inline float Quaternion::Dot(const Quaternion& _q) const
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&_q);
	return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(q1, q2));
}

inline Vector3 Quaternion::GetEuler() const
{
	Vector3 R = { 0,0,0 };
	Matrix M = Matrix::CreateFromQuaternion(*this);

	if (M._32 >= 1.0f)
	{
		R.x = DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else if (M._32 <= -1.0f)
	{
		R.x = -DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else
	{
		R.x = asinf(-M._32);
		R.y = atan2f(M._31, M._33);
		R.z = atan2f(M._12, M._22);
	}

	R.x = DirectX::XMConvertToDegrees(R.x);
	R.y = DirectX::XMConvertToDegrees(R.y);
	R.z = DirectX::XMConvertToDegrees(R.z);
	return R;
}

inline void Quaternion::SetEuler(Vector3 _v)
{
	Quaternion Q = Quaternion::Euler(_v);
	DirectX::XMStoreFloat4(this, DirectX::XMLoadFloat4(&Q));
}

inline Quaternion Quaternion::AngleAxis(const Vector3& _axis, float _angle)
{
	DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&_axis);
	
	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionRotationAxis(a, _angle));
	return R;
}

inline Quaternion Quaternion::Euler(float _x, float _y, float _z)
{
	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionRotationRollPitchYaw(
		DirectX::XMConvertToRadians(_x), DirectX::XMConvertToRadians(_y), DirectX::XMConvertToRadians(_z)));
	return R;
}
inline Quaternion Quaternion::Euler(Vector3 _v)
{
	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionRotationRollPitchYaw(
	DirectX::XMConvertToRadians(_v.x), DirectX::XMConvertToRadians(_v.y), DirectX::XMConvertToRadians(_v.z)));
	return R;
}

inline Quaternion Quaternion::CreateFromRotationMatrix(const Matrix& _m)
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&_m);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionRotationMatrix(M));
	return R;
}

inline Quaternion Quaternion::Lerp(const Quaternion& _q1, const Quaternion& _q2, float _t)
{
	using namespace DirectX;
	DirectX::XMVECTOR Q0 = DirectX::XMLoadFloat4(&_q1);
	DirectX::XMVECTOR Q1 = DirectX::XMLoadFloat4(&_q2);

	DirectX::XMVECTOR dot = DirectX::XMVector4Dot(Q0, Q1);

	DirectX::XMVECTOR R;
	if (DirectX::XMVector4GreaterOrEqual(dot, DirectX::XMVectorZero()))
	{
		R = DirectX::XMVectorLerp(Q0, Q1, _t);
	}
	else
	{
		DirectX::XMVECTOR tv = DirectX::XMVectorReplicate(_t);
		DirectX::XMVECTOR t1v = DirectX::XMVectorReplicate(1.f - _t);
		DirectX::XMVECTOR X0 = DirectX::XMVectorMultiply(Q0, t1v);
		DirectX::XMVECTOR X1 = DirectX::XMVectorMultiply(Q1, tv);
		R = DirectX::XMVectorSubtract(X0, X1);
	}

	Quaternion result;
	DirectX::XMStoreFloat4(&result, DirectX::XMQuaternionNormalize(R));
	return result;
}

inline Quaternion Quaternion::Slerp(const Quaternion& _q1, const Quaternion& _q2, float _t) 
{
	DirectX::XMVECTOR Q0 = DirectX::XMLoadFloat4(&_q1);
	DirectX::XMVECTOR Q1 = DirectX::XMLoadFloat4(&_q2);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionSlerp(Q0, Q1, _t));
	return R;
}

inline Quaternion Quaternion::Concatenate(const Quaternion& _q1, const Quaternion& _q2)
{
	DirectX::XMVECTOR Q0 = DirectX::XMLoadFloat4(&_q1);
	DirectX::XMVECTOR Q1 = DirectX::XMLoadFloat4(&_q2);

	Quaternion R;
	DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionMultiply(Q1, Q0));
	return R;
}


//--------------------------------------
// Vector2 
//--------------------------------------
inline bool Vector2::operator== (const Vector2& _v) const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v);
	return DirectX::XMVector2Equal(v1, v2);
}

inline bool Vector2::operator!= (const Vector2& _v) const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v);
	return DirectX::XMVector2NotEqual(v1, v2);
}

inline Vector2& Vector2::operator+= (const Vector2& _v)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v);
	DirectX::XMVECTOR res = DirectX::XMVectorAdd(v1, v2);
	DirectX::XMStoreFloat2(this, res);
	return *this;
}

inline Vector2& Vector2::operator-= (const Vector2& _v)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v);
	DirectX::XMVECTOR res = DirectX::XMVectorSubtract(v1, v2);
	DirectX::XMStoreFloat2(this, res);
	return *this;
}

inline Vector2& Vector2::operator*= (const Vector2& _v)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v);
	DirectX::XMVECTOR res = DirectX::XMVectorMultiply(v1, v2);
	DirectX::XMStoreFloat2(this, res);
	return *this;
}

inline Vector2& Vector2::operator*= (float _s)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR res = DirectX::XMVectorScale(v1, _s);
	DirectX::XMStoreFloat2(this, res);
	return *this;
}

inline Vector2& Vector2::operator/= (float _s)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR res = DirectX::XMVectorScale(v1, 1.0f / _s);
	DirectX::XMStoreFloat2(this, res);
	return *this;
}

inline Vector2 operator+ (const Vector2& _v1, const Vector2& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorAdd(v1, v2);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline Vector2 operator- (const Vector2& _v1, const Vector2& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorSubtract(v1, v2);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline Vector2 operator* (const Vector2& _v1, const Vector2& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorMultiply(v1, v2);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline Vector2 operator* (const Vector2& _v1, float _s)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR R = DirectX::XMVectorScale(v1, _s);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline Vector2 operator* (float _s, const Vector2& _v1)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR R = DirectX::XMVectorScale(v1, _s);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline Vector2 operator/ (const Vector2& _v1, const Vector2& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorDivide(v1, v2);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline float Vector2::Length() const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR res = DirectX::XMVector2Length(v1);
	return DirectX::XMVectorGetX(res);
}

inline float Vector2::LengthSq() const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR res = DirectX::XMVector2LengthSq(v1);
	return DirectX::XMVectorGetX(res);
}

inline float Vector2::Dot(const Vector2& _v) const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v);
	DirectX::XMVECTOR res = DirectX::XMVector2Dot(v1, v2);
	return DirectX::XMVectorGetX(res);
}

inline Vector2 Vector2::Cross(const Vector2& _v) const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v);
	DirectX::XMVECTOR R = DirectX::XMVector2Cross(v1, v2);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline Vector2 Vector2::Normalize()
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR R = DirectX::XMVector2Normalize(v1);
	DirectX::XMStoreFloat2(this, R);
	return *this;
}

inline Vector2 Vector2::Clamp(const Vector2& _min, const Vector2 _max)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_min);
	DirectX::XMVECTOR v3 = DirectX::XMLoadFloat2(&_max);
	DirectX::XMVECTOR R = DirectX::XMVectorClamp(v1, v2, v3);
	DirectX::XMStoreFloat2(this, R);
	return *this;
}

inline float Vector2::Distance(const Vector2& _v1, const Vector2& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR v = DirectX::XMVectorSubtract(v2, v1);
	DirectX::XMVECTOR res = DirectX::XMVector2Length(v);
	return DirectX::XMVectorGetX(res);
}
inline float Vector2::DistanceSq(const Vector2& _v1, const Vector2& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR v = DirectX::XMVectorSubtract(v2, v1);
	DirectX::XMVECTOR res = DirectX::XMVector2LengthSq(v);
	return DirectX::XMVectorGetX(res);
}

inline Vector2 Vector2::Min(const Vector2& _v1, const Vector2& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorMin(v1, v2);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline Vector2 Vector2::Max(const Vector2& _v1, const Vector2& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorMax(v1, v2);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline Vector2 Vector2::Lerp(const Vector2& _v1, const Vector2& _v2, float _t)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorLerp(v1, v2, _t);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

inline Vector2 Vector2::SmoothStep(const Vector2& _v1, const Vector2& _v2, float _t)
{
	if (_t > 1.0f)
	{
		_t = 1.0f;
	}
	else if (_t < 0.0f)
	{
		_t = 0.0f;
	}

	_t = _t * _t * (3.0f - 2.0f * _t);

	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorLerp(v1, v2, _t);
	Vector2 res;
	DirectX::XMStoreFloat2(&res, R);
	return res;
}

//--------------------------------------
// Vector3 
//--------------------------------------
inline bool Vector3::operator== (const Vector3& v) const
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v);
	return DirectX::XMVector3Equal(tempVector1, tempVector2);
}

inline bool Vector3::operator!= (const Vector3& v) const
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v);
	return DirectX::XMVector3NotEqual(tempVector1, tempVector2);
}

inline Vector3& Vector3::operator+= (const Vector3& v)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v);
	DirectX::XMStoreFloat3(this, DirectX::XMVectorAdd(tempVector1, tempVector2));
	return *this;
}

inline Vector3& Vector3::operator-= (const Vector3& v)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v);
	DirectX::XMStoreFloat3(this, DirectX::XMVectorSubtract(tempVector1, tempVector2));
	return *this;
}

inline Vector3& Vector3::operator*= (const Vector3& v)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v);
	DirectX::XMStoreFloat3(this, DirectX::XMVectorMultiply(tempVector1, tempVector2));
	return *this;
}

inline Vector3& Vector3::operator*= (float s)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	DirectX::XMStoreFloat3(this, DirectX::XMVectorScale(tempVector1, s));
	return *this;
}

inline Vector3& Vector3::operator/= (float s)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	DirectX::XMStoreFloat3(this, DirectX::XMVectorScale(tempVector1, 1.0f / s));
	return *this;
}

inline Vector3 operator+ (const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 addVector;
	DirectX::XMStoreFloat3(&addVector, DirectX::XMVectorAdd(tempVector1, tempVector2));
	return addVector;
}

inline Vector3 operator- (const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 subtractVector;
	DirectX::XMStoreFloat3(&subtractVector, DirectX::XMVectorSubtract(tempVector1, tempVector2));
	return subtractVector;
}

inline Vector3 operator* (const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 multiplyVector;
	DirectX::XMStoreFloat3(&multiplyVector, DirectX::XMVectorMultiply(tempVector1, tempVector2));
	return multiplyVector;
}

inline Vector3 operator* (const Vector3& v, float s)
{
	const DirectX::XMVECTOR tempVector = DirectX::XMLoadFloat3(&v);
	Vector3 scaleVector;
	DirectX::XMStoreFloat3(&scaleVector, DirectX::XMVectorScale(tempVector, s));
	return scaleVector;
}

inline Vector3 operator* (float s, const Vector3& v)
{
	const DirectX::XMVECTOR tempVector = DirectX::XMLoadFloat3(&v);
	Vector3 scaleVector;
	DirectX::XMStoreFloat3(&scaleVector, DirectX::XMVectorScale(tempVector, s));
	return scaleVector;
}

inline Vector3 operator/ (const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 divideVector;
	DirectX::XMStoreFloat3(&divideVector, DirectX::XMVectorDivide(tempVector1, tempVector2));
	return divideVector;
}

inline float Vector3::Length() const
{
	const DirectX::XMVECTOR tempVector = DirectX::XMLoadFloat3(this);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(tempVector);

	return DirectX::XMVectorGetX(length);
}

inline float Vector3::LengthSq() const
{
	const DirectX::XMVECTOR tempVector = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR lengthSq = DirectX::XMVector3LengthSq(tempVector);

	return DirectX::XMVectorGetX(lengthSq);
}

inline Vector3 Vector3::Dot(const Vector3& v)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v);
	const DirectX::XMVECTOR dotVector = DirectX::XMVector3Dot(tempVector1, tempVector2);
	DirectX::XMStoreFloat3(this, dotVector);
	return *this;
}

inline Vector3 Vector3::Cross(const Vector3& v)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v);
	const DirectX::XMVECTOR crossVector = DirectX::XMVector3Cross(tempVector1, tempVector2);
	DirectX::XMStoreFloat3(this, crossVector);
	return *this;
}

inline Vector3 Vector3::Normalize()
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR normalVector = DirectX::XMVector3Normalize(tempVector1);
	DirectX::XMStoreFloat3(this, normalVector);
	return *this;
}

inline Vector3 Vector3::Clamp(const Vector3& min, const Vector3 max)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(this);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&min);
	const DirectX::XMVECTOR tempVector3 = DirectX::XMLoadFloat3(&max);
	const DirectX::XMVECTOR clampVector = DirectX::XMVectorClamp(tempVector1, tempVector2, tempVector3);
	DirectX::XMStoreFloat3(this, clampVector);
	return *this;
}

inline float Vector3::Distance(const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	const DirectX::XMVECTOR subtractVector = DirectX::XMVectorSubtract(tempVector2, tempVector1);
	const float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(subtractVector));
	return distance;
}
inline float Vector3::DistanceSq(const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	const DirectX::XMVECTOR subtractVector = DirectX::XMVectorSubtract(tempVector2, tempVector1);
	const float distanceSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(subtractVector));
	return distanceSq; 
}

inline Vector3 Vector3::Min(const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 min;
	DirectX::XMStoreFloat3(&min, DirectX::XMVectorMin(tempVector1, tempVector2));
	return min;
}

inline Vector3 Vector3::Max(const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 max;
	DirectX::XMStoreFloat3(&max, DirectX::XMVectorMax(tempVector1, tempVector2));
	return max;
}

inline Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, const float time)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 lerpVector;
	DirectX::XMStoreFloat3(&lerpVector, DirectX::XMVectorLerp(tempVector1, tempVector2, time));
	return lerpVector;
}

inline Vector3 Vector3::SmoothStep(const Vector3& v1, const Vector3& v2, float time)
{

	// Smooth Step
	if (time > 1.0f)
	{
		time = 1.0f;
	}
	else if (time < 0.0f)
	{
		time = 0.0f;
	}

	time = time * time * (3.0f - 2.0f * time);

	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 smoothVector;
	DirectX::XMStoreFloat3(&smoothVector, DirectX::XMVectorLerp(tempVector1, tempVector2, time));
	return smoothVector;
}

inline Vector3 Vector3::Transform(const Vector3& v1, const Matrix& v2)
{
	Vector3 transformVector;
	DirectX::XMStoreFloat3(&transformVector, DirectX::XMVector3Transform(v1, v2));
	return transformVector;
}

inline Vector3 Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 dotVector;
	DirectX::XMStoreFloat3(&dotVector, DirectX::XMVector3Dot(tempVector1, tempVector2));
	return dotVector;
}

inline Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v1);
	const DirectX::XMVECTOR tempVector2 = DirectX::XMLoadFloat3(&v2);
	Vector3 crossVector;
	DirectX::XMStoreFloat3(&crossVector, DirectX::XMVector3Cross(tempVector1, tempVector2));
	return crossVector;
}

inline Vector3 Vector3::Normalize(const Vector3& v)
{
	const DirectX::XMVECTOR tempVector1 = DirectX::XMLoadFloat3(&v);
	Vector3 normalVector;
	DirectX::XMStoreFloat3(&normalVector, DirectX::XMVector3Normalize(tempVector1));
	return normalVector;
}



//--------------------------------------
// Vector4
//--------------------------------------
inline bool Vector4::operator== (const Vector4& _v) const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v);
	return DirectX::XMVector4Equal(v1, v2);
}

inline bool Vector4::operator!= (const Vector4& _v) const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v);
	return DirectX::XMVector4NotEqual(v1, v2);
}

inline Vector4& Vector4::operator+= (const Vector4& _v)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v);
	DirectX::XMVECTOR res = DirectX::XMVectorAdd(v1, v2);
	DirectX::XMStoreFloat4(this, res);
	return *this;
}

inline Vector4& Vector4::operator-= (const Vector4& _v)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v);
	DirectX::XMVECTOR res = DirectX::XMVectorSubtract(v1, v2);
	DirectX::XMStoreFloat4(this, res);
	return *this;
}

inline Vector4& Vector4::operator*= (const Vector4& _v)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v);
	DirectX::XMVECTOR res = DirectX::XMVectorMultiply(v1, v2);
	DirectX::XMStoreFloat4(this, res);
	return *this;
}

inline Vector4& Vector4::operator*= (float _s)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR res = DirectX::XMVectorScale(v1, _s);
	DirectX::XMStoreFloat4(this, res);
	return *this;
}

inline Vector4& Vector4::operator/= (float _s)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR res = DirectX::XMVectorScale(v1, 1.0f / _s);
	DirectX::XMStoreFloat4(this, res);
	return *this;
}

inline Vector4 operator+ (const Vector4& _v1, const Vector4& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorAdd(v1, v2);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}

inline Vector4 operator- (const Vector4& _v1, const Vector4& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorSubtract(v1, v2);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}

inline Vector4 operator* (const Vector4& _v1, const Vector4& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorMultiply(v1, v2);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}

inline Vector4 operator* (const Vector4& _v1, float _s)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR R = DirectX::XMVectorScale(v1, _s);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}

inline Vector4 operator* (float _s, const Vector4& _v1)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR R = DirectX::XMVectorScale(v1, _s);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}

inline Vector4 operator/ (const Vector4& _v1, const Vector4& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorDivide(v1, v2);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}

inline float Vector4::Length() const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR res = DirectX::XMVector4Length(v1);
	return DirectX::XMVectorGetX(res);
}

inline float Vector4::LengthSq() const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR res = DirectX::XMVector4LengthSq(v1);
	return DirectX::XMVectorGetX(res);
}

inline float Vector4::Dot(const Vector4& _v) const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v);
	DirectX::XMVECTOR res = DirectX::XMVector4Dot(v1, v2);
	return DirectX::XMVectorGetX(res);
}

inline Vector4 Vector4::Cross(const Vector4& _v1, const Vector4& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v3 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVector4Cross(v1, v2, v3);
	DirectX::XMStoreFloat4(this, R);
	return *this;
}

inline Vector4 Vector4::Normalize()
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR R = DirectX::XMVector4Normalize(v1);
	DirectX::XMStoreFloat4(this, R);
	return *this;
}

inline Vector4 Vector4::Clamp(const Vector4& _min, const Vector4 _max)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_min);
	DirectX::XMVECTOR v3 = DirectX::XMLoadFloat4(&_max);
	DirectX::XMVECTOR R = DirectX::XMVectorClamp(v1, v2, v3);
	DirectX::XMStoreFloat4(this, R);
	return *this;
}

inline float Vector4::Distance(const Vector4& _v1, const Vector4& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR v = DirectX::XMVectorSubtract(v2, v1);
	DirectX::XMVECTOR res = DirectX::XMVector4Length(v);
	return DirectX::XMVectorGetX(res);
}
inline float Vector4::DistanceSq(const Vector4& _v1, const Vector4& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR v = DirectX::XMVectorSubtract(v2, v1);
	DirectX::XMVECTOR res = DirectX::XMVector4LengthSq(v);
	return DirectX::XMVectorGetX(res);
}

inline Vector4 Vector4::Min(const Vector4& _v1, const Vector4& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorMin(v1, v2);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}

inline Vector4 Vector4::Max(const Vector4& _v1, const Vector4& _v2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorMax(v1, v2);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}

inline Vector4 Vector4::Lerp(const Vector4& _v1, const Vector4& _v2, float _t)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorLerp(v1, v2, _t);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}

inline Vector4 Vector4::SmoothStep(const Vector4& _v1, const Vector4& _v2, float _t)
{
	if (_t > 1.0f)
	{
		_t = 1.0f;
	}
	else if (_t < 0.0f)
	{
		_t = 0.0f;
	}

	_t = _t * _t * (3.0f - 2.0f * _t);

	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&_v1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&_v2);
	DirectX::XMVECTOR R = DirectX::XMVectorLerp(v1, v2, _t);
	Vector4 res;
	DirectX::XMStoreFloat4(&res, R);
	return res;
}


//--------------------------------------
// Matrix 
//--------------------------------------
inline Matrix::Matrix(const DirectX::XMFLOAT3X3& _M)
{
	_11 = _M._11; _12 = _M._12; _13 = _M._13; _14 = 0.f;
	_21 = _M._21; _22 = _M._22; _23 = _M._23; _24 = 0.f;
	_31 = _M._31; _32 = _M._32; _33 = _M._33; _34 = 0.f;
	_41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
}

inline Matrix::Matrix(const DirectX::XMFLOAT4X3& _M)
{
	_11 = _M._11; _12 = _M._12; _13 = _M._13; _14 = 0.f;
	_21 = _M._21; _22 = _M._22; _23 = _M._23; _24 = 0.f;
	_31 = _M._31; _32 = _M._32; _33 = _M._33; _34 = 0.f;
	_41 = _M._41; _42 = _M._42; _43 = _M._43; _44 = 1.f;
}

inline bool Matrix::operator== (const Matrix& _m) const
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._11));
	DirectX::XMVECTOR y2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._21));
	DirectX::XMVECTOR y3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._31));
	DirectX::XMVECTOR y4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._41));

	return (
		DirectX::XMVector4Equal(x1, y1) &&
		DirectX::XMVector4Equal(x2, y2) &&
		DirectX::XMVector4Equal(x3, y3) &&
		DirectX::XMVector4Equal(x4, y4)) != 0;
}

inline bool Matrix::operator!= (const Matrix& _m) const
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._11));
	DirectX::XMVECTOR y2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._21));
	DirectX::XMVECTOR y3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._31));
	DirectX::XMVECTOR y4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._41));

	return (
		DirectX::XMVector4NotEqual(x1, y1) ||
		DirectX::XMVector4NotEqual(x2, y2) ||
		DirectX::XMVector4NotEqual(x3, y3) ||
		DirectX::XMVector4NotEqual(x4, y4)) != 0;
}

inline Matrix& Matrix::operator= (const DirectX::XMFLOAT3X3& _M)
{
	_11 = _M._11; _12 = _M._12; _13 = _M._13; _14 = 0.f;
	_21 = _M._21; _22 = _M._22; _23 = _M._23; _24 = 0.f;
	_31 = _M._31; _32 = _M._32; _33 = _M._33; _34 = 0.f;
	_41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
	return *this;
}

inline Matrix& Matrix::operator= (const DirectX::XMFLOAT4X3& _M)
{
	_11 = _M._11; _12 = _M._12; _13 = _M._13; _14 = 0.f;
	_21 = _M._21; _22 = _M._22; _23 = _M._23; _24 = 0.f;
	_31 = _M._31; _32 = _M._32; _33 = _M._33; _34 = 0.f;
	_41 = _M._41; _42 = _M._42; _43 = _M._43; _44 = 1.f;
	return *this;
}

inline Matrix& Matrix::operator+= (const Matrix& _m)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._11));
	DirectX::XMVECTOR y2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._21));
	DirectX::XMVECTOR y3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._31));
	DirectX::XMVECTOR y4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._41));

	x1 = DirectX::XMVectorAdd(x1, y1);
	x2 = DirectX::XMVectorAdd(x2, y2);
	x3 = DirectX::XMVectorAdd(x3, y3);
	x4 = DirectX::XMVectorAdd(x4, y4);

	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);

	return *this;
}

inline Matrix& Matrix::operator-= (const Matrix& _m)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._11));
	DirectX::XMVECTOR y2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._21));
	DirectX::XMVECTOR y3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._31));
	DirectX::XMVECTOR y4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._41));

	x1 = DirectX::XMVectorSubtract(x1, y1);
	x2 = DirectX::XMVectorSubtract(x2, y2);
	x3 = DirectX::XMVectorSubtract(x3, y3);
	x4 = DirectX::XMVectorSubtract(x4, y4);

	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);

	return *this;
}

inline Matrix& Matrix::operator*= (const Matrix& _m)
{
	DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(this);
	DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&_m);
	DirectX::XMMATRIX res = DirectX::XMMatrixMultiply(m1, m2);
	DirectX::XMStoreFloat4x4(this, res);
	return *this;
}

inline Matrix& Matrix::operator*= (float _s)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	x1 = DirectX::XMVectorScale(x1, _s);
	x2 = DirectX::XMVectorScale(x2, _s);
	x3 = DirectX::XMVectorScale(x3, _s);
	x4 = DirectX::XMVectorScale(x4, _s);

	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);
	return *this;
}

inline Matrix& Matrix::operator/= (float _s)
{
	assert(_s != 0.0f);
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	float rs = 1.0f / _s;
	x1 = DirectX::XMVectorScale(x1, rs);
	x2 = DirectX::XMVectorScale(x2, rs);
	x3 = DirectX::XMVectorScale(x3, rs);
	x4 = DirectX::XMVectorScale(x4, rs);

	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);
	return *this;
}

inline Matrix& Matrix::operator/= (const Matrix& _m)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._11));
	DirectX::XMVECTOR y2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._21));
	DirectX::XMVECTOR y3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._31));
	DirectX::XMVECTOR y4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._41));

	x1 = DirectX::XMVectorDivide(x1, y1);
	x2 = DirectX::XMVectorDivide(x2, y2);
	x3 = DirectX::XMVectorDivide(x3, y3);
	x4 = DirectX::XMVectorDivide(x4, y4);

	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);
	return *this;
}

inline Matrix Matrix::operator- () const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR v3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR v4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	v1 = DirectX::XMVectorNegate(v1);
	v2 = DirectX::XMVectorNegate(v2);
	v3 = DirectX::XMVectorNegate(v3);
	v4 = DirectX::XMVectorNegate(v4);

	Matrix R;
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), v1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), v2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), v3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), v4);

	return R;
}

inline Matrix operator+ (const Matrix& _m1, const Matrix& _m2)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._41));

	DirectX::XMVECTOR y1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._11));
	DirectX::XMVECTOR y2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._21));
	DirectX::XMVECTOR y3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._31));
	DirectX::XMVECTOR y4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._41));

	x1 = DirectX::XMVectorAdd(x1, y1);
	x2 = DirectX::XMVectorAdd(x2, y2);
	x3 = DirectX::XMVectorAdd(x3, y3);
	x4 = DirectX::XMVectorAdd(x4, y4);

	Matrix R;
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);

	return R;
}

inline Matrix operator- (const Matrix& _m1, const Matrix& _m2)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._41));

	DirectX::XMVECTOR y1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._11));
	DirectX::XMVECTOR y2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._21));
	DirectX::XMVECTOR y3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._31));
	DirectX::XMVECTOR y4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._41));

	x1 = DirectX::XMVectorSubtract(x1, y1);
	x2 = DirectX::XMVectorSubtract(x2, y2);
	x3 = DirectX::XMVectorSubtract(x3, y3);
	x4 = DirectX::XMVectorSubtract(x4, y4);

	Matrix R;
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);

	return R;
}

inline Matrix operator* (const Matrix& _m1, const Matrix& _m2)
{
	DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(&_m1);
	DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&_m2);
	DirectX::XMMATRIX rs = DirectX::XMMatrixMultiply(m1, m2);

	Matrix R;
	DirectX::XMStoreFloat4x4(&R, rs);
	return R;
}

inline Matrix operator* (const Matrix& _m1, float _s)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._41));

	x1 = DirectX::XMVectorScale(x1, _s);
	x2 = DirectX::XMVectorScale(x2, _s);
	x3 = DirectX::XMVectorScale(x3, _s);
	x4 = DirectX::XMVectorScale(x4, _s);

	Matrix R;
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);

	return R;
}

inline Matrix operator* (float _s, const Matrix& _m1)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._41));

	x1 = DirectX::XMVectorScale(x1, _s);
	x2 = DirectX::XMVectorScale(x2, _s);
	x3 = DirectX::XMVectorScale(x3, _s);
	x4 = DirectX::XMVectorScale(x4, _s);

	Matrix R;
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);

	return R;
}

inline Matrix operator/ (const Matrix& _m, float _s)
{
	assert(_s != 0.0f);
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m._41));

	float rs = 1.0f / _s;

	x1 = DirectX::XMVectorScale(x1, rs);
	x2 = DirectX::XMVectorScale(x2, rs);
	x3 = DirectX::XMVectorScale(x3, rs);
	x4 = DirectX::XMVectorScale(x4, rs);

	Matrix R;
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);

	return R;
}

inline Matrix operator/ (const Matrix& _m1, const Matrix& _m2)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m1._41));

	DirectX::XMVECTOR y1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._11));
	DirectX::XMVECTOR y2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._21));
	DirectX::XMVECTOR y3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._31));
	DirectX::XMVECTOR y4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_m2._41));

	x1 = DirectX::XMVectorDivide(x1, y1);
	x2 = DirectX::XMVectorDivide(x2, y2);
	x3 = DirectX::XMVectorDivide(x3, y3);
	x4 = DirectX::XMVectorDivide(x4, y4);

	Matrix R;
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);

	return R;
}


inline bool Matrix::Decompose(Vector3& _scale, Quaternion& _rotation, Vector3& _translation)
{
	DirectX::XMVECTOR s, r, t;

	if (!DirectX::XMMatrixDecompose(&s, &r, &t, *this))
		return false;

	DirectX::XMStoreFloat3(&_scale, s);
	DirectX::XMStoreFloat4(&_rotation, r);
	DirectX::XMStoreFloat3(&_translation, t);

	return true;
}

// Get Transpose Matrix
inline Matrix Matrix::Transpose() const
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(this);
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixTranspose(M));
	return R;
}

// Get Inverse Matrix
inline Matrix Matrix::Invert() const
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(this);
	Matrix R;
	DirectX::XMVECTOR det;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixInverse(&det, M));
	return R;
}

inline float Matrix::Determinant() const
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(this);

}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

_Use_decl_annotations_
inline Matrix Matrix::CreateBillboard(const Vector3& _obj, const Vector3& _cameraPos,
	const Vector3& _cameraUp, _In_opt_ const Vector3* _cameraForward)
{
	DirectX::XMVECTOR O = DirectX::XMLoadFloat3(&_obj);
	DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&_cameraPos);
	DirectX::XMVECTOR Z = DirectX::XMVectorSubtract(O, C);

	DirectX::XMVECTOR N = DirectX::XMVector3LengthSq(Z);
	if (DirectX::XMVector3Less(N, DirectX::g_XMEpsilon))
	{
		if (_cameraForward)
		{
			DirectX::XMVECTOR F = DirectX::XMLoadFloat3(_cameraForward);
			Z = DirectX::XMVectorNegate(F);
		}
		else
		{
			Z = DirectX::g_XMNegIdentityR2;
		}
	}
	else
	{
		Z = DirectX::XMVector3Normalize(Z);
	}

	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&_cameraUp);
	DirectX::XMVECTOR X = DirectX::XMVector3Cross(up, Z);
	X = DirectX::XMVector3Normalize(X);

	DirectX::XMVECTOR Y = DirectX::XMVector3Cross(Z, X);

	DirectX::XMMATRIX M;
	M.r[0] = X;
	M.r[1] = Y;
	M.r[2] = Z;
	M.r[3] = DirectX::XMVectorSetW(O, 1.0f);

	Matrix R;
	DirectX::XMStoreFloat4x4(&R, M);

	return R;
}


_Use_decl_annotations_
inline Matrix Matrix::CreateConstrainedBillboard(const Vector3& _obj, const Vector3& _cameraPos,
	const Vector3& _rotateAxis, _In_opt_ const Vector3* _cameraForward,
	_In_opt_ const Vector3* _objForward)
{
	static const DirectX::XMVECTORF32 s_MinAngle = { {{
		0.99825467075f,0.99825467075f,0.99825467075f,0.99825467075f}} };
	// 1.0 - XMConvertToRadians( 0.1f );

	DirectX::XMVECTOR O = DirectX::XMLoadFloat3(&_obj);
	DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&_cameraPos);
	DirectX::XMVECTOR faceDir = DirectX::XMVectorSubtract(O, C);

	DirectX::XMVECTOR N = DirectX::XMVector3LengthSq(faceDir);

	if (DirectX::XMVector3Less(N, DirectX::g_XMEpsilon))
	{
		if (_cameraForward)
		{
			DirectX::XMVECTOR F = DirectX::XMLoadFloat3(_cameraForward);
			faceDir = DirectX::XMVectorNegate(F);
		}
		else
		{
			faceDir = DirectX::g_XMNegIdentityR2;
		}
	}
	else
	{
		faceDir = DirectX::XMVector3Normalize(faceDir);
	}

	DirectX::XMVECTOR Y = DirectX::XMLoadFloat3(&_rotateAxis);
	DirectX::XMVECTOR X, Z;

	DirectX::XMVECTOR dot = DirectX::XMVectorAbs(DirectX::XMVector3Dot(Y, faceDir));

	if (DirectX::XMVector3Greater(dot, s_MinAngle))
	{
		if (_objForward)
		{
			Z = DirectX::XMLoadFloat3(_objForward);
			dot = DirectX::XMVectorAbs(DirectX::XMVector3Dot(Y, DirectX::g_XMNegIdentityR2));
		}
		else
		{
			dot = DirectX::XMVectorAbs(DirectX::XMVector3Dot(Y, DirectX::g_XMNegIdentityR2));
			if (DirectX::XMVector3Greater(dot, s_MinAngle))
			{
				Z = DirectX::g_XMIdentityR0;
			}
			else
			{
				Z = DirectX::g_XMNegIdentityR2;
			}

			X = DirectX::XMVector3Cross(Y, Z);
			X = DirectX::XMVector3Normalize(X);

			Z = DirectX::XMVector3Cross(X, Y);
			Z = DirectX::XMVector3Normalize(Z);
		}
	}
	else
	{
		X = DirectX::XMVector3Cross(Y, faceDir);
		X = DirectX::XMVector3Normalize(X);

		Z = DirectX::XMVector3Cross(X, Y);
		Z = DirectX::XMVector3Normalize(Z);
	}

	DirectX::XMMATRIX M;
	M.r[0] = X;
	M.r[1] = Y;
	M.r[2] = Z;
	M.r[3] = DirectX::XMVectorSetW(O, 1.0f);

	Matrix R;
	DirectX::XMStoreFloat4x4(&R, M);

	return R;
}

inline Matrix Matrix::CreateTranslation(const Vector3& _pos)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z));
	return R;
}

inline Matrix Matrix::CreateTranslation(float _x, float _y, float _z)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixTranslation(_x, _y, _z));
	return R;
}

inline Matrix Matrix::CreateScale(const Vector3& _scale)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixScaling(_scale.x, _scale.y, _scale.z));
	return R;
}

inline Matrix Matrix::CreateScale(float _x, float _y, float _z)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixScaling(_x, _y, _z));
	return R;
}

inline Matrix Matrix::CreateScale(float _scale)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixScaling(_scale, _scale, _scale));
	return R;
}

inline Matrix Matrix::CreateRotationX(float _radians)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixRotationX(_radians));
	return R;
}

inline Matrix Matrix::CreateRotationY(float _radians)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixRotationY(_radians));
	return R;
}

inline Matrix Matrix::CreateRotationZ(float _radians)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixRotationZ(_radians));
	return R;
}

inline Matrix Matrix::CreateFromAxisAngle(const Vector3& _axis, float _angle)
{
	Matrix R;
	DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&_axis);
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixRotationAxis(v, _angle));
	return R;
}

inline Matrix Matrix::CreatePerspectiveFieldOfView(float _fov, float _aspectRatio,
	float _nearPlane, float _farPlane)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixPerspectiveFovRH(
		_fov, _aspectRatio, _nearPlane, _farPlane));
	return R;
}
inline Matrix Matrix::CreatePerspective(float _width, float _height,
	float _nearPlane, float _farPlane)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixPerspectiveRH(
		_width, _height, _nearPlane, _farPlane));
	return R;

}

inline Matrix Matrix::CreatePerspectiveOffCenter(float _left, float _right,
	float _bottom, float _top, float _nearPlane, float _farPlane)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixPerspectiveOffCenterRH(
		_left, _right, _bottom, _top, _nearPlane, _farPlane));
	return R;
}

inline Matrix Matrix::CreateOrthographic(float _width, float _height,
	float _zNearPlane, float _zFarPlane)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixOrthographicRH(
		_width, _height, _zNearPlane, _zFarPlane));
	return R;
}
inline Matrix Matrix::CreateOrthographicOffCenter(float _left, float _right,
	float _bottom, float _top, float _zNearPlane, float _zFarPlane)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixOrthographicOffCenterRH(
		_left, _right, _bottom, _top, _zNearPlane, _zFarPlane));
	return R;
}

inline Matrix Matrix::CreateLookAt(const Vector3& _eye, const Vector3& _target, const Vector3& _up)
{
	Matrix R;
	DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&_eye);
	DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&_target);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&_up);
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixLookAtRH(eye, target, up));
	return R;
}

inline Matrix Matrix::CreateWorld(const Vector3& _pos, const Vector3& _forward, const Vector3& _up)
{
	DirectX::XMVECTOR zaxis = DirectX::XMVector3Normalize(DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&_forward)));
	DirectX::XMVECTOR yaxis = DirectX::XMLoadFloat3(&_up);
	DirectX::XMVECTOR xaxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(yaxis, zaxis));
	yaxis = DirectX::XMVector3Cross(zaxis, xaxis);

	Matrix R;
	DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&R._11), xaxis);
	DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&R._21), yaxis);
	DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&R._31), zaxis);
	R._14 = R._24 = R._34 = 0.f;
	R._41 = _pos.x; R._42 = _pos.y; R._43 = _pos.z;
	R._44 = 1.f;
	return R;
}

inline Matrix Matrix::CreateFromQuaternion(const Quaternion& _quaternion)
{
	Matrix R;
	DirectX::XMVECTOR quatv = DirectX::XMLoadFloat4(&_quaternion);
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixRotationQuaternion(quatv));
	return R;
}

inline Matrix Matrix::CreateFromYawPitchRoll(float _yaw, float _pitch, float _roll)
{
	Matrix R;
	DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll));
	return R;
}

inline Matrix Matrix::Lerp(const Matrix& _M1, const Matrix& _M2, float _t)
{
	DirectX::XMVECTOR x1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_M1._11));
	DirectX::XMVECTOR x2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_M1._21));
	DirectX::XMVECTOR x3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_M1._31));
	DirectX::XMVECTOR x4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_M1._41));

	DirectX::XMVECTOR y1 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_M2._11));
	DirectX::XMVECTOR y2 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_M2._21));
	DirectX::XMVECTOR y3 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_M2._31));
	DirectX::XMVECTOR y4 = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_M2._41));

	x1 = DirectX::XMVectorLerp(x1, y1, _t);
	x2 = DirectX::XMVectorLerp(x2, y2, _t);
	x3 = DirectX::XMVectorLerp(x3, y3, _t);
	x4 = DirectX::XMVectorLerp(x4, y4, _t);

	Matrix R;
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);

	return R;
}

inline Matrix Matrix::Transform(const Matrix& _M, const Quaternion& _rotation)
{
	DirectX::XMVECTOR quatv = DirectX::XMLoadFloat4(&_rotation);

	DirectX::XMMATRIX M0 = DirectX::XMLoadFloat4x4(&_M);
	DirectX::XMMATRIX M1 = DirectX::XMMatrixRotationQuaternion(quatv);

	Matrix R;
	DirectX::XMStoreFloat4x4(&R, XMMatrixMultiply(M0, M1));
	return R;
}

inline Matrix Matrix::Identity()
{
	return DirectX::XMMatrixIdentity();
}

