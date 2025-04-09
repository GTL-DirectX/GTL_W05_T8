#pragma once
#include "Vector.h"
#include "Serialization/Archive.h"


// 4D Vector
struct FVector4
{
    float X, Y, Z, W;

    FVector4() : X(0), Y(0), Z(0), W(0) {}
    FVector4(float InX, float InY, float InZ, float InW)
        : X(InX), Y(InY), Z(InZ), W(InW)
    {}
    FVector4(FVector InVector, float InW = 0)
        : X(InVector.X), Y(InVector.Y), Z(InVector.Z)
        , W(InW)
    {}

    FVector4 operator+(const FVector4& Other) const;
    FVector4 operator-(const FVector4& Other) const;

    FVector4 operator/(float Scalar) const;

    FString ToString() const;
};

inline FVector4 FVector4::operator-(const FVector4& Other) const
{
    return {
        X - Other.X,
        Y - Other.Y,
        Z - Other.Z,
        W - Other.W
    };
}

inline FVector4 FVector4::operator+(const FVector4& Other) const
{
    return {
        X + Other.X,
        Y + Other.Y,
        Z + Other.Z,
        W + Other.W
    };
}

inline FVector4 FVector4::operator/(float Scalar) const
{
    return {
        X / Scalar,
        Y / Scalar,
        Z / Scalar,
        W / Scalar
    };
}

inline FString FVector4::ToString() const
{
    return FString::Printf(TEXT("X=%3.3f Y=%3.3f Z=%3.3f W=%3.3f"), X, Y, Z, W);
}

inline FArchive& operator<<(FArchive& Ar, FVector4& V)
{
    return Ar << V.X << V.Y << V.Z << V.W;
}
