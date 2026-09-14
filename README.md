# MissionSystem

Unreal Engine 5.8 + C++로 제작한 **NPC 기반 미션 시스템** 포트폴리오 프로젝트입니다.
NPC에게 미션을 수락하면 위에서 몬스터가 내려오는 **쿼터뷰 디펜스 미니게임**으로 진입해 일정 시간을 생존하면 성공하는 흐름을 보여줍니다. 완성된 게임이 아니라, 깔끔한 코드 구조와 디자인 패턴 적용을 보여주는 데 목적이 있습니다.

## 진행 흐름

1. 플레이어가 NPC와 오버랩 → 미션 수락/거절 대화창 표시
2. 수락 시 쿼터뷰 디펜스 모드로 전환, 몬스터 웨이브 스폰 + 생존 타이머 시작
3. 몬스터를 피하며 생존 → 타이머 만료 시 성공, HP 0 도달 시 실패
4. 결과창 확인 → HP 회복, 원래 위치로 복귀, NPC 재활성화

## 설계 포인트

채용 담당자가 구조를 빠르게 파악할 수 있도록, 적용한 패턴 위주로 정리했습니다.

| 패턴 | 적용 위치 |
|---|---|
| State Machine | `UBaseMission` — `Ready → InProgress → Succeeded/Failed` |
| Factory Method | `UMissionFactory` — `EMissionUnique` → 미션 인스턴스 매핑 |
| Template Method | `UDefenseMinigameMission` → `UMissionAct1Chapter1/2` |
| Object Pool | `AMonster` — `Destroy()` 대신 `Deactivate()`로 풀에 반환 |
| Facade | `UMissionManager` — HUD/위젯이 서브시스템 하나만 알면 되도록 진입점 통합 |

각 패턴의 상세 구조와 책임 분리 이유는 [`CLAUDE.md`](./CLAUDE.md)에 정리되어 있습니다.

## 기술 스택

- Unreal Engine 5.8, C++
- Enhanced Input
- UMG (State 기반 HUD/대화창 위젯)

## 빌드 / 실행

이 저장소에는 `.sln`/`.vcxproj`가 포함되어 있지 않습니다(gitignore). 클론 후:

1. `MissionSystem.uproject` 우클릭 → **Generate Visual Studio project files**
2. 생성된 `.sln`을 Visual Studio 2022(Unreal 워크로드)에서 열어 `MissionSystemEditor` (Development Editor, Win64) 빌드
3. 또는 `MissionSystem.uproject`를 바로 실행해 컴파일 여부를 물으면 승인

자세한 빌드/아키텍처/코드 스타일 규칙은 [`CLAUDE.md`](./CLAUDE.md)를 참고하세요.
