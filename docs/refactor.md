# リファクタリング方針書

## 概要

本ドキュメントは、Qtdon プロジェクトに対して実施した全面リファクタリングの方針・変更内容・理由をまとめたものである。

## 実施日

2026年2月28日

---

## 1. ビルドシステムの移行: qmake → CMake

### 変更内容
- `Qtdon.pro`, `qml.qrc`, `deployment.pri` を廃止
- `CMakeLists.txt` を新規作成（`qt_add_qml_module` 使用）

### 理由
- Qt 6 では CMake が公式推奨のビルドシステム
- `qt_add_qml_module` により QML ファイル・C++ 型の登録を一元管理できる
- `qml.qrc` が不要になり、QML リソースの管理が簡潔化

### 削除ファイル
| ファイル | 理由 |
|---|---|
| `Qtdon.pro` | CMake に移行 |
| `qml.qrc` | `qt_add_qml_module` が自動管理 |
| `deployment.pri` | qmake 専用のデプロイ設定 |

---

## 2. QML-C++ バインディングの刷新: Binding → MastodonClient

### 変更内容
- `binding.h / .cpp` を廃止し、`mastodonclient.h / .cpp` を新規作成
- `setContextProperty("mastodon", &binding)` を廃止
- `QML_ELEMENT` マクロで QML モジュールに型として自動登録

### 理由
- `setContextProperty` は Qt 6 で非推奨。型推論やツールサポートが効かない
- `QML_ELEMENT` によりコンパイル時の型チェック・QML Language Server 対応が可能
- `Q_PROPERTY(WRITE)` のみの設計から `Q_INVOKABLE` + `Q_PROPERTY(READ/NOTIFY)` の双方向通信に改善

### API の変化
| 旧 (Binding) | 新 (MastodonClient) |
|---|---|
| `Q_PROPERTY(QString startAuth WRITE ...)` | `Q_INVOKABLE void startAuth(const QString&)` |
| `Q_PROPERTY(QByteArray postAuthCode WRITE ...)` | `Q_INVOKABLE void postAuthCode(const QString&)` |
| `Q_PROPERTY(QByteArray toot WRITE ...)` | `Q_INVOKABLE void postStatus(const QString&)` |
| — | `Q_PROPERTY(bool authenticated READ/NOTIFY)` |
| — | `Q_PROPERTY(QString errorMessage READ/NOTIFY)` |

---

## 3. 非同期化: QEventLoop の排除

### 変更内容
- `OAuthMastodon::requestAccessToken()` から `QEventLoop` + `QTimer` による同期ブロックを削除
- `connect(reply, &QNetworkReply::finished, this, [lambda])` による完全非同期に変更

### 理由
- `QEventLoop` はイベントループの再入を引き起こし、予期しないシグナル処理・クラッシュの原因になる
- GUI スレッドをブロックするため、タイムアウト中はアプリ全体がフリーズする
- Qt の推奨パターンに反する

### 旧コード（概要）
```cpp
QEventLoop q;
QTimer t;
t.setSingleShot(true);
QNetworkReply* reply = m_networkManager->post(request, postData);
connect(reply, SIGNAL(finished()), &q, SLOT(quit()));
t.start(5000);
q.exec();  // ← ブロック
```

### 新コード（概要）
```cpp
QNetworkReply *reply = m_networkManager->post(request, postData);
connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    reply->deleteLater();
    // ... レスポンス処理 ...
    emit accessTokenReceived();
});
```

---

## 4. UI とロジックの分離: QMessageBox の全面除去

### 変更内容
- `OAuthMastodon`, `QMastodonNetBase`, `QMastodonPostStatus` から全ての `QMessageBox` コードを削除
- エラーはシグナル (`errorOccurred`, `error`) で通知し、QML 側で表示

