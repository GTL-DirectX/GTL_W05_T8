#include "Level.h"
#include "UObject/Casts.h"
#include "GameFramework/Actor.h"

UObject* ULevel::Duplicate()
{
    ULevel* NewLevel = Cast<ULevel>(Super::Duplicate());

    if (!NewLevel)
        return nullptr;

    for (AActor* Actor : Actors)
    {
        AActor* NewActor = Cast<AActor>(Actor->Duplicate());
        if (NewActor)
        {
            NewLevel->Actors.Add(NewActor);
        }
    }
    
    NewLevel->OwningWorld = OwningWorld;
    return NewLevel;
}
