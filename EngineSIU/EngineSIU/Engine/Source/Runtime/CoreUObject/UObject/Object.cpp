#include "Object.h"
#include "UClass.h"

#include "EngineStatics.h"
#include "UObjectArray.h"


UClass* UObject::StaticClass()
{
    static UClass ClassInfo{TEXT("UObject"), sizeof(UObject), alignof(UObject), nullptr};
    return &ClassInfo;
}

UObject::UObject()
    : UUID(0)
    // TODO: Object를 생성할 때 직접 설정하기
    , InternalIndex(std::numeric_limits<uint32>::max())
    , NamePrivate("None")
{
}

UObject* UObject::Duplicate()
{
    UClass* ClassInfo = GetClass();

    const uint32 Id = UEngineStatics::GenUUID();
    const FString Name = ClassInfo->GetName() + "_" + std::to_string(Id);

    void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(ClassInfo->GetClassSize());
    std::memcpy(RawMemory, this, ClassInfo->GetClassSize());
    std::memset(static_cast<char*>(RawMemory) + 8, 0, ClassInfo->GetClassSize() - 8); // vtable 제외 나머지 메모리 초기화
    UObject* NewObject = static_cast<UObject*>(RawMemory);

    if (!NewObject)
        return nullptr;

    NewObject->ClassPrivate = ClassInfo;
    NewObject->NamePrivate = Name;
    NewObject->UUID = Id;

    GUObjectArray.AddObject(NewObject);
    return NewObject;
}

bool UObject::IsA(const UClass* SomeBase) const
{
    const UClass* ThisClass = GetClass();
    return ThisClass->IsChildOf(SomeBase);
}
