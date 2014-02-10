SigUtil (現在整理中)
=======
自分用C++ユーティリティ(template ライブラリ).   
boostの併用推奨(主にoptionalのため).　なくても動作はしますが無効値は実装定義.

～おしながき～ 

\<tool.hpp> 
* class SimpleRandom: 初期化時に指定した範囲の一様分布乱数を発生(データ型指定、値域指定、乱数生成アルゴリズム指定)
  * RandomUniqueNumbers: 重複の無い一様分布の整数乱数を生成(値域指定、個数指定)
* class TimeWatch: 高機能ストップウォッチ(ラップ、スプリット、一時停止、複数保存)
* class Histgram: ヒストグラム(データ型指定、ビン数指定、値域指定、出力・保存機能)
* class Percent: パーセント型

\<file.hpp>
* GetFileNames: 指定ディレクトリにあるファイル名を取得(option:隠しファイル識別、拡張子指定)
* GetFolderNames: 指定ディレクトリにあるフォルダ名を取得(option:隠しファイル識別、拡張子指定)
* SaveLine: お手軽にファイルへ1行ずつ保存 
  * SaveNum: SaveLineの数値版(改行のかわりにデリミタ指定可)
* ReadLine: お手軽にファイルから1行ずつ読み込み
  * ReadNum: ReadLineの数値版(改行のかわりにデリミタ指定可)

\<functional.hpp>
* Map: 1引数高階関数　(a -> b) -> [a] -> [b]
* ZipWith: 2引数高階関数　(a -> b -> c) -> [a] -> [b] -> [c]
* HigherOrderFunction: n引数高階関数 (上記の一般化)
* Zip: タプルを作る　[a] -> [b] -> ... -> [(a, b, ...)]
* Fill: 値を複製したコンテナを返す　uint -> a -> [a]
* Reverse: コンテナの要素を逆転させたコンテナを返す　[a] -> [a]
* Merge: コンテナの結合　[a] -> [b] -> [c]
* Take: コンテナの先頭からn個を取り出したコンテナを返す uint -> [a] -> [a]
* Drop: コンテナの先頭からn個を削除したコンテナを返す　uint -> [a] -> [a]

\<string.hpp> 

\<eraser.hpp>


