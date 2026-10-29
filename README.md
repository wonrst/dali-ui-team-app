# DALi Team App

세 개의 `TeamUIApplication`으로 구성된 DALi UI 템플릿입니다.
메인 창(600 × 1080)은 영어·한국어·아랍어 선택 버튼과 라벨을,
두 서브 창(각 520 × 448)은 번역 라벨을 표시합니다. 기본 언어는 영어입니다.
1920 × 1080 화면 기준으로 메인은 오른쪽, 서브 창은 왼쪽 위·아래에 배치합니다.
세 창은 반투명이며 서브 창에는 둥근 모서리를 적용합니다.

## 코드 구성

- `src/main-member.cpp`: 메인 UI, 언어 설정, 서브 멤버 실행·종료와 초기 포커스.
- `src/first-member.cpp`, `src/second-member.cpp`: 각 서브 창의 UI·생명주기·번역.
- `src/window-navigation.*`: 창 간 키보드 이동.
- `src/app-config.h`, `src/team-helpers.h`: 멤버 ID·이동 경로와 Team 공통 호출.
- `po/`: 세 멤버가 공유하는 gettext 도메인의 영어·한국어·아랍어 번역.

각 멤버의 `CreateUi()`는 `GetDefaultWindow()`에 세로 `StackLayout`을 추가하고
그 안에 라벨을 배치합니다. 메인의 언어 버튼은 가로 `StackLayout`에 배치합니다.
UI를 추가·수정하려면 해당 멤버의 `CreateUi()`를 편집하면 됩니다.
`OnLanguageChanged()`에서는 `dgettext()`로 라벨을 갱신합니다.
방향키는 창 안의 포커스를 이동하고 경계에서 다른 창으로 넘어갑니다.
Tab / Shift+Tab은 창을 순환합니다. Back / Escape는 서브 창에서 메인으로 돌아오며,
메인에서는 앱을 종료합니다. 창 안의 포커스 복원은 DALi가 처리합니다.

## 빌드와 실행

Team 1.3.6 이상, app-core-cpp, DALi Core/Adaptor/UI Foundation을 제공하는
GBS 저장소와 프로파일을 설정한 뒤 실행합니다. PO는 빌드 시 MO로 컴파일됩니다.

```sh
gbs build -A armv7l --include-all --packaging-dir packaging --spec dali-ui-team-app.spec
```

생성된 RPM을 대상 장치에 복사한 뒤 설치·등록·실행합니다.

```sh
rpm -Uvh dali-ui-team-app-1.0.0-1.armv7l.rpm
tpk-backend --preload -y dali-ui-team-app
app_launcher -s dali-ui-team-app
```
