# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

NPC 기반 미션(퀘스트) 시스템과, 미션 수락 시 진입하는 쿼터뷰 디펜스 미니게임(위에서 몬스터가 내려오고 좌우로만 피하는)을 보여주는 Unreal Engine 5.8 C++ 프로젝트(`MissionSystem`)입니다. 완성된 게임이 아니라 깔끔한 코드 스타일을 보여주는 것이 목적입니다(`README.md`). 코드 주석과 로그 문자열은 한국어로 작성되어 있습니다.

## 빌드 / 실행

일반적인 UE5 C++ 프로젝트로, 별도의 CLI 빌드 스크립트나 테스트 러너는 없습니다.

- Visual Studio(Unreal 워크로드가 설치된 VS2022 — 필요한 컴포넌트는 `.vsconfig`에 명시됨)에서 `.sln`을 열어 `MissionSystemEditor` 타겟(Development Editor, Win64)을 빌드하거나, `MissionSystem.uproject`를 직접 열어 에디터를 실행하면 컴파일 여부를 물어봅니다. (`.sln`/`.vcxproj`는 gitignore 처리되어 저장소에 없으므로 최초 클론 후에는 아래 단계로 새로 생성해야 함)
- 소스 파일을 추가/삭제한 뒤 프로젝트 파일을 재생성하려면 `MissionSystem.uproject`를 우클릭 → "Generate Visual Studio project files"를 실행합니다.
- 이 저장소에는 자동화된 테스트가 없습니다.
- 모듈(`MissionSystem.Build.cs`)은 `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `UMG`에 의존합니다.

## 코드 스타일

- **불리언 부정에 `!` 연산자를 쓰지 않습니다.** `if (!bValid)`/`if (!Foo())` 대신 `if (bValid == false)`/`if (Foo() == false)`처럼 항상 `== false`/`== true`로 명시적으로 비교합니다. (예: `if (bIsActive == false)`, `if (TargetPlayer.IsValid() == false)`) 새 코드를 작성하거나 기존 코드를 수정할 때 이 규칙을 따르세요.

## 아키텍처

### 미션 상태 머신

- **`UMissionManager`** (`Manager/`) — `UGameInstanceSubsystem`. 현재 진행 중인 미션, 미션/결과 UI 위젯, 미션을 발생시킨 NPC를 소유하는 유일한 전역 진입점입니다. 시스템 간 호출은 거의 대부분 `GetGameInstance()->GetSubsystem<UMissionManager>()`를 통해 이루어집니다. **몬스터 스폰/생존 타이머(디펜스 미니게임)는 소유하지 않음** — 이건 미션 자신(`UDefenseMinigameMission`)의 책임이며, `MissionManager`는 `CurrentMission->GetMinigameRemainingTime()`처럼 미션에 위임만 함(HUD 위젯이 여전히 `MissionManager` 하나만 알면 되도록 하는 Facade 역할).
- **`UMissionFactory`** (`Factory/`) — Factory Method 패턴. `EMissionUnique`를 `UBaseMission` 인스턴스로 매핑합니다. `Act1Chapter1`→`UMissionAct1Chapter1`, `Act1Chapter2`→`UMissionAct1Chapter2`(둘 다 `UDefenseMinigameMission` 상속) — 새 미션별 동작이 필요하면 `UBaseMission`(또는 디펜스 미니게임이 필요하면 `UDefenseMinigameMission`)을 상속받는 클래스를 만들어 여기서 분기하면 됩니다.
- **`UBaseMission`** (`Mission/BaseMission.h`) — `Ready → InProgress → Succeeded`/`Failed` 상태 머신(`EMissionState`)입니다. 상태 전환은 `SetState()`를 통해 이루어지며, 가상 함수인 `Enter*State()` 훅을 호출하고 `OnMissionStateChanged`(다이나믹 멀티캐스트 델리게이트)를 브로드캐스트합니다. `AgreeMission(AMissionSystemCharacter* Player)`가 호출한 플레이어를 `MissionPlayer`(`TWeakObjectPtr`)에 저장해 `GetMissionPlayer()`로 서브클래스의 State 훅에서 쓸 수 있게 함. `GetMinigameRemainingTime()`/`GetMinigameTotalDuration()` 가상 함수(기본 -1/0)를 미니게임을 갖는 서브클래스가 오버라이드.
  - **주의**: `BaseMission.h`는 `AMissionSystemCharacter`를 전방 선언만 함. `TWeakObjectPtr<T>::Get()`은 내부적으로 `static_cast`를 하기 때문에 완전한 타입이 필요 — 그래서 `GetMissionPlayer()`/`AgreeMission()`의 구현은 반드시 `BaseMission.cpp`(여기서 `MissionSystemCharacter.h`를 include)에 있어야 하며, 헤더에 인라인으로 정의하면 컴파일 에러가 남.
- **`UDefenseMinigameMission`** (`Mission/DefenseMinigameMission.h`, `Abstract`) — 디펜스 미니게임(몬스터 웨이브 생존)으로 진행되는 미션들의 공통 베이스. `UDefenseMinigameController`를 직접 소유하며, `EnterInProgressState()`에서 `Start(GetMissionPlayer())`, `EnterSucceededState()`/`EnterFailedState()`에서 `Stop()`. 생존 타이머 만료(`OnSurvived`)를 직접 구독해 `SuccessMission()`을 호출. `UMissionAct1Chapter1`/`UMissionAct1Chapter2`는 이 클래스를 상속만 하고 미션별 `Enter*State()` 로그만 추가한 상태(Template Method 패턴).
- **`ANPC`** (`Character/NPC/`) — 스피어 오버랩 트리거 액터. 플레이어와 오버랩되면 `MissionManager->ShowMainMissionWidget(MissionUnique, this)`를 호출하고, 오버랩이 끝나면 위젯을 숨깁니다.

### 미션 진행 흐름

1. 플레이어가 `ANPC`와 오버랩 → `ShowMainMissionWidget` → `RequestMission` → `UBaseMission`이 `Ready` 상태로 진입, `WBP_MainMission`에 Agree/Disagree 버튼 표시.
2. **Agree**(`HandleMissionButtonAction` Left/Ready) → NPC 숨김/충돌 끔 → `AMissionSystemCharacter::SetPlayMode(DefenseMode)`(쿼터뷰 전환) → `CurrentMission->AgreeMission(MissionCharacter)`(`InProgress`로 전환). 미션이 `UDefenseMinigameMission` 계열이면 이 안의 `EnterInProgressState()`가 몬스터 스폰 + `SurvivalDuration`(기본 30초) 생존 타이머를 알아서 시작함 → `AMainPlayerController::SetDefenseHUDVisible(true)`로 HP/타이머 위젯 노출.
3. 디펜스 모드 중 몬스터(`AMonster`, `Character/Monster/`)가 플레이어 쪽(월드 -X, 화면상 위→아래)으로 이동하며 오버랩 시 `AMissionSystemCharacter::ApplyDamage()` 호출.
4. 종료 조건 두 가지:
   - **생존 성공**: 타이머 만료 → 미션의 `UDefenseMinigameController::OnSurvived` 브로드캐스트 → 미션 자신의 핸들러가 `SuccessMission()` 호출 (`Succeeded`)
   - **HP 0**: `ApplyDamage`에서 HP≤0이면 `MissionManager::HandlePlayerDefeated()` → `CurrentMission->FailedMission()` (`Failed`)
   두 경우 모두 `EnterSucceededState()`/`EnterFailedState()`가 동기적으로 `DefenseMinigame->Stop()`을 호출하므로 `MissionManager`가 따로 정지시킬 필요 없음.
5. `OnMissionStateChanged`가 `Succeeded`/`Failed`를 받으면 `SetDefenseHUDVisible(false)` + `ShowMissionResultWidget(state)`로 결과창(`UMissionResultWidget`, `WBP_MainMissionResult`) 노출.
6. 결과창의 Confirm 버튼 → `HandleMissionResultConfirmed` → HP 회복(`ResetHP`) → `ClearCurrentMission()`: **NPC를 다시 활성화하기 전에** 플레이어를 `AGameModeBase::FindPlayerStart`로 찾은 `PlayerStart` 위치로 텔레포트(같은 자리에서 NPC와 즉시 재오버랩하는 버그 방지) → `SetPlayMode(Normal)` → NPC 재표시/충돌 복원.

### UI 위젯과 `AMainPlayerController`

`AMainPlayerController`가 모든 UMG 위젯 인스턴스(`MainMissionWidget`, `MissionResultWidget`, `HPWidget`, `MissionTimerWidget`)를 `BeginPlay`에서 생성·`AddToViewport()`하고, `UMissionManager`가 각 위젯의 delegate(`OnMissionButtonAction`, `OnConfirmed`)를 구독해 상태를 동기화합니다.

- **`UMainMissionWidget`** — Agree/Disagree(Ready) 대화창. `HandleMissionButtonAction`은 **`Ready` 상태에서만** `Left`/`Right`를 처리함 — InProgress 중에는 버튼으로 미션을 강제 종료하지 않음(과거엔 InProgress에도 Success/Fail 수동 버튼이 있었으나 제거됨. 미션은 오직 디펜스 미니게임 결과로만 끝남). 이 설명과 다르게 코드를 고치게 되면(예: InProgress 분기를 다시 추가) 반드시 사용자에게 먼저 확인할 것.
- **`UMissionResultWidget`** — 성공/실패 결과창. **하나의 클래스를 성공/실패 양쪽에 재사용**하며 `SetResultState(EMissionState)`로 타이틀만 바꿉니다. Confirm 버튼 하나만 있음.
- **`UHPWidget`**, **`UMissionTimerWidget`** — 디펜스 모드 동안만 노출되는 순수 표시용 HUD(HP 게이지는 `FMath::FInterpTo`로 서서히 감소, 타이머는 `NativeTick`에서 `MissionManager::GetMissionRemainingTime()`을 매 프레임 폴링). **버튼이 없는 위젯은 반드시 `ESlateVisibility::HitTestInvisible`로 노출해야 함** — `Visible`로 하면 위젯의 (기본적으로 화면 전체를 덮는) 루트 Canvas Panel이 다른 위젯의 클릭을 가로채는 버그가 발생했던 전례가 있음.
- 위젯 클래스 지정은 **`AMainPlayerController`의 `TSubclassOf` 프로퍼티**(C++ 기본값 또는 BP 서브클래스인 `PC_MainPlayer`에서)로 이루어짐. C++ 쪽 `LoadClass(...)` 폴백 경로는 미지정 시에만 쓰이는 안전망이라 실제 에셋 이름과 정확히 일치하지 않을 수 있음(예: 코드상 폴백은 `WBP_MissionTimer`지만 실제 에셋명은 `WBP_Timer`) — **실제 위젯 배선을 확인하려면 코드의 문자열 경로가 아니라 `Content/ThirdPerson/Blueprints/PC_MainPlayer`의 기본값을 봐야 함**.

### GameMode 체인 (주의)

`Config/DefaultEngine.ini`의 `GlobalDefaultGameMode`는 C++ `AMissionSystemGameMode`이지만, **레벨(`ThirdPersonMap`)의 World Settings가 GameMode Override를 `Content/GameMode/GM_Main`(Blueprint, `AMissionSystemGameMode` 상속)으로 지정**하고 있고, `GM_Main`이 다시 `PlayerControllerClass`를 `Content/ThirdPerson/Blueprints/PC_MainPlayer`(Blueprint, `AMainPlayerController` 상속)로 오버라이드합니다. 즉 실제 실행되는 GameMode/PlayerController는 C++ 기본값이 아니라 이 두 Blueprint이며, 위젯 클래스 등 BP 전용 기본값은 여기서 찾아야 합니다.

### 플레이어 캐릭터 (`AMissionSystemCharacter`, `Character/User/`)

- `ECharacterPlayMode`: `Normal`(3인칭 자유 이동) / `DefenseMode`(쿼터뷰, 좌우 이동만, `Tick()`에서 카메라 거리/각도/오프셋을 `FInterpTo`/`RInterpTo`로 보간).
- 이동 속도는 모드별로 분리됨: `NormalModeMoveSpeed`(기본 500) / `DefenseModeMoveSpeed`(기본 850, 몬스터를 피해야 하므로 더 빠름) — `SetPlayMode()`가 전환 시 `GetCharacterMovement()->MaxWalkSpeed`를 해당 값으로 갈아끼움.
- HP: `MaxHP`(기본 500)/`CurrentHP`, `ApplyDamage()`(0 이하가 되면 `MissionManager::HandlePlayerDefeated()` 호출), `ResetHP()`, `OnHPChanged`(`BlueprintAssignable`) 델리게이트로 `UHPWidget`에 통지.
- 이동/시점/점프는 Enhanced Input(`UInputMappingContext`/`UInputAction`) 사용.

### 몬스터 (`Character/Monster/`, `Spawner/`, `Manager/DefenseMinigameController`)

- **`UDefenseMinigameController`** (`Manager/`, plain `UObject`) — 몬스터 스폰 + 생존 타이머만 전담하는 컨트롤러. **`UDefenseMinigameMission`(미션 자신)이 소유** — `Start(Player)`/`Stop()`으로 제어하고 `OnSurvived` 델리게이트로 생존 성공을 알림(SRP: 미션 상태 전이 로직과 분리. 과거엔 `UMissionManager`가 소유했으나, "어떤 미션이 디펜스 미니게임을 쓰는가"는 `MissionManager`가 아니라 미션 자신이 알아야 할 규칙이라 판단해 이전함).
- **`AMonster`** — **오브젝트 풀 패턴**으로 관리됨. `Tick`마다 월드 `-X` 방향으로 `MoveSpeed`(기본 900)만큼 이동하다 `MaxTravelDistance` 초과 또는 플레이어와 오버랩 시 `Destroy()` 대신 `Deactivate()`(숨김 + 충돌/틱 비활성화 + `OnDeactivated` 브로드캐스트)로 풀에 반환됨. 오버랩 시 `DamageAmount`(기본 10)만큼 `ApplyDamage` 호출.
- **`AMonsterSpawner`** — `StartSpawning(Player)` 시 `SpawnInterval`(기본 0.5초)마다 `SpawnMonster()`가 호출되고, 매번 `MinSpawnCount`~`MaxSpawnCount`(기본 1~3) 사이에서 랜덤으로 뽑은 개수만큼 `SpawnOneMonster()`를 반복 호출해 **한 번에 여러 마리를 동시에** 등장시킴(난이도 조절용). 각 `SpawnOneMonster()`는 `AcquireMonster()`(풀에 대기 중인 몬스터가 있으면 재사용, 없으면만 `SpawnActor`)로 몬스터를 꺼내 플레이어 앞쪽(`SpawnDistanceX`) 좌우 랜덤(`SpawnRangeY`) 위치에 배치. `StopSpawning()`은 타이머를 멈추고 활성 몬스터를 전부 `Deactivate()`(풀로 반환, `Destroy()` 아님).

## 새 미션 추가하기

1. `Define/MissionDefine.h`의 `EMissionUnique`에 항목 추가.
2. `Mission/`에 새 미션 클래스를 만듭니다. 디펜스 미니게임(몬스터 웨이브)을 쓰는 미션이면 `UDefenseMinigameMission`을, 아니면 `UBaseMission`을 상속받아 `Enter*State()`(로그/연출/보상 등)를 오버라이드.
3. `UMissionFactory::CreateMission`에 케이스 추가해 새로 만든 클래스를 반환.
4. 새 미션을 제공할 `ANPC`(블루프린트 또는 C++ 기본값)의 `MissionUnique` 프로퍼티에 지정.

## 참고 사항

- `MissionDefine.h`는 공용 열거형(`EMissionUnique`, `EMissionState`, `EMissionButtonAction`)과 지역화된 `FText` UI 문자열(`LOCTEXT`)을 한곳에 모아두며, 미션 클래스들과 위젯에서 함께 사용합니다.
- 각 위젯의 `BindWidget` 자식 바인딩(`MissionTitleText`, `LeftButton`, `HPBar`/`HPText`, `TimerBar`/`TimerText` 등)은 `Content/UMG/` 아래 대응되는 WBP 에셋(`WBP_MainMission`, `WBP_MainMissionResult`, `WBP_HP`, `WBP_Timer`)의 위젯 이름과 반드시 일치해야 합니다.
- 캐릭터 메시/애니메이션은 UE5 기본 마네킹 중 **Quinn**만 사용합니다(`SKM_Quinn`, `ABP_Quinn`) — Manny 계열(`SK_Mannequin`은 Quinn이 스켈레톤을 공유하므로 예외)은 실제로 참조되지 않는 자산이니, 콘텐츠를 정리할 때 어떤 애셋이 어떤 애니메이션 블루프린트에서 참조되는지 먼저 확인 없이 지우지 말 것(`ABP_Manny`를 지웠다가 `ABP_Quinn` 로드 실패를 유발한 전례 있음 — Quinn의 `ABP_Quinn`이 사용하지 않는 노드로 `ABP_Manny`를 하드 레퍼런스하고 있었음).
