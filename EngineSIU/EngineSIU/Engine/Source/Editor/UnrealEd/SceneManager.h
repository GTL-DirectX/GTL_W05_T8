#pragma once
#include <memory>

#include "HAL/PlatformType.h"
#include "World/World.h"


namespace SceneManagerData
{
struct FSceneData;
}

class FString;


class FSceneManager
{
public:
    // TODO: AssetManager 만들면 Manager에서 필요한 값을 가져오는걸로 바꾸기

    /**
     * Json형식으로 저장된 World파일을 불러옵니다.
     * @param FilePath Json형식으로 World정보가 저장된 파일의 경로
     * @return 생성된 World
     */
    static UWorld* LoadSceneFromJsonFile(const FString& FilePath);

    /**
     * World를 Json형식으로 저장합니다.
     * @param FilePath World를 저장할 파일 경로
     * @param World 저장할 World
     * @return 성공적으로 저장되었는지 여부
     */
    static bool SaveSceneToJsonFile(const FString& FilePath, const UWorld* World);

private:
    /**
     * JSON 문자열을 역직렬화하여 FSceneData를 생성합니다.
     *
     * @param JsonString 역직렬화할 JSON 문자열
     * @return 역직렬화된 FSceneData 객체
     */
    static SceneManagerData::FSceneData JsonToSceneData(const FString& JsonString);

    /**
     * World를 FSceneData로 직렬화합니다.
     *
     * @param InWorld 직렬화할 World 객체
     * @return World를 표현하는 FSceneData 객체
     */
    static SceneManagerData::FSceneData WorldToSceneData(const UWorld* InWorld);

private:
    // TODO: IFileManager::Get().CreateFileReader() & Writer() 만들면 여기에 추가
    // static void DeserializeFromBinary(const void* Data, int64 Size, SceneManagerData::FSceneData& OutSceneData);
    // or 중간 구조체 사용 안할경우
    // static UWorld* DeserializeFromBinary(const void* Data, int64 Size);

    // static TArray<uint8> SerializeToBinary(const SceneManagerData::FSceneData& SceneData);
    // or 중간 구조체 사용 안할경우
    // static TArray<uint8> SerializeToBinary(UWorld* World);
};
