# AGENTS.md — Qtdon

## プロジェクト概要

Qtdon は **Qt 6 / C++17** と **Qt Quick (QML)** で構築された Mastodon クライアントアプリケーション。
OAuth2 認可コードフローを経て、任意の Mastodon インスタンスへのステータス投稿（トゥート）が可能。

- **ライセンス:** LGPL v3
- **ビルドシステム:** CMake（`CMakeLists.txt`）— Qt 6.5+ 必須
- **Qt モジュール:** `Quick`, `Network`, `QuickControls2`
- **C++ 標準:** C++17

## ディレクトリ構造

```
Qtdon/
├── CMakeLists.txt                   # CMake ビルド定義（qt_add_qml_module 使用）
├── main.cpp                         # アプリケーションエントリーポイント
├── Main.qml                         # メイン UI（ログイン・投稿画面）— 大文字始まりで QML 型として登録
├── mastodonclient.h / .cpp          # QML ↔ C++ ファサード（QML_ELEMENT）
├── oauth2.h / .cpp                  # OAuth2 汎用ヘルパークラス
├── oauthmastodon.h / .cpp           # Mastodon 固有の OAuth2 非同期フロー
├── qmastodonnetbase.h / .cpp        # API リクエスト抽象基底クラス
├── qmastodonpoststatus.h / .cpp     # POST /api/v1/statuses 実装
├── .gitignore                       # Git 除外設定
├── content/
│   └── AuthWindow.qml               # 認証ウィンドウ（未実装プレースホルダー）
├── LICENSE
├── README.md
└── AGENTS.md                        # 本ファイル
```

## アーキテクチャ

### クラス階層

```
QObject
├── OAuth2                           # OAuth2 認証クエリ・ヘッダー生成
│   └── OAuthMastodon                # Mastodon 向け非同期認証フロー
├── QMastodonNetBase                 # API リクエスト基底（JSON解析・エラーハンドリング）
│   └── QMastodonPostStatus          # POST /api/v1/statuses
└── MastodonClient  [QML_ELEMENT]    # QML 向けファサード
```

### レイヤー構成

1. **UI 層 (QML):** `main.qml` — `MastodonClient` を直接インスタンス化し、プロパティバインディングで状態を反映
2. **ファサード層:** `MastodonClient` — `QML_ELEMENT` で QML モジュールに自動登録。`Q_INVOKABLE` メソッドと `Q_PROPERTY(READ/NOTIFY)` で双方向通信
3. **認証層:** `OAuth2` → `OAuthMastodon` — 完全非同期。ブラウザで認可URL を開き、シグナルでアクセストークン取得完了を通知
4. **API 層:** `QMastodonNetBase` → `QMastodonPostStatus` — 認証済みの HTTP リクエストを Mastodon API に送信。エラーはシグナル経由で上位に伝播

### データフロー

```
[QML UI]
  │  Q_INVOKABLE
  ▼
[MastodonClient]
  ├── startAuth(host)     → OAuthMastodon::requestAuthorization()
  │                           → QDesktopServices::openUrl()
  │                           → emit authorizationUrlOpened()
  │
  ├── postAuthCode(code)  → OAuthMastodon::requestAccessToken()   [async]
  │                           → connect(reply, &finished, lambda)
  │                           → emit accessTokenReceived()
  │                           → MastodonClient::authenticated = true
  │
  └── postStatus(text)    → QMastodonPostStatus::postStatus()     [async]
                              → connect(reply, &finished, onReplyFinished)
                              → parseJsonFinished() → emit statusPosted()
```

## 主要クラス詳細

### `MastodonClient` (mastodonclient.h/cpp)
- `QML_ELEMENT` マクロにより QML モジュール `Qtdon` に自動登録
- プロパティ:
  - `authenticated` (READ + NOTIFY) — 認証状態
  - `errorMessage` (READ + NOTIFY) — 最新エラーメッセージ
- `Q_INVOKABLE` メソッド: `startAuth()`, `postAuthCode()`, `postStatus()`
- `OAuthMastodon`, `QNetworkAccessManager` のライフサイクルを管理

### `OAuth2` (oauth2.h/cpp)
- `client_id` / `client_secret` を保持
- `generateAuthQuery()` — 認可URLのクエリパラメータ生成
- `generateTokenRequestData()` — トークンリクエストの POST body 生成
- `generateBearerHeader()` — `static` メソッド、Bearer ヘッダー生成
- **注意:** `CLIENT_ID` / `CLIENT_SECRET` は `oauth2.cpp` 内のプレースホルダー（`***`）

### `OAuthMastodon` (oauthmastodon.h/cpp)
- `OAuth2` を継承。完全非同期の認証フロー
- `requestAuthorization()` — ブラウザで認可URL を開く
- `requestAccessToken()` — ラムダ接続による非同期トークン取得（`QEventLoop` 不使用）
- シグナル: `authorizationUrlOpened()`, `accessTokenReceived()`, `errorOccurred()`

### `QMastodonNetBase` (qmastodonnetbase.h/cpp)
- Mastodon API リクエストの抽象基底クラス
- `ErrorCode` を `enum class` + `Q_ENUM` で定義
- `onReplyFinished()` — HTTP ステータスに基づくエラー分類（UI 非依存）
- `parseJsonFinished()` 純粋仮想関数

### `QMastodonPostStatus` (qmastodonpoststatus.h/cpp)
- `POST /api/v1/statuses` を実装
- パラメータ: `status`, `in_reply_to_id`, `media_ids[]`, `sensitive`, `spoiler_text`, `visibility`
- 新connect構文でシグナル接続

## ビルド方法

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x
cmake --build .
```

## コーディング規約

- **C++17** — `auto`, `[[nodiscard]]`, range-for, structured bindings を活用
- **nullptr** — ポインタ初期化は一律 `nullptr`
- **新connect構文** — `connect(obj, &Class::signal, ...)` のみ使用（`SIGNAL()/SLOT()` マクロ禁止）
- **非同期パターン** — `QEventLoop` による同期的ブロック禁止。`connect` + ラムダで処理
- **UI 分離** — C++ 層での `QMessageBox` 等のダイアログ使用禁止。エラーはシグナルで QML に通知
- **QML 型登録** — `QML_ELEMENT` マクロ使用。`setContextProperty` 禁止
- **文字列リテラル** — `QStringLiteral()` を使用
- **メンバ初期化** — クラス内デフォルトメンバ初期化子（`= nullptr`, `= false` 等）を使用

## 新規 API エンドポイント追加手順

1. `QMastodonNetBase` を継承した新クラスを作成
2. API リクエストメソッドを実装し、`connect(reply, &QNetworkReply::finished, this, &Class::onReplyFinished)` で接続
3. `parseJsonFinished()` を `override` してレスポンスを処理
4. `MastodonClient` に `Q_INVOKABLE` メソッドとシグナルを追加
5. `CMakeLists.txt` の `qt_add_executable` に新ファイルを追加

## 既知の制限・TODO

- `oauth2.cpp` の `CLIENT_ID` / `CLIENT_SECRET` がプレースホルダー — 外部設定への移行が必要
- タイムライン表示・通知など読み取り系 API は未実装
- `content/AuthWindow.qml` が未実装
- アクセストークンの永続化（`QSettings` 等）が未実装
- ユニットテスト未整備
