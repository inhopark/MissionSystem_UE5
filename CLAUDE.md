# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

간단한 NPC 기반 미션(퀘스트) 시스템을 보여주는 Unreal Engine 5.5 C++ 프로젝트(`MissionSystem`)입니다. README에 따르면 완성된 게임이 아니라, 깔끔하고 읽기 쉬운 코드 스타일을 보여주는 것이 목적입니다. 코드 주석과 로그 문자열은 한국어로 작성되어 있습니다.

## 빌드 / 실행

일반적인 UE5 C++ 프로젝트로, 별도의 CLI 빌드 스크립트나 테스트 러너는 없습니다.

- Visual Studio(Unreal 워크로드가 설치된 VS2022 — 필요한 컴포넌트는 `.vsconfig`에 명시됨)에서 `MissionSystem.sln`을 열어 `MissionSystemEditor` 타겟(Development Editor, Win64)을 빌드하거나, `MissionSystem.uproject`를 직접 열어 에디터를 실행하면 컴파일 여부를 물어봅니다.
- 소스 파일을 추가/삭제한 뒤 `.sln`을 재생성하려면 `MissionSystem.uproject`를 우클릭 → "Generate Visual Studio project files"를 실행하거나(또는 엔진의 `GenerateProjectFiles` 스크립트로 `UnrealBuildTool`을 실행), 해당 작업을 수행합니다.
- 이 저장소에는 자동화된 테스트가 없습니다.

## 아키텍처

미션 시스템은 팩토리 + 서브시스템 조합으로 구동되는 작은 상태 머신이며, 델리게이트를 통해 액터/UI와 분리되어 있습니다:

- **`UMissionManager`** (`Manager/`) — `UGameInstanceSubsystem`. 현재 진행 중인 미션, 미션 UI 위젯, 미션을 발생시킨 NPC를 소유하는 유일한 전역 진입점입니다. 시스템 간 호출은 거의 대부분 `GetGameInstance()->GetSubsystem<UMissionManager>()`를 통해 이루어집니다.
- **`UMissionFactory`** (`Factory/`) — `EMissionUnique`(특정 미션을 식별하는 열거형, 예: `Act1Chapter1`)를 `NewObject`를 통해 구체적인 `UBaseMission` 서브클래스로 매핑합니다. 새 미션 타입은 여기에 추가합니다.
- **`UBaseMission`** (`Mission/BaseMission.h`) — `Ready → InProgress → Completed` 상태 머신(`EMissionState`)을 구현하는 베이스 클래스입니다. 상태 전환은 `SetState()`를 통해 이루어지며, 이 함수는 가상 함수인 `Enter*State()` 훅을 호출하고 `OnMissionStateChanged`(다이나믹 멀티캐스트 델리게이트)를 브로드캐스트합니다. 구체적인 미션 클래스(`MissionAct1Chapter1`, `MissionAct1Chapter2`)는 `Enter*State()`와 `AgreeMission`/`DisagreeMission`/`SuccessMission`/`FailedMission` 메서드들을 오버라이드하여 미션별 동작을 추가합니다. 현재는 모두 `Super::`만 호출하는 스텁 상태입니다.
- **`ANPC`** (`Character/NPC/`) — 스피어 오버랩 트리거를 가진 액터입니다. 플레이어 캐릭터와 오버랩되면 `MissionManager->ShowMainMissionWidget(MissionUnique, this)`를 호출하고, 오버랩이 끝나면 위젯을 숨깁니다. 각 NPC 인스턴스는 `EMissionUnique` 값으로 어떤 미션을 제공할지 설정됩니다.
- **`UMainMissionWidget`** (`UMG/`) — 미션 대화창 UMG 위젯입니다(바인딩되는 블루프린트 위젯은 `WBP_MainMission`이며, 컨트롤러에 지정되지 않은 경우 `AMainPlayerController::BeginPlay`에서 경로로 폴백 로드합니다). `EMissionState`에 따라 미션 제목/버튼 텍스트를 렌더링하고, `OnMissionButtonAction`(Left/Right = 상태에 따라 Agree/Disagree 또는 Success/Fail)을 브로드캐스트하여 `HandleMissionButtonAction`을 통해 `MissionManager`로 전달합니다.
- **`AMissionSystemCharacter`** (`Character/User/`) — 플레이어 캐릭터입니다. `ECharacterPlayMode`(`Normal` = 3인칭 자유 이동, `DefenseMode` = 쿼터뷰 미니게임 모드)를 가지며, 미션을 수락/종료할 때 `MissionManager`가 이 값을 전환합니다. 이동/시점/점프에는 Enhanced Input(`UInputMappingContext`/`UInputAction`)을 사용합니다.
- **`AMainPlayerController`** — 단일 `UMainMissionWidget` 인스턴스를 생성 및 소유하고, 뷰포트에 숨김 상태로 추가한 뒤 `MissionManager`에 등록합니다.

미션 진행 흐름: 플레이어가 `ANPC`와 오버랩 → `MissionManager::ShowMainMissionWidget` → `RequestMission` → `MissionFactory::CreateMission` → 새로 생성된 `UBaseMission` 서브클래스 인스턴스가 `Ready` 상태로 진입 → 위젯에 Agree/Disagree 버튼 표시 → `HandleMissionButtonAction`이 `AgreeMission`(상태 → `InProgress`, NPC 숨김/충돌 비활성화, 캐릭터가 `DefenseMode`로 전환) 또는 `DisagreeMission`(미션 종료, NPC 복원)을 호출합니다. `MissionManager::OnMissionStateChanged`는 위젯에 표시되는 텍스트/버튼을 미션 상태와 계속 동기화합니다.

### 새 미션 추가하기

1. `Define/MissionDefine.h`의 `EMissionUnique`에 항목을 추가합니다.
2. `Mission/`에 `UBaseMission` 서브클래스를 만들고 필요한 `Enter*State()`/동작 메서드를 오버라이드합니다.
3. `UMissionFactory::CreateMission`에 해당 case를 추가합니다.
4. 새 미션을 제공할 `ANPC`(블루프린트 또는 C++ 기본값)의 `MissionUnique` 프로퍼티에 새로 추가한 `EMissionUnique` 값을 지정합니다.

## 참고 사항

- `MissionDefine.h`는 공용 열거형(`EMissionUnique`, `EMissionState`, `EMissionButtonAction`)과 지역화된 `FText` UI 문자열(`LOCTEXT`)을 한곳에 모아두며, 미션 클래스들과 위젯에서 함께 사용합니다.
- `UMainMissionWidget`의 위젯 자식 바인딩(`MissionTitleText`, `LeftButton`/`RightButton` 등)은 `BindWidget`을 사용하므로, `Content/` 아래의 대응되는 `WBP_MainMission` UMG 블루프린트 에셋의 위젯 이름과 반드시 일치해야 합니다.
- 모듈(`MissionSystem.Build.cs`)은 `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `UMG`에 의존합니다.
