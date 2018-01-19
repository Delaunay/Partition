#include <Utilities/Logging.h>
#include <Utilities/Math.h>

#include <utility>

#include <DirectXMath.h>

namespace pn {

#pragma region Constants

const float EPSILON = 0.00001f;

const float PI     = 3.14159265359f;
const float TWOPI  = 2 * PI;
const float PIDIV2 = PI / 2;
const float PIDIV4 = PI / 4;

const vec2f vec2f::Zero  = vec2f(0.0f, 0.0f);
const vec2f vec2f::One   = vec2f(1.0f, 1.0f);
const vec2f vec2f::UnitX = vec2f(1.0f, 0.0f);
const vec2f vec2f::UnitY = vec2f(0.0f, 1.0f);

const vec3f vec3f::Zero  = vec3f(0.0f, 0.0f, 0.0f);
const vec3f vec3f::One   = vec3f(1.0f, 1.0f, 1.0f);
const vec3f vec3f::UnitX = vec3f(1.0f, 0.0f, 0.0f);
const vec3f vec3f::UnitY = vec3f(0.0f, 1.0f, 0.0f);
const vec3f vec3f::UnitZ = vec3f(0.0f, 0.0f, 1.0f);

const vec4f vec4f::Zero  = vec4f(0.0f, 0.0f, 0.0f, 0.0f);
const vec4f vec4f::One   = vec4f(1.0f, 1.0f, 1.0f, 1.0f);
const vec4f vec4f::UnitX = vec4f(1.0f, 0.0f, 0.0f, 0.0f);
const vec4f vec4f::UnitY = vec4f(0.0f, 1.0f, 0.0f, 0.0f);
const vec4f vec4f::UnitZ = vec4f(0.0f, 0.0f, 1.0f, 0.0f);
const vec4f vec4f::UnitW = vec4f(0.0f, 0.0f, 0.0f, 1.0f);

const quaternion quaternion::Zero     = quaternion(0.0f, 0.0f, 0.0f, 0.0f);
const quaternion quaternion::Identity = quaternion(0.0f, 0.0f, 0.0f, 1.0f);

// clang-format off
const mat4f mat4f::Identity = mat4f(
	1.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 1.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 1.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 1.0f);

const mat4f mat4f::Zero = mat4f(
	0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f);

const mat4f mat4f::One  = mat4f(
	1.0f, 1.0f, 1.0f, 1.0f, 
	1.0f, 1.0f, 1.0f, 1.0f, 
	1.0f, 1.0f, 1.0f, 1.0f, 
	1.0f, 1.0f, 1.0f, 1.0f);
// clang-format on
#pragma endregion

#pragma region Matrix Functions

mat4f Transpose(const mat4f &m) {
    // clang-format off
    return mat4f(
		m._00, m._10, m._20, m._30, 
		m._01, m._11, m._21, m._31, 
		m._02, m._12, m._22, m._32,
        m._03, m._13, m._23, m._33);
    // clang-format on
}
mat4f Inverse(const mat4f &in) {
    // from
    // https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform
    mat4f m;
    float s0 = in._00 * in._11 - in._10 * in._01;
    float s1 = in._00 * in._12 - in._10 * in._02;
    float s2 = in._00 * in._13 - in._10 * in._03;
    float s3 = in._01 * in._12 - in._11 * in._02;
    float s4 = in._01 * in._13 - in._11 * in._03;
    float s5 = in._02 * in._13 - in._12 * in._03;

    float c5 = in._22 * in._33 - in._32 * in._23;
    float c4 = in._21 * in._33 - in._31 * in._23;
    float c3 = in._21 * in._32 - in._31 * in._22;
    float c2 = in._20 * in._33 - in._30 * in._23;
    float c1 = in._20 * in._32 - in._30 * in._22;
    float c0 = in._20 * in._31 - in._30 * in._21;

    // Should check for 0 determinant
    // You can do (in + Identidy * eps) to make sure the matrix is always invertible

    float invdet = 1 / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

    m._00 = (in._11 * c5 - in._12 * c4 + in._13 * c3) * invdet;
    m._01 = (-in._01 * c5 + in._02 * c4 - in._03 * c3) * invdet;
    m._02 = (in._31 * s5 - in._32 * s4 + in._33 * s3) * invdet;
    m._03 = (-in._21 * s5 + in._22 * s4 - in._23 * s3) * invdet;

    m._10 = (-in._10 * c5 + in._12 * c2 - in._13 * c1) * invdet;
    m._11 = (in._00 * c5 - in._02 * c2 + in._03 * c1) * invdet;
    m._12 = (-in._30 * s5 + in._32 * s2 - in._33 * s1) * invdet;
    m._13 = (in._20 * s5 - in._22 * s2 + in._23 * s1) * invdet;

    m._20 = (in._10 * c4 - in._11 * c2 + in._13 * c0) * invdet;
    m._21 = (-in._00 * c4 + in._01 * c2 - in._03 * c0) * invdet;
    m._22 = (in._30 * s4 - in._31 * s2 + in._33 * s0) * invdet;
    m._23 = (-in._20 * s4 + in._21 * s2 - in._23 * s0) * invdet;

    m._30 = (-in._10 * c3 + in._11 * c1 - in._12 * c0) * invdet;
    m._31 = (in._00 * c3 - in._01 * c1 + in._02 * c0) * invdet;
    m._32 = (-in._30 * s3 + in._31 * s1 - in._32 * s0) * invdet;
    m._33 = (in._20 * s3 - in._21 * s1 + in._22 * s0) * invdet;

    return m;
}

mat4f Translation(const vec3f &translation) {
    return Translation(translation.x, translation.y, translation.z);
}
mat4f Translation(const float xt, const float yt, const float zt) {
    // clang-format off
	return mat4f(
		1.0f, 0.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 0.0f, 
		0.0f, 0.0f, 1.0f, 0.0f, 
		xt,     yt,   zt, 1.0f);
    // clang-format on
}

mat4f Scale(const float scale) { return Scale(scale, scale, scale); }
mat4f Scale(const vec3f &scale) { return Scale(scale.x, scale.y, scale.z); }
mat4f Scale(const float xs, const float ys, const float zs) {
    // clang-format off
	return mat4f(
		  xs, 0.0f, 0.0f, 0.0f, 
		0.0f,   ys, 0.0f, 0.0f, 
		0.0f, 0.0f,   zs, 0.0f, 
		0.0f, 0.0f, 0.0f, 1.0f);
    // clang-format on
}

mat4f RotationX(const float rad) {
    const float cos_t = cosf(rad);
    const float sin_t = sinf(rad);
    // clang-format off
    return mat4f(
		1.0f,   0.0f,  0.0f, 0.0f, 
		0.0f,  cos_t, sin_t, 0.0f, 
		0.0f, -sin_t, cos_t, 0.0f, 
		0.0f,   0.0f,  0.0f, 1.0f);
    // clang-format on
}
mat4f RotationY(const float rad) {
    const float cos_t = cosf(rad);
    const float sin_t = sinf(rad);
    // clang-format off
    return mat4f(
		cos_t, 0.0f, -sin_t, 0.0f, 
		 0.0f, 1.0f,   0.0f, 0.0f, 
		sin_t, 0.0f,  cos_t, 0.0f,
		 0.0f, 0.0f,   0.0f, 1.0f);
    // clang-format on
}
mat4f RotationZ(const float rad) {
    const float cos_t = cosf(rad);
    const float sin_t = sinf(rad);
    // clang-format off
    return mat4f(
		 cos_t, sin_t, 0.0f, 0.0f, 
		-sin_t, cos_t, 0.0f, 0.0f,
		  0.0f,  0.0f, 1.0f, 0.0f, 
	   	  0.0f,  0.0f, 0.0f, 1.0f);
    // clang-format on
}

mat4f SRTMatrix(const vec3f &scale, const vec3f &euler_angles, const vec3f &translation) {
    return SRTMatrix(scale, EulerToQuaternion(euler_angles), translation);
}
mat4f SRTMatrix(const vec3f &scale, const quaternion &rotation, const vec3f &translation) {
    return Scale(scale) * QuaternionToRotationMatrix(rotation) * Translation(translation);
}

/*
vec3f		GetTranslation(const mat4f& m) {
        return vec3f(m._30, m._31, m._32);
}
vec3f		GetScale(const mat4f& m) {
        const float xs = Length(vec3f(m._00, m._01, m._02));
        const float ys = Length(vec3f(m._10, m._11, m._12));
        const float zs = Length(vec3f(m._20, m._21, m._22));
        return vec3f(xs, ys, zs);
}
vec3f		GetRotation(const mat4f& m) {
        const vec3f scale = GetScale(m);
        auto ortho_m = Scale(Reciprocal(scale)) * m;
        const float xr = atan2f(ortho_m._21, ortho_m._22);
        const float yr = atan2f(-ortho_m._20, Length(vec2f(ortho_m._21, ortho_m._22)));
        const float zr = atan2f(ortho_m._10, ortho_m._00);
        return vec3f(xr, yr, zr);
}
void		Decompose(const mat4f& m, vec3f& translation, vec3f& rotation, vec3f& scale) {
        translation = GetTranslation(m);
        rotation	= GetRotation(m);
        scale		= GetScale(m);
}*/

mat4f EulerToRotationMatrix(const vec3f &euler) {
    return EulerToRotationMatrix(euler.x, euler.y, euler.z);
}
mat4f EulerToRotationMatrix(const float pitch, const float yaw, const float roll) {
    const float cy = cosf(yaw);
    const float sy = sinf(yaw);

    const float cp = cosf(pitch);
    const float sp = sinf(pitch);

    const float cr = cosf(roll);
    const float sr = sinf(roll);

    const float sy_sp = sy * sp;
    const float sy_cp = sy * cp;
    const float sr_cp = sr * cp;
    const float cr_cp = cr * cp;

    // clang-format off
    return Transpose(mat4f(
		cr * cy - sr * sy_sp,      -sr_cp, sr * sp * cy + cr * sy, 0.0f,
        sr * cy + cr * sy_sp,       cr_cp, sr * sy - cr * cy * sp, 0.0f,
		            -cp * sy,          sp,                cp * cy, 0.0f,
		                0.0f,        0.0f,                   0.0f, 1.0f));
    // clang-format on
}
mat4f AxisAngleToRotationMatrix(const vec3f &axis, const float angle) {
    return QuaternionToRotationMatrix(AxisAngleToQuaternion(axis, angle));
}

mat4f PerspectiveFov(const float fov, const float aspect_ratio, const float near_z,
                     const float far_z) {
    const float a1 = 1 / tanf(fov / 2.0f);
    const float d  = far_z - near_z;
    // clang-format off
    return mat4f(
		a1 / aspect_ratio, 0.0,                   0.0, 0.0, 
		              0.0,  a1,                   0.0, 0.0, 
		              0.0, 0.0,             far_z / d, 1.0, 
		              0.0, 0.0, -(far_z * near_z) / d, 0.0);
    // clang-format on
}
mat4f Orthographic(const float width, const float height, const float near_z, const float far_z) {
    const float d = far_z - near_z;
    // clang-format off
    return mat4f(
		2.0f / width,          0.0f,        0.0f, 0.0f, 
	          	0.0f, 2.0f / height,        0.0f, 0.0f, 
	        	0.0f,          0.0f,       1 / d, 0.0f, 
		        0.0f,          0.0f, -near_z / d, 1.0f);
    // clang-format on
}

mat4f FromCoordinateSystem(const vec3f &origin, const vec3f &forward, const vec3f &up) {
    vec3f z = Normalize(forward);
    vec3f x = Normalize(Cross(up, z));
    vec3f y = Normalize(Cross(z, x));
    return mat4f(x, y, z, origin);
}
mat4f ToCoordinateSystem(const vec3f &origin, const vec3f &forward, const vec3f &up) {
    return Inverse(FromCoordinateSystem(origin, forward, up));
}
mat4f LookAt(const vec3f &position, const vec3f &target, const vec3f &up) {
    return ToCoordinateSystem(position, target - position, up);
}

#pragma endregion

#pragma region Quaternion Functions

mat4f QuaternionToRotationMatrix(const quaternion &q) {
    const float sx = q.x * q.x;
    const float sy = q.y * q.y;
    const float sz = q.z * q.z;

    const float x_z = q.x * q.z;
    const float x_y = q.x * q.y;
    const float x_w = q.x * q.w;

    const float y_z = q.y * q.z;
    const float y_w = q.y * q.w;

    const float z_w = q.z * q.w;

    // clang-format off
    return mat4f(
		1 - 2 * (sy + sz),   2 * (x_y + z_w),   2 * (x_z - y_w), 0.0f, 
		  2 * (x_y - z_w), 1 - 2 * (sx + sz),   2 * (y_z + x_w), 0.0f, 
	   	  2 * (x_z + y_w),   2 * (y_z - x_w), 1 - 2 * (sx + sy), 0.0f, 
		             0.0f,              0.0f,              0.0f, 1.0f);
    // clang-format on
}
quaternion RotationMatrixToQuaternion(const mat4f &m) {
    const float trace = m._00 + m._11 + m._22;
    quaternion  q;
    if(trace > 0) {
	float s = 0.5f / sqrtf(trace + 1.0f);
	q.w     = 0.25f / s;
	q.x     = (m._21 - m._12) * s;
	q.y     = (m._02 - m._20) * s;
	q.z     = (m._10 - m._01) * s;
    } else {
	if(m._00 > m._11 && m._00 > m._22) {
	    float s = 2.0f * sqrtf(1.0f + m._00 - m._11 - m._22);
	    q.w     = (m._21 - m._12) / s;
	    q.x     = 0.25f * s;
	    q.y     = (m._01 + m._10) / s;
	    q.z     = (m._02 + m._20) / s;
	} else if(m._11 > m._22) {
	    float s = 2.0f * sqrtf(1.0f + m._11 - m._00 - m._22);
	    q.w     = (m._02 - m._20) / s;
	    q.x     = (m._01 + m._10) / s;
	    q.y     = 0.25f * s;
	    q.z     = (m._12 + m._21) / s;
	} else {
	    float s = 2.0f * sqrtf(1.0f + m._22 - m._00 - m._11);
	    q.w     = (m._10 - m._01) / s;
	    q.x     = (m._02 + m._20) / s;
	    q.y     = (m._12 + m._21) / s;
	    q.z     = 0.25f * s;
	}
    }
    return Normalize(q);
}

#pragma endregion

#pragma region Equality Testing Functions

bool IsEqual(const vec2f &v1, const vec2f &v2, const float eps) {
    return (abs(v1.x - v2.x) <= EPSILON) && (abs(v1.y - v2.y) <= EPSILON);
}
bool IsEqual(const vec3f &v1, const vec3f &v2, const float eps) {
    return (abs(v1.x - v2.x) <= EPSILON) && (abs(v1.y - v2.y) <= EPSILON) &&
           (abs(v1.z - v2.z) <= EPSILON);
}
bool IsEqual(const vec4f &v1, const vec4f &v2, const float eps) {
    return (abs(v1.x - v2.x) <= EPSILON) && (abs(v1.y - v2.y) <= EPSILON) &&
           (abs(v1.z - v2.z) <= EPSILON) && (abs(v1.w - v2.w) <= EPSILON);
}
bool IsEqual(const quaternion &v1, const quaternion &v2, const float eps) {
    return (abs(v1.x - v2.x) <= EPSILON) && (abs(v1.y - v2.y) <= EPSILON) &&
           (abs(v1.z - v2.z) <= EPSILON) && (abs(v1.w - v2.w) <= EPSILON);
}
#define E(x, y, e) (abs(x - y) <= e)
bool IsEqual(const mat4f &v1, const mat4f &v2, const float eps) {
    return E(v1._00, v2._00, eps) && E(v1._01, v2._01, eps) && E(v1._02, v2._02, eps) &&
           E(v1._03, v2._03, eps) && E(v1._10, v2._10, eps) && E(v1._11, v2._11, eps) &&
           E(v1._12, v2._12, eps) && E(v1._13, v2._13, eps) && E(v1._20, v2._20, eps) &&
           E(v1._21, v2._21, eps) && E(v1._22, v2._22, eps) && E(v1._23, v2._23, eps) &&
           E(v1._30, v2._30, eps) && E(v1._31, v2._31, eps) && E(v1._32, v2._32, eps) &&
           E(v1._33, v2._33, eps);
}
bool IsEqual(const mat4f &v1, const DirectX::XMMATRIX &v2, const float eps) {
    return E(v1._00, v2.r[0].m128_f32[0], eps) && E(v1._01, v2.r[0].m128_f32[1], eps) &&
           E(v1._02, v2.r[0].m128_f32[2], eps) && E(v1._03, v2.r[0].m128_f32[3], eps) &&
           E(v1._10, v2.r[1].m128_f32[0], eps) && E(v1._11, v2.r[1].m128_f32[1], eps) &&
           E(v1._12, v2.r[1].m128_f32[2], eps) && E(v1._13, v2.r[1].m128_f32[3], eps) &&
           E(v1._20, v2.r[2].m128_f32[0], eps) && E(v1._21, v2.r[2].m128_f32[1], eps) &&
           E(v1._22, v2.r[2].m128_f32[2], eps) && E(v1._23, v2.r[2].m128_f32[3], eps) &&
           E(v1._30, v2.r[3].m128_f32[0], eps) && E(v1._31, v2.r[3].m128_f32[1], eps) &&
           E(v1._32, v2.r[3].m128_f32[2], eps) && E(v1._33, v2.r[3].m128_f32[3], eps);
}
#undef E

#pragma endregion

#pragma region Vector Functions

vec3f Cross(const vec3f &u, const vec3f &v) {
    vec3f result;
    result.x = (u.y * v.z) - (u.z * v.y);
    result.y = (u.z * v.x) - (u.x * v.z);
    result.z = (u.x * v.y) - (u.y * v.x);
    return result;
}

#pragma endregion

#pragma region Utility Functions

#define OP(x, m1, m2) ((x < m1) ? m1 : (x > m2) ? m2 : x)
float Clamp(const float u, const float min, const float max) { return OP(u, min, max); }
vec2f Clamp(const vec2f &u, const vec2f &min, const vec2f &max) {
    vec2f result;
    result.x = OP(u.x, min.x, max.x);
    result.y = OP(u.y, min.y, max.y);
    return result;
}
vec3f Clamp(const vec3f &u, const vec3f &min, const vec3f &max) {
    vec3f result;
    result.x = OP(u.x, min.x, max.x);
    result.y = OP(u.y, min.y, max.y);
    result.z = OP(u.z, min.z, max.z);
    return result;
}
vec4f Clamp(const vec4f &u, const vec4f &min, const vec4f &max) {
    vec4f result;
    result.x = OP(u.x, min.x, max.x);
    result.y = OP(u.y, min.y, max.y);
    result.z = OP(u.z, min.z, max.z);
    result.z = OP(u.w, min.w, max.w);
    return result;
}
#undef OP

#define OP(x, y) ((x < y) ? x : y)
vec2f Min(const vec2f &u, const vec2f &v) {
    vec2f result;
    result.x = OP(u.x, v.x);
    result.y = OP(u.y, v.y);
    return result;
}
vec3f Min(const vec3f &u, const vec3f &v) {
    vec3f result;
    result.x = OP(u.x, v.x);
    result.y = OP(u.y, v.y);
    result.z = OP(u.z, v.z);
    return result;
}
vec4f Min(const vec4f &u, const vec4f &v) {
    vec4f result;
    result.x = OP(u.x, v.x);
    result.y = OP(u.y, v.y);
    result.z = OP(u.z, v.z);
    result.w = OP(u.w, v.w);
    return result;
}
#undef OP

#define OP(x, y) ((x > y) ? x : y)
vec2f Max(const vec2f &u, const vec2f &v) {
    vec2f result;
    result.x = OP(u.x, v.x);
    result.y = OP(u.y, v.y);
    return result;
}
vec3f Max(const vec3f &u, const vec3f &v) {
    vec3f result;
    result.x = OP(u.x, v.x);
    result.y = OP(u.y, v.y);
    result.z = OP(u.z, v.z);
    return result;
}
vec4f Max(const vec4f &u, const vec4f &v) {
    vec4f result;
    result.x = OP(u.x, v.x);
    result.y = OP(u.y, v.y);
    result.z = OP(u.z, v.z);
    result.w = OP(u.w, v.w);
    return result;
}
#undef OP

template <> float SmoothStep(const float &edge0, const float &edge1, const float &v) {
    float t = Clamp((v - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

#pragma endregion

} // namespace pn