#include "Actor.h"
#include "World/World.h"


UObject* AActor::Duplicate()
{
    ThisClass* NewActor = Cast<ThisClass>(Super::Duplicate());

    NewActor->Owner = Owner;

    // 기본적으로 있던 컴포넌트 제거
    TSet CopiedComponents = NewActor->OwnedComponents;
    for (UActorComponent* Components : CopiedComponents)
    {
        Components->DestroyComponent();
    }
    NewActor->OwnedComponents.Empty();


    // 부모-자식 관계 저장용 맵
    TMap<USceneComponent*, USceneComponent*> ParentChildMap;

    // 컴포넌트 복제 및 부모-자식 관계 추적
    TMap<USceneComponent*, USceneComponent*> OriginalToDuplicateMap; // 원본 -> 복제된 컴포넌트 매핑

    for (UActorComponent* Component : OwnedComponents)
    {
        UActorComponent* NewComponent = Cast<UActorComponent>(Component->Duplicate());
        NewComponent->OwnerPrivate = NewActor;
        NewActor->OwnedComponents.Add(NewComponent);

        // RootComponent 설정
        if (RootComponent == Component)
        {
            NewActor->RootComponent = static_cast<USceneComponent*>(NewComponent);
        }

        // 부모-자식 관계 저장 (USceneComponent만 해당)
        if (USceneComponent* SceneComp = Cast<USceneComponent>(Component))
        {
            OriginalToDuplicateMap.Add(SceneComp, static_cast<USceneComponent*>(NewComponent));
            if (USceneComponent* ParentComp = SceneComp->GetAttachParent())
            {
                ParentChildMap.Add(static_cast<USceneComponent*>(NewComponent), ParentComp);
            }
        }

        // 컴포넌트 초기화
        NewComponent->InitializeComponent();
    }

    // 부모-자식 관계 복원
    for (const auto& [ChildComp, OriginalParentComp] : ParentChildMap)
    {
        // 복제된 부모를 찾아 설정
        if (USceneComponent** NewParentComp = OriginalToDuplicateMap.Find(OriginalParentComp))
        {
            ChildComp->AttachToComponent(*NewParentComp);
        }
    }

    return NewActor;
}

void AActor::BeginPlay()
{
    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Comp : CopyComponents)
    {
        Comp->BeginPlay();
    }
}

void AActor::Tick(float DeltaTime)
{
    // TODO: 임시로 Actor에서 Tick 돌리기
    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Comp : CopyComponents)
    {
        Comp->TickComponent(DeltaTime);
    }
}

void AActor::Destroyed()
{
    // Actor가 제거되었을 때 호출하는 EndPlay
    EndPlay(EEndPlayReason::Destroyed);
}

void AActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 본인이 소유하고 있는 모든 컴포넌트의 EndPlay 호출
    for (UActorComponent* Component : GetComponents())
    {
        if (Component->HasBegunPlay())
        {
            Component->EndPlay(EndPlayReason);
        }
    }
    UninitializeComponents();
}

bool AActor::Destroy()
{
    if (!IsActorBeingDestroyed())
    {
        if (UWorld* World = GetWorld())
        {
            World->DestroyActor(this);
            bActorIsBeingDestroyed = true;
        }
    }

    return IsActorBeingDestroyed();
}

void AActor::RemoveOwnedComponent(UActorComponent* Component)
{
    OwnedComponents.Remove(Component);
}

void AActor::InitializeComponents()
{
    TSet<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComp : Components)
    {
        if (ActorComp->bAutoActive && !ActorComp->IsActive())
        {
            ActorComp->Activate();
        }

        if (!ActorComp->HasBeenInitialized())
        {
            ActorComp->InitializeComponent();
        }
    }
}

void AActor::UninitializeComponents()
{
    TSet<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComp : Components)
    {
        if (ActorComp->HasBeenInitialized())
        {
            ActorComp->UninitializeComponent();
        }
    }
}

bool AActor::SetRootComponent(USceneComponent* NewRootComponent)
{
    if (NewRootComponent == nullptr || NewRootComponent->GetOwner() == this)
    {
        if (RootComponent != NewRootComponent)
        {
            USceneComponent* OldRootComponent = RootComponent;
            RootComponent = NewRootComponent;

            if (OldRootComponent)
            {
                OldRootComponent->SetupAttachment(RootComponent);
            }
        }
        return true;
    }
    return false;
}

bool AActor::SetActorLocation(const FVector& NewLocation)
{
    if (RootComponent)
    {
       RootComponent->SetLocation(NewLocation);
        return true;
    }
    return false;
}

bool AActor::SetActorRotation(const FVector& NewRotation)
{
    if (RootComponent)
    {
        RootComponent->SetRotation(NewRotation);
        return true;
    }
    return false;
}

bool AActor::SetActorScale(const FVector& NewScale)
{
    if (RootComponent)
    {
        RootComponent->SetScale(NewScale);
        return true;
    }
    return false;
}
