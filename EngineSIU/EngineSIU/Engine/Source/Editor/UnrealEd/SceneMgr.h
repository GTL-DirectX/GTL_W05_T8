#pragma once
#include "Container/Map.h"

class UObject;

struct _SceneData
{
    int32 Version;
    int32 NextUUID;
    TMap<int32, UObject*> Primitives;
    TMap<int32, UObject*> Cameras;
};

class FSceneMgr
{
public:
    [[deprecated]]
    static _SceneData ParseSceneData(const FString& jsonStr);
    [[deprecated]]
    static FString LoadSceneFromFile(const FString& filename);
    static std::string SerializeSceneData(const _SceneData& sceneData);
    [[deprecated]]
    static bool SaveSceneToFile(const FString& filename, const _SceneData& sceneData);
};
