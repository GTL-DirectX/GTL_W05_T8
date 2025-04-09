#include "SceneManager.h"

#include "EditorViewportClient.h"
#include "Container/Array.h"
#include "Container/Map.h"
#include "Engine/EditorEngine.h"
#include "Engine/Engine.h"
#include "Math/Vector.h"
#include "UObject/NameTypes.h"

#include "JSON/json.hpp"
#include "LevelEditor/SLevelEditor.h"

using json = nlohmann::json;


#pragma region nlohmann::json function overload
[[maybe_unused]]
static void to_json(json& Json, const FVector& V)
{
    Json = json{{"X", V.X}, {"Y", V.Y}, {"Z", V.Z}};
}

[[maybe_unused]]
static void to_json(json& Json, const FString& S)
{
    Json = S;
}

[[maybe_unused]]
static void to_json(json& Json, const FName& N)
{
    Json = N.ToString();
}

[[maybe_unused]]
static void from_json(const json& Json, FVector& V)
{
    V.X = Json.at("X").get<float>();
    V.Y = Json.at("Y").get<float>();
    V.Z = Json.at("Z").get<float>();
}

[[maybe_unused]]
static void from_json(const json& Json, FString& S)
{
    if (Json.is_string())
    {
        S = Json.get<FString>();
    }
}

[[maybe_unused]]
static void from_json(const json& Json, FName& N)
{
    if (Json.is_string())
    {
        N = Json.get<FString>();
    }
}
#pragma endregion

namespace SceneManagerData
{
struct FEditorCamaraData
{
    FVector Location;
    FVector Rotation;
    float FOVAngle;
    float NearClipPlane;
    float FarClipPlane;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FEditorCamaraData, Location, Rotation, FOVAngle, NearClipPlane, FarClipPlane)
};

struct FTransformData
{
    FVector Location;
    FVector Rotation;
    FVector Scale;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FTransformData, Location, Rotation)
};

struct FActorData
{
    FString ActorLabel;
    FName Type;
    FTransformData TransformData;
    // TMap<FName, TPair<FString, FString>> Properties;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FActorData, ActorLabel, Type, TransformData)
};

struct FLevelData
{
    FString LevelName;
    TArray<FActorData> Actors;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FLevelData, LevelName, Actors)
};

struct FSceneData
{
    int32 Version;
    FEditorCamaraData EditorCamara;
    TArray<FLevelData> Levels;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FSceneData, Version, EditorCamara, Levels)
};
}


UWorld* FSceneManager::LoadSceneFromJsonFile(const FString& FilePath)
{
    // TODO: Implements This
    return nullptr;
}

bool FSceneManager::SaveSceneToJsonFile(const FString& FilePath, const UWorld* World)
{
    const json Data = JsonToSceneData(FilePath);
    FString JsonString = Data.dump(4); // 4는 들여쓰기 수준

    return true;
}

SceneManagerData::FSceneData FSceneManager::JsonToSceneData(const FString& JsonString)
{
    return {};
}

SceneManagerData::FSceneData FSceneManager::WorldToSceneData(const UWorld* InWorld)
{
    using namespace SceneManagerData;

    // 뷰포트의 카메라 정보를 저장 (현재는 1개만)
#if 1 // TODO: WITH_EDITOR 추가하면 변경하기
    const AEditorPlayer* EdPlayer = Cast<UEditorEngine>(GEngine)->GetEditorPlayer();
    const auto FirstViewport = *GEngineLoop.GetLevelEditor()->GetViewports();
    FEditorCamaraData CameraData =
    {
        .Location = EdPlayer->GetActorLocation(),
        .Rotation = EdPlayer->GetActorRotation(),
        .FOVAngle = FirstViewport->FOVAngle,
        .NearClipPlane = FirstViewport->nearPlane,
        .FarClipPlane = FirstViewport->farPlane,
    };
#else
    FEditorCamaraData CameraData =
    {
        .Location = 0.0f,
        .Rotation = 0.0f,
        .FOVAngle = 0.0f,
        .NearClipPlane = 0.1f,
        .FarClipPlane = 10000.0f
    };
#endif

    // 월드의 레벨 정보를 저장
    const ULevel* ActiveLevel = InWorld->GetActiveLevel();

    TArray<FActorData> ActorsData;
    for (const AActor* Actor : ActiveLevel->Actors)
    {
        // TODO: 나중에 UPROPERTY를 이용해서 적용
        // TMap<FName, TArray<TPair<FString, FString>>> ActorProperties;
        // for (const UActorComponent* Comp : Actor->GetComponents())
        // {
        //     if (USceneComponent* SceneComp = Cast<USceneComponent>(Comp))
        //     {
        //         TArray<TPair<FString, FString>> Properties;
        //         if (Comp->IsA<>())
        //         {
        //             
        //         }
        //         ActorProperties[Comp->GetFName()] = Properties;
        //     }
        // }

        ActorsData.Emplace(
#if 1 // TODO: WITH_EDITOR 추가하면 변경하기
            Actor->GetActorLabel(),
#else
            Actor->GetName(),
#endif
            Actor->GetClass()->GetFName(),
            FTransformData {
                .Location = Actor->GetActorLocation(),
                .Rotation = Actor->GetActorRotation(),
                .Scale = Actor->GetActorScale(),
            }
        );
    }

    return {
        .Version = 1,
        .EditorCamara = CameraData,
        .Levels = {
            {
                .LevelName = ActiveLevel->GetName(),
                .Actors = ActorsData,
            }
        },
    };
}