### 理由
- C++ 層での UI 表示は Qt Quick アプリケーションのアーキテクチャに反する
- `QMessageBox` は `QtWidgets` モジュールに依存し、`QGuiApplication` では使用不可
- エラーハンドリングをプレゼンテーション層から分離することでテスタビリティが向上

### 削除した QMessageBox 使用箇所
| クラス | 用途 |
|---|---|
| `OAuthMastodon::requestAccessToken()` | タイムアウト通知 |
| `OAuthMastodon::popUpError()` | ネットワークエラー通知 |
| `QMastodonNetBase::reply()` | レスポンスデータ表示 |
| `QMastodonPostStatus::parseJsonFinished()` | 投稿成功通知 |

---

## 5. Qt Widgets 依存の除去

### 変更内容
- `main.cpp` で `QApplication` → `QGuiApplication` に変更
- `QT += widgets` 依存を完全除去

### 理由
- Qt Quick アプリケーションでは `QGuiApplication` で十分
- `QtWidgets` モジュールへの不要な依存を排除し、バイナリサイズとビルド時間を削減

---

## 6. C++17 モダン化

### 一覧

| 項目 | 旧 | 新 |
|---|---|---|
| ポインタ初期化 | `= 0` / `= NULL` | `= nullptr` |
| 仮想関数 | (未指定) | `override` |
| getter | (未指定) | `[[nodiscard]]` |
| connect 構文 | `SIGNAL()/SLOT()` | `&Class::signal` 形式 |
| enum | `enum ErrorCode { ... }` | `enum class ErrorCode { ... }` + `Q_ENUM` |
| 文字列 | `""` リテラル | `QStringLiteral()` |
| イテレータ | `QMapIterator` / `QListIterator` | range-for |
| メンバ初期化 | コンストラクタ内 | クラス内デフォルトメンバ初期化子 |
| クラス名 | `oauth2` (小文字) | `OAuth2` (PascalCase) |

---

## 7. QML UI の再構成

### 変更内容
- `main.qml` → `Main.qml` にリネーム（QML 型名規約: 大文字始まり）
- 絶対座標配置から `ColumnLayout` / `RowLayout` ベースのレスポンシブレイアウトに変更
- `MastodonClient` を QML 内で直接インスタンス化（`import Qtdon`）
- `Connections` で認証完了時にポップアップを自動クローズ
- `errorMessage` プロパティのリアクティブ表示を追加

### 理由
- `loadFromModule()` で使用するには QML ファイル名が大文字始まりである必要がある
- Layout ベースの設計はウィンドウリサイズに自然に対応する
- `setContextProperty` 不要で、型安全な QML コードになる

---

## 8. ファイル構成の変化

### 削除
| ファイル | 理由 |
|---|---|
| `binding.h / .cpp` | `MastodonClient` に置き換え |
| `Qtdon.pro` | CMake 移行 |
| `qml.qrc` | `qt_add_qml_module` が代替 |
| `deployment.pri` | qmake 専用 |
| `main.qml` | `Main.qml` にリネーム |

### 新規
| ファイル | 役割 |
|---|---|
| `CMakeLists.txt` | CMake ビルド定義 |
| `mastodonclient.h / .cpp` | QML ファサード（`QML_ELEMENT`）|
| `Main.qml` | メイン UI（リネーム + 全面書き換え）|

### 改修
| ファイル | 主な変更 |
|---|---|
| `oauth2.h / .cpp` | クラス名 `oauth2` → `OAuth2`、`[[nodiscard]]`、`QStringLiteral` |
| `oauthmastodon.h / .cpp` | `QEventLoop` 廃止、`QMessageBox` 廃止、新 connect 構文 |
| `qmastodonnetbase.h / .cpp` | `enum class`、`QMessageBox` 廃止、`nullptr` |
| `qmastodonpoststatus.h / .cpp` | `QStringList`、range-for、新 connect 構文 |
| `main.cpp` | `QGuiApplication`、`loadFromModule` |
| `.gitignore` | `build/` ディレクトリ、Qt 生成ファイル追加 |
