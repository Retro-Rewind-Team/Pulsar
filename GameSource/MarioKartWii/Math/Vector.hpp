#ifndef _GAME_VECTOR_
#define _GAME_VECTOR_

#include <core/egg/Math/Vector.hpp>
#include <core/egg/Math/Quat.hpp>

typedef EGG::Quatf Quat;
struct Mtx34;

struct Vec3 : EGG::Vector3f {
    Vec3() {};  // 805a1fb0
    ~Vec3() {};  // 80513600
    Vec3(float x, float y, float z) : EGG::Vector3f(x, y, z) {}
    Vec3 operator-();  // 805147d4 unary -, negates the vec
    static Vec3 RotateQuaternion(const Quat& rot, const Vec3& point);  // 805147fc
    static Vec3 RotateQuaternionConjugate(const Quat& rot, const Vec3& point);  // 80574c7c
    Vec3 operator+(const Vec3& rhs);  // 80514838
    Vec3 operator-(const Vec3& rhs);  // 8051486c
    static Vec3 Cross(const Vec3& left, const Vec3& right);  // 80537830 probably part of another class
    static Vec3 MultiplyByScalar(float scalar, const Vec3& src);  // 80514810
    static Vec3 MultiplyByScalar2(float scalar, const Vec3& src);  // 80518ab8

    static Vec3 DivideByScalar(float scalar, const Vec3& src);  // 806a62a4
    static Vec3 EulerFromMatrix(const EGG::Matrix34f& src);  // 8067eaec
    static Vec3 MultMtxByVec(const Mtx34& mtx, const Vec3& vec);  // 8059a4f8
    void CalcYAxisRotMtx(Mtx34& dest) const;  // 806b3ca4 rotMtx with axis (x, 0, z), so xz rotation
    static Vec3 ProjectVecOntoUnit(const Vec3& vecToProj, const Vec3& unitVector);  // 805aeb88
    static Vec3 RejectVecOntoUnit(const Vec3& vecToProj, const Vec3& unitVector);  // 805aebd0
    static void ProjectAndRejectVecOntoUnit(Vec3& projDest, Vec3& rejDest, const Vec3& vecToProj, const Vec3& unitVector);  // 805aec24
};

struct Vec2 : EGG::Vector2f {
    Vec2() {}
    Vec2(float x, float z) : EGG::Vector2f(x, z) {}
    Vec2 operator+(const Vec2& rhs) const;  // 80512040
    Vec2 operator-(const Vec2& rhs) const;  // 8051201c
    static Vec2 MultiplyByScalar2(float scalar, const Vec3& src);  // 8054be10
};

#endif