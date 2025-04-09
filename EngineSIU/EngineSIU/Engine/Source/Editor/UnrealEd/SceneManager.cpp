#include "SceneManager.h"
#include "Container/Array.h"
#include "Container/Map.h"
#include "Math/Vector.h"
#include "UObject/NameTypes.h"

#include "JSON/json.hpp"

using json = nlohmann::json;


namespace SceneManagerData
{
struct FEditorCamaraData
{
    FVector Location;
    FVector Rotation;
    float FOVAngle;
    float NearClipPlane;
    float FarClipPlane;
};

struct FTransformData
{
    FVector Location;
    FVector Rotation;
    FVector Scale;
};

struct FActorData
{
    FName Type;
    FTransformData TransformData;
    FString ActorLabel;
};

struct FLevelData
{
    FString LevelName;
    TArray<FActorData> Actors;
    TMap<FName, TPair<FString, FString>> Properties;
};

struct FSceneData
{
    int32 Version;
    FEditorCamaraData EditorCamara;
    TArray<FLevelData> Levels;
};
}