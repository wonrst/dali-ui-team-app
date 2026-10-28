# Samsong Buxby

세 개의 독립 윈도우로 구성된 DALi Team 샘플입니다.

## 앱 구성

| 멤버 ID | 역할 | 윈도우 크기 |
|---|---|---|
| `dali-buxby` | 언어 선택과 추천 카드 | 600 × 1080 |
| `dali-buxby.day` | 일정 | 520 × 448 |
| `dali-buxby.activity` | 활동 차트 | 520 × 448 |

세 멤버는 모두 `TeamUIApplication`입니다. 메인이 두 서브 멤버를 실행하고 함께 종료합니다.
메인에서 영어·한국어·아랍어를 선택하며, 각 멤버는 언어 변경 콜백에서 자신의 화면을 번역합니다.
기본 언어는 영어입니다.

## GBS 빌드

Team 1.3.6 이상, app-core-cpp, DALi Core/Adaptor/UI Foundation/UI Components를 제공하는
GBS 저장소와 프로파일을 설정한 뒤 프로젝트 루트에서 실행합니다.
상세 빌드 의존성은 [packaging/dali-buxby.spec](packaging/dali-buxby.spec)에 정의되어 있습니다.

```sh
gbs build -A armv7l --include-all --packaging-dir packaging --spec dali-buxby.spec
```

다른 아키텍처는 `-A` 값을 변경합니다. 빌드 시 PO도 MO로 컴파일되어 RPM에 포함됩니다.

## 설치 및 실행

생성된 RPM을 대상 장치로 복사한 뒤 설치하고 앱을 등록합니다.

```sh
rpm -Uvh dali-buxby-1.0.0-1.armv7l.rpm
tpk-backend --preload -y dali-buxby
app_launcher -s dali-buxby
```
