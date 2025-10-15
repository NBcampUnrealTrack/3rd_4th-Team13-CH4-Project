# 🎮 OutGame 시스템 구축

> **타이틀 → 로비 → 게임 플레이**로 이어지는 멀티플레이 접속 흐름 구현

## 📌 주요 기능

### 1. 멀티플레이 접속 시스템
- **Listen 서버 생성** 및 클라이언트 Join 로직 구현
- **스마트 포트 할당 시스템**
  - 로컬/사설 IP → 7777 포트
  - 공인 IP → 17777 포트 (포트포워딩 대응)
  - IP 대역 자동 판별 및 포트 할당
- **서버 IP 입력 UI** 구현
  - 기본값 자동 클리어 기능
  - 사용자 친화적 입력 경험 제공

### 2. 로비 시스템
- **실시간 접속자 관리**
  - GameState 델리게이트 기반 플레이어 리스트 동기화
  - PlayerState 복제를 통한 닉네임/팀 정보 공유
  - 동시 접속 대응 (지연 브로드캐스트: 0.2초)  
    → 중복 갱신 문제 방지
- **Host/Client UI 분기 처리**
  - Host 전용: 게임 시작 버튼, IP 정보 표시
  - Client: 접속자 리스트만 표시
  - 최소 인원 수 미만일 경우, 호스트의 Play 버튼이 비활성화되어 게임 시작을 제한함
  - 접속 인원 변화 시 `HandlePlayerListChanged()` → `UpdateUIByRole()` 순으로 UI 자동 갱신 처리
- **IP 정보 시스템**
  - Winsock API 활용 로컬 IP 획득
  - 외부 API (`ipify.org`) 연동 → 공인 IP 자동 조회
  - 실시간 IP 정보 표시 (Local IP / Public IP)

## 🔧 기술적 특징

### 아키텍처
```
타이틀(L_Title) → [서버 생성/접속] → 로비(L_Lobby) → [게임 시작] → 게임플레이
```

### 핵심 구현
- **GameMode/GameState** 계층 구조 기반 접속자 관리
- **델리게이트 체인**을 통한 UI 자동 갱신
  - `GameState::OnPlayerListChanged` → `PlayerState::OnRep_PlayerName` → UI 업데이트
- **ServerTravel + Seamless Travel**: 레벨 전환 시 접속 유지 및 로딩 최소화

### 보안 및 최적화
- 서버 권한 검증: **게임 시작은 Host만 가능**
- COND_OwnerOnly 복제 조건 사용 → 네트워크 대역폭 최적화
- RPC Validate를 통한 입력 검증 및 보안 강화