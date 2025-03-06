#pragma once

struct FVector3 {
    float x, y, z;
    FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    // 벡터 덧셈 연산자
    FVector3 operator+(const FVector3& Other) const {
        return FVector3(x + Other.x, y + Other.y, z + Other.z);
    }

    // 벡터 뺄셈 연산자
    FVector3 operator-(const FVector3& Other) const {
        return FVector3(x - Other.x, y - Other.y, z - Other.z);
    }

    // 벡터 스칼라 곱 연산자 (오른쪽 스칼라 곱)
    FVector3 operator*(float Scalar) const {
        return FVector3(x * Scalar, y * Scalar, z * Scalar);
    }

    // 벡터 스칼라 나누기 연산자
    FVector3 operator/(float Scalar) const {
        return FVector3(x / Scalar, y / Scalar, z / Scalar);
    }

    // 벡터 덧셈 후 대입 연산자
    FVector3& operator+=(const FVector3& Other) {
        x += Other.x;
        y += Other.y;
        z += Other.z;
        return *this;
    }

    // 벡터 뺄셈 후 대입 연산자
    FVector3& operator-=(const FVector3& Other) {
        x -= Other.x;
        y -= Other.y;
        z -= Other.z;
        return *this;
    }

    // 벡터 스칼라 곱 후 대입 연산자
    FVector3& operator*=(float Scalar) {
        x *= Scalar;
        y *= Scalar;
        z *= Scalar;
        return *this;
    }

    // 벡터 스칼라 나누기 후 대입 연산자
    FVector3& operator/=(float Scalar) {
        x /= Scalar;
        y /= Scalar;
        z /= Scalar;
        return *this;
    }
};