﻿#pragma once

struct FVector;
struct FVector4;

// 4x4 행렬 연산
struct alignas(16) FMatrix
{
public:
    alignas(16) float M[4][4];

public:
    static const FMatrix Identity;

public:
    // 기본 연산자 오버로딩
    FMatrix operator+(const FMatrix& Other) const;
    FMatrix operator-(const FMatrix& Other) const;
    FMatrix operator*(const FMatrix& Other) const;
    FMatrix operator*(float Scalar) const;
    FMatrix operator/(float Scalar) const;
    float* operator[](int row);
    const float* operator[](int row) const;

    // 유틸리티 함수
    static FMatrix Transpose(const FMatrix& Mat);
    static FMatrix Inverse(const FMatrix& Mat);
    static FMatrix CreateRotation(float roll, float pitch, float yaw);
    static FMatrix CreateScale(float scaleX, float scaleY, float scaleZ);
    static FVector TransformVector(const FVector& v, const FMatrix& m);
    static FVector4 TransformVector(const FVector4& v, const FMatrix& m);
    static FMatrix CreateTranslationMatrix(const FVector& position);

    FVector4 TransformFVector4(const FVector4& vector) const;
    FVector TransformPosition(const FVector& vector) const;
};
