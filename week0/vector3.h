#pragma once

struct FVector3 {
    float x, y, z;
    FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    // ���� ���� ������
    FVector3 operator+(const FVector3& Other) const {
        return FVector3(x + Other.x, y + Other.y, z + Other.z);
    }

    // ���� ���� ������
    FVector3 operator-(const FVector3& Other) const {
        return FVector3(x - Other.x, y - Other.y, z - Other.z);
    }

    // ���� ��Į�� �� ������ (������ ��Į�� ��)
    FVector3 operator*(float Scalar) const {
        return FVector3(x * Scalar, y * Scalar, z * Scalar);
    }

    // ���� ��Į�� ������ ������
    FVector3 operator/(float Scalar) const {
        return FVector3(x / Scalar, y / Scalar, z / Scalar);
    }

    // ���� ���� �� ���� ������
    FVector3& operator+=(const FVector3& Other) {
        x += Other.x;
        y += Other.y;
        z += Other.z;
        return *this;
    }

    // ���� ���� �� ���� ������
    FVector3& operator-=(const FVector3& Other) {
        x -= Other.x;
        y -= Other.y;
        z -= Other.z;
        return *this;
    }

    // ���� ��Į�� �� �� ���� ������
    FVector3& operator*=(float Scalar) {
        x *= Scalar;
        y *= Scalar;
        z *= Scalar;
        return *this;
    }

    // ���� ��Į�� ������ �� ���� ������
    FVector3& operator/=(float Scalar) {
        x /= Scalar;
        y /= Scalar;
        z /= Scalar;
        return *this;
    }
};