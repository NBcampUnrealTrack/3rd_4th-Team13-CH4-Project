# ⚔️ GAS(Gameplay Ability System) 기반 스킬 시스템 

> **Event-Driven + Server Authority** 아키텍처로 설계된 확장 가능한 멀티플레이 스킬 시스템

> 런타임 아이템 획득 → 동적 스킬 등록 → 다중 검증 실행

## 📌 핵심 설계 원칙

### 1. Event-Driven 아키텍처
- **SkillEventSubsystem을 통한 디커플링**
  - 아이템 획득 이벤트 → 서브시스템 브로드캐스트 → 스킬 매니저 등록
  - 모듈 간 직접 참조 제거로 유지보수성 향상

### 2. Server Authority 구조
- **모든 스킬 로직은 서버에서 실행**
  - 클라이언트는 RPC 요청만 수행
  - 다중 검증 레이어로 치트 원천 차단

### 3. 태그 기반 확장 설계
- **GameplayTag 매핑 시스템**
  - `TMap<FGameplayTag, TSubclassOf<UGameplayAbility>>`
  - 신규 스킬 추가 시 **코드 수정 불필요** (에디터에서 설정)
  - 도둑/경찰 전용 아이템 필터링 (팀 태그 검증)

## 🎯 주요 기능

### 1. 런타임 스킬 관리
- **동적 스킬 슬롯 시스템**
  - 아이템 획득 → 스킬 자동 등록
  - 중복 스킬 시 사용 횟수 누적
  - 슬롯 꽉 차면 **FIFO 방식** 자동 제거
- **서버 권한 기반 검증**
  - 서버에서만 스킬 추가/제거 실행
  - 클라이언트는 RPC를 통해 요청
  - 중복 획득 방지 (상태 플래그 3단 체크)

### 2. 다중 검증 안티치트 시스템
**5단계 검증 파이프라인**으로 메모리 조작 완벽 차단:
```
클라이언트 입력
    ↓
① RPC Validate (슬롯 인덱스 검증)
    ↓
② 서버 쿨다운 체크 (ServerCooldownEndTimes)
    ↓
③ 스팸 방지 (0.1초 최소 간격)
    ↓
④ GAS CommitAbility (쿨다운/Cost)
    ↓
⑤ AttributeSet 검증 (속도 변조 차단)
```

- **이중 쿨다운 시스템**
  - GAS와 독립된 서버 쿨다운 맵 운영
  - GameplayEffect 델리게이트 바인딩으로 자동 동기화
  - 클라이언트 메모리 변조 시도 무효화

### 3. UI 실시간 동기화
- **Replication + 델리게이트 패턴**
  - `OnRep_SkillSlots()` → `OnSkillChanged` 브로드캐스트
  - COND_OwnerOnly로 대역폭 최적화
- **자동 쿨다운 UI 갱신**
  - GAS 델리게이트(`HandleEffectAdded/Removed`) 바인딩
  - GameplayEffect 적용 시 자동 업데이트

## 🔧 기술적 특징

### 네트워크 최적화
- **Cosmetic 투사체** (클라이언트 로컬 생성)
- **마지막 투사체 생성 보장** (0.1초 타이머 기반 지연 제거)
- **중복 오버랩 방지**
  - 상태 플래그 3단 체크 (`bIsPickedUp`, `bIsDestroyed`, `bIsProcessingPickup`)
  - 콜리전 즉시 비활성화로 네트워크 레이스 컨디션 방지

### 핵심 구조
```
[아이템 획득] → [서브시스템 이벤트] → [스킬 매니저 등록] 
    → [플레이어 입력] → [서버 검증] → [GAS 실행]
```

### 안전장치
- ASC 초기화 3곳 분산 (BeginPlay, PossessedBy, OnRep_PlayerState)

  - **BeginPlay**  
    테스트 결과, BeginPlay 시점에 PlayerState가 유효할 경우 SkillManagerComponent가 ASC를 정상적으로 가져오는 것이 확인됨.

  - **PossessedBy, OnRep_PlayerState**  
  그러나 네트워크 환경과 리플리케이션 타이밍에 따라 BeginPlay 시점에 PlayerState가 없거나 완전히 초기화되지 않은 경우가 있음.

  따라서 PossessedBy와 OnRep_PlayerState에서도 ASC 초기화를 시도함으로써 예외 상황에 대비하는 안전장치 역할을 수행함.


- 쿨다운 인덱스 자동 동기화 (슬롯 시프트 시 맵도 함께 조정)
- AttributeSet `PreAttributeChange`로 속도 변조 실시간 차단