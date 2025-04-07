#pragma once
#include "EngineStatics.h"
#include "Object.h"
#include "UClass.h"
#include "UObjectArray.h"

class FObjectFactory
{
public:
    static UObject* ConstructObject(UClass* InClass)
    {
        const uint32 Id = UEngineStatics::GenUUID();
        const FString Name = InClass->GetName() + "_" + std::to_string(Id);

        UObject* Obj = InClass->ClassCTOR();
        Obj->ClassPrivate = InClass;
        Obj->NamePrivate = Name;
        Obj->UUID = Id;

        GUObjectArray.AddObject(Obj);

        UE_LOG(LogLevel::Display, "Created New Object : %s", *Name);
        return Obj;
    }

    template<typename T>
        requires std::derived_from<T, UObject>
    static T* ConstructObject()
    {
        return static_cast<T*>(ConstructObject(T::StaticClass()));
    }
};
