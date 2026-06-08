# Akexi

## Introduction

Do you use forms, queries, and reports when managing your databases? They are incredibly convenient, right?

But haven't you noticed that these human-centric frontend features often get in the way when you try to read or write that same data using your own custom scripts or external tools? Why are "features for humans" and "data for computers" locked together in a single, inseparable file?

Akexi saves your UI definitions (forms, queries, reports) in a dedicated `*.akexi` file, while your actual table data lives purely in a standard, universally accessible `*.sqlite3` file. You get the absolute best of both worlds: a highly productive, beautiful GUI for human data entry, and raw, unrestricted SQLite access for your scripts (Rust, Python, etc...), web apps, or any other external tools working right alongside it.

(Our choice of Kexi as a foundation comes from a very specific journey. We initially considered LibreOffice Base, and while it was technically capable of decoupling a raw SQLite3 database via JDBC, we struggled to adapt to its design approach. Because it inherits from Writer's document layout, you are forced to design user interfaces using centimeters, and a barrage of separate windows constantly pops up for every single task. We simply wanted a clean, cohesive, single-window workspace.)

## Background and Philosophy

I am currently building a personal daily life management database called "[VitalLedger](https://github.com/akenejie/VitalLedger)." However, after writing the SQL schemas, I realized I needed a proper GUI to interact with it—which is the exact reason I created Akexi.

My development approach is essentially "dogfooding." I am developing Akexi by using it in my own daily life to manage VitalLedger, tweaking and fixing the parts that feel inconvenient to me as I go.

## Technical Details

Akexi is a fork of **Kexi 3.2**.
It supports both the new `*.akexi` format and legacy `*.kexi` files (which strictly adhere to the Kexi 3.2 format). To prevent conflicts with existing Kexi installations, all internal Kexi plugin IDs (`org.kexi-project.*`) have been safely renamed to `akenejie.akexi-project.*`.

### The `*.akexi` Project (Metadata) File

The `*.akexi` file is actually an SQLite database dedicated solely to the frontend. It contains two system tables prefixed with `akexi__`:

* **`akexi__projectinfo`**: A key-value store for project-level metadata.
* `db_ref_mode` / `db_ref_path`: Determines how the companion `*.sqlite3` data file is located (`0` = auto by filename, `1` = relative path, `2` = absolute path).
* `caption`: The human-readable project name (survives file moves).
* `project_version_major` / `project_version_minor`: The Akexi project file format version (currently `0.0`).


* **`akexi__tablecomments`**: Stores column-level descriptions for user tables. (In the legacy KEXI format, this was mixed into `kexi__fields`). The schema is `(table_name, column_name, comment)` with a composite primary key.

Both tables are generated upon creating a new Akexi project and are automatically created using `IF NOT EXISTS` when opening an existing `*.akexi` file. During a KEXI-to-Akexi conversion, all metadata is migrated into these tables, and the old `kexi__*` / `akexi__*` system tables are completely dropped from the `.sqlite3` data file, leaving your raw data perfectly clean.

## Contributing

Contributions to Akexi are warmly welcomed!
Whether it's bug reports or feature requests, your help is appreciated. Please note that as the author, my native language is Japanese. Therefore, pull requests focused on internationalization (i10n) and translations are incredibly helpful and highly encouraged. Let's make this tool accessible to everyone!

## Contact and Links

* **Akexi Project (Issues, Feature Requests, PRs):** https://github.com/akenejie/akexi
* **Akenejie's Website:** https://akenejie.f5.si/en/

*Original Base Project:*

* Kexi Project home page: [http://kexi-project.org](http://kexi-project.org)
* Kexi within the Calligra Suite: [http://www.calligra.org/kexi](http://www.calligra.org/kexi)

## License and Copyright

Akexi - Visual database applications creator

Akexi is developed by akenejie, bringing a decoupled architecture of frontend (*.akexi) and backend (SQLite3) to the original Kexi project.
**© 2026 アケネＪ / Akenejie**

Based on Kexi, developed by the Kexi Team - an international group of independent developers. They form a part of the Calligra Project.
**© 2002-2019, The Kexi Team**
[https://www.kexi-project.org](https://www.kexi-project.org)

**License:**
Akexi is distributed under the GNU Affero General Public License Version 3 (AGPLv3).
Original Kexi components remain under their respective licenses (GPLv2+, LGPLv2+, BSD).

Have fun,
--- Akenejie

---

# Akexi (日本語)

## はじめに

データベースを操作するとき、フォームやクエリ、レポートといった機能を使っていますか？とても便利ですよね。

でも、そういった「人間のためのフロントエンド機能」が、自作プログラムなどからデータを読み書きしようとした途端、邪魔になった経験はありませんか？なぜ「人間のための機能」と「パソコンのためのデータ」は、ひとまとめに保存されてしまうのでしょうか。この疑問から生まれたのが「Akexi」です。

最大の特徴は、フロントエンドとバックエンドの完全な分離です。画面設計（フォームやクエリなど）は専用の `*.akexi` ファイルに、そして実際のテーブルデータは汎用的な `*.sqlite3` ファイルに保存します。これにより、美しく快適なGUIでデータを直感的に管理しつつ、同じディレクトリにあるSQLite3ファイルを各種プログラミング言語（Rust、Pythonなど）や外部アプリから相対パスでサクッと読み書きできる、圧倒的な自由を手に入れることができます。

（なぜKexiをベースに選んだのかというと、当初はLibreOffice Baseを検討していて、JDBC経由で純粋なsqlite3の分離は出来たものの、Writterの流用によってメートル単位でのGUI設計という仕様や、作業のたびに新しいウィンドウが次々と開く仕様に適応できず、すっきりとした単一ウィンドウの環境が欲しかったからです。）

## 開発の背景とスタンス

現在、私は「[VitalLedger](https://github.com/akenejie/VitalLedger)」という自分の日常生活を管理するためのデータベースを制作しています。しかし、SQLでデータの枠組みまでは作ったものの、それを操作するための使い勝手の良いフロントエンド環境がなく、そのために開発したのがこのAkexiです。

そのため、開発スタンスとしては私自身が日常生活で「VitalLedger」を運用しながら、使っていて不便に感じたところを自分のペースで少しずつ直していく、という形をとっています。

## 技術仕様

Akexiは **Kexi 3.2** のフォークです。
新しい `*.akexi` 形式のファイルに加えて、従来の `*.kexi` 形式のファイルも扱うことができ、保存時はKexi 3.2のフォーマット仕様に準拠して書き込まれます。また、既存のKexi環境との競合を防ぐため、すべてのプラグインID（`org.kexi-project.*`）は `akenejie.akexi-project.*` という名前に変更しています。

### `*.akexi` プロジェクト（メタデータ）ファイルについて

`*.akexi` ファイルの中身は、フロントエンド定義専用のSQLiteデータベースです。内部には `kexi__` プレフィックス以外に新たに `akexi__` プレフィックスを持つ2つのシステムテーブルが含まれています。

* **`akexi__projectinfo`**: プロジェクトレベルのメタデータを保持するKey-Valueストアです。
* `db_ref_mode` / `db_ref_path`: ペアとなるデータ本体（`*.sqlite3`）の参照方法を定義します（`0` = プロジェクトと同一ファイル名から拡張子だけsqlite3にしたファイル、`1` = 相対パス、`2` = 絶対パス）。
* `caption`: プロジェクトの表示名（ファイルを移動しても維持されます）。
* `project_version_major` / `project_version_minor`: Akexiプロジェクトのファイルフォーマットバージョン（現在は `0.0`）。


* **`akexi__tablecomments`**: ユーザーテーブルの列に対する説明（コメント）を保存します。従来のKEXIフォーマットでは `kexi__fields` の中に混在していたものを独立させました。スキーマは複合主キーを持つ `(table_name, column_name, comment)` となっています。

これらのテーブルは新規プロジェクト作成時に生成され、既存の `*.akexi` ファイルを開く際にもファイルがなければ自動生成される仕組みです。従来のKexiファイルからAkexi形式へ変換する処理を行うと、メタデータはこれらのテーブルに移行され、同時にデータ本体の `.sqlite3` ファイルからは `kexi__*` や `akexi__*` といったシステムテーブルが完全にドロップされ、純粋なデータのみが残るようになっています。

## コントリビューション（貢献）について

開発への貢献はいつでも大歓迎です！
バグ報告や機能提案はもちろんですが、作者である私は日本語しか分からないため、世界中の方に使ってもらうには他言語への翻訳・国際化（i10n）が不可欠です。もしよろしければ、翻訳に関するプルリクエストで力を貸していただけると本当に助かります。

## リンクと連絡先

* **Akexi プロジェクト (バグ報告・要望・PRはこちらへ):** https://github.com/akenejie/akexi
* **アケネＪのホームページ:** https://akenejie.f5.si/ja/

*ベースとなったオリジナルプロジェクト:*

* Kexi Project home page: [http://kexi-project.org](http://kexi-project.org)
* Kexi within the Calligra Suite: [http://www.calligra.org/kexi](http://www.calligra.org/kexi)

## ライセンスと著作権
- 英語を参照。
