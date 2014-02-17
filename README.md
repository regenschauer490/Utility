SigUtil (現在整理・拡張中)
=======
汎用性と使いやすさを重視した自分用C++11ユーティリティ(template ライブラリ).   
boostの併用推奨(主にoptionalのため).　なくても動作はしますが無効値は実装定義.

～おしながき～ 

**\<functional.hpp>**
* Map: 1引数高階関数　(a -> b) -> [a] -> [b]
* ZipWith: 2引数高階関数　(a -> b -> c) -> [a] -> [b] -> [c]
* HigherOrderFunction: n引数高階関数 (上記の一般化)
* Zip: 複数のコンテナから、タプルのコンテナを作る　[a] -> [b] -> ... -> [(a, b, ...)]
* Zip: コンテナのタプルから、タプルのコンテナを作る　([a], [b], ...) -> [(a, b, ...)]
* UnZip: タプルのコンテナから、リストのタプルを作る　[(a, b, ...)] -> ([a], [b], ...)
* Repeat: 値を複製したコンテナを返す　uint -> a -> [a]
* ArithSequence: 等差数列を作る (初項, 公差, 項数)
* Reverse: コンテナの要素を逆転させたコンテナを返す　[a] -> [a]
* Merge: コンテナの結合　[a] -> [b] -> [c]
* Take: コンテナの先頭からn個を取り出したコンテナを返す uint -> [a] -> [a]
* Drop: コンテナの先頭からn個を削除したコンテナを返す　uint -> [a] -> [a]
* Sort: 任意の比較関数でソートしたコンテナを返す　(a -> a -> bool) -> [a] -> [a]

**\<string.hpp>** 
* RegexSearch: std::regex_searchのラッパ関数.探索結果を添字アクセスできる形([][])で取得
 * RegexEscaper: 正規表現の特殊文字をエスケープ
 * RegexMaker: エスケープ処理してstd::regex or std::wregexを返す (外部取得したテキスト内の文字を使って検索する時などに)
* Split: 文字列をある文字列(デリミタ)を目印に分割する
* CatStr: コンテナに格納された全文字列を結合して1つの文字列にする(区切り文字の指定可)
* WSTRtoSTR: ワイド文字 -> マルチバイト文字 (ex: Windows環境では UTF-16 -> Shift-JIS)
* STRtoWSTR: マルチバイト文字 -> ワイド文字 (ex: Windows環境では Shift-JIS -> UTF-16)
* class ZenHanReplace: 全角・半角文字の置換処理を行う (アルファベット, カタカナ, 数字)
* class TagDealer: HTML風にタグをエンコード・デコード

**\<modify.hpp>**
* Sort: 標準ソート関数のラッパ (比較関数を指定可)
* SortWithIndex: ソート前の位置を保持してソート (ex: [30, 50, -10, 0] -> ([-10, 0, 30, 50], [2, 3, 0, 1]))
* Shuffle: コンテナの要素をシャッフル
* Shuffle: 複数のコンテナの要素を対応させながらシャッフル (ex: c1[1, 2, 3], c2[1, 2, 3] -> c1'[3, 1, 2], c2'[3, 1, 2])
* RemoveDuplicates: コンテナの要素から重複したものを削除
* RemoveOne: コンテナから指定要素を1つだけ削除
 * RemoveOneIf: 要素のかわりに述語関数で判別
* RemoveAll: コンテナから指定要素を全削除
 * RemoveAllIf: 要素のかわりに述語関数で判別

**\<set.hpp>**

**\<file.hpp>**
* GetFileNames: 指定ディレクトリにあるファイル名を取得(option:隠しファイル識別、拡張子指定)
* GetFolderNames: 指定ディレクトリにあるフォルダ名を取得(option:隠しファイル識別、拡張子指定)
* SaveLine: お手軽にファイルへ1行ずつ保存 
  * SaveNum: SaveLineの数値版(改行のかわりにデリミタ指定可)
* ReadLine: お手軽にファイルから1行ずつ読み込み
  * ReadNum: ReadLineの数値版(改行のかわりにデリミタ指定可)

**\<tool.hpp>** 
* class SimpleRandom: 初期化時に指定した範囲の一様分布乱数を発生(データ型指定、値域指定、乱数生成アルゴリズム指定)
  * RandomUniqueNumbers: 重複の無い一様分布の整数乱数を生成(値域指定、個数指定)
* class TimeWatch: 高機能ストップウォッチ(ラップ、スプリット、一時停止、複数保存)
* class Histgram: ヒストグラム(データ型指定、ビン数指定、値域指定、出力・保存機能)
* class Percent: パーセント型
