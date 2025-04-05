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

    /**
     * TODO: 아래 코드는 다중상속인 경우에는 문제가 될 수 있음
     * (Ex: class APawn : public AActor, public IInputSystem)
     * 이러면 AActor에 대한 vtable과, IInputSystem에 대한 vtable이 나뉨
     */
    void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(ClassInfo->GetClassSize());
    if (!RawMemory)
    {
        return nullptr;
    }

    std::memcpy(RawMemory, this, ClassInfo->GetClassSize());
    std::memset(static_cast<char*>(RawMemory) + sizeof(void*), 0, ClassInfo->GetClassSize() - sizeof(void*)); // vtable 제외 나머지 메모리 초기화
    UObject* NewObject = static_cast<UObject*>(RawMemory);

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
