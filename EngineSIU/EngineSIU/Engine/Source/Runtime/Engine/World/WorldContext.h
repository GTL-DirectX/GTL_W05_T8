#pragma once
#include "WorldType.h"
#include "UObject/NameTypes.h"

class UWorld;


/**
 * UWorld에 대한 정보를 저장해두는 Context
 */
struct FWorldContext
{
    FWorldContext()
        : WorldType(EWorldType::None)
        , ContextHandle(NAME_None)
        , ThisCurrentWorld(nullptr)
    {
    }

    void SetCurrentWorld(UWorld* InWorld)
    {
        ThisCurrentWorld = InWorld;
    }

    UWorld* World() const
    {
        return ThisCurrentWorld;
    }

public:
    EWorldType WorldType;
    FName ContextHandle;

private:
    UWorld* ThisCurrentWorld;
};
