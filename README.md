SigUtil (現在開発中 version 0.95)
=======
汎用性と使いやすさを重視したC++11ユーティリティ(template ライブラリ).   
主にコンテナに対する汎用的な操作と、機械学習や数値計算に便利なモジュールを作成している．
開発は基本的にVisualStudio環境で行っているが、GCCやClang環境での動作確認も行っており移植性も考慮している．
boostの機能に依存している部分が多少あり、boostを使用できない場合には一部使用できない機能あり.

#### [ 動作確認環境 ]
- Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)
- Visual C++ Compiler 2014 (v140)
- Visual C++ Compiler 2015 Preview (v140)
- g++ 4.8
- g++ 4.9
- clang 3.4 (based on LLVM 3.4)

#### [ インストール & テスト ]
ヘッダオンリーなのでSigUtil/libフォルダにパスを通して#includeするだけで利用できます. 
デフォルトではboostを使用する設定になっていますが、使用しない場合は次の処理を行ってください．  
* SigUtil/lib/sigutil.hpp を開いて「SIG\_ENABLE\_BOOST」の値をboost使用時は1、未使用時は0に設定.

動作確認のテストは、以下の操作を行ってください．(xxxはコンパイラのバージョン)
* VisualStudio環境: VisualStudio2013(November2013 CTP)以降で project/VisualStudio xxx/SigUtil.sln を起動してコンパイル+実行.
* GCC環境: ターミナルで「project/gcc」下に移動し、「make -f gcc.mk」を実行 (事前に gcc.mk のINCLUDEやLIBRARYパスは環境に合わせて修正して下さい).コンパイルに成功した後は、「./bin/test」でテストを実行.
* Clang環境: ターミナルで「project/clang」下に移動し、「make -f clang.mk」を実行 (事前に clang.mk のINCLUDEやLIBRARYパスは環境に合わせて修正して下さい). コンパイルに成功した後は、「./bin/test」でテストを実行.

* SigUtil/main.cpp にテスト項目が列挙されています. テスト内容やライブラリの使用例は SigUtil/example/\*\*\*.cpp に記述されています.

#### [ おしながき ]  
詳細なリファレンスは http://regenschauer490.github.io/Utility/ を参照．以下は一覧と概要です．
 
**\<array.hpp>**
* class array: std::arrayの利便性を拡張し、固定長なstd::vectorの感覚で使用できる静的配列

**\<functional.hpp>**
　関数型プログラミング支援
* map: 1引数高階関数 (a -> b) -> [a] -> [b]
* zipWith:  2引数高階関数 (a -> b -> c) -> [a] -> [b] -> [c]
* variadicZipWith: map,zipWith の可変長化　(a -> b -> ...) -> [a] -> [b] -> ...
* foldl: コンテナの先頭からたたみ込み (a -> b -> a) -> a -> [b] -> a
* foldr: コンテナの末尾からたたみ込み (a -> b -> b) -> b -> [a] -> b
* filter: コンテナから指定条件を満たす要素を抽出する　(a -> Bool) -> [a] -> [a]
* partition: コンテナから指定条件を満たす要素とそれ以外要素をそれぞれ抽出する　(a -> Bool) -> [a] -> ([a], [a])
* zip: 複数のコンテナから、タプルのコンテナを作る　[a] -> [b] -> ... -> [(a, b, ...)]
* zip: コンテナのタプルから、タプルのコンテナを作る　([a], [b], ...) -> [(a, b, ...)]
* unzip: タプルのコンテナから、リストのタプルを作る　[(a, b, ...)] -> ([a], [b], ...)
* replicate: 値を複製したコンテナを返す　uint -> a -> [a]
* seqn: 等差数列作成(初項, 公差, 項数)　a -> b -> uint -> [common_type(a,b)]
* reverse: コンテナの要素を逆転させたコンテナを返す　[a] -> [a]
* merge: コンテナの結合　[a] -> [a] -> [a]
* take: コンテナの先頭からn個を取り出したコンテナを返す uint -> [a] -> [a]
* drop: コンテナの先頭からn個を削除したコンテナを返す　uint -> [a] -> [a]
* sort: 任意の比較関数でソートしたコンテナを返す　(a -> a -> bool) -> [a] -> [a]

**\<iteration.hpp>**
　かゆいところに手が届く反復処理関数
* for\_each: std::for\_eachの可変長版(複数コンテナを反復処理)
* for\_each: 上記に加えて、添字の設定と使用もできる
* compound_assignment: コンテナへの代入演算

**\<calculation.hpp>**
　スカラ変数とベクトル変数を統一的にelement-wiseに計算する関数．および統計処理関数
* plus: 汎用的な加算関数
  * operator+: 演算子版
* minus: 汎用的な減算関数
  * operator-: 演算子版
* mult: 汎用的な乗算関数
  * operator*: 演算子版
* div: 汎用的な除算関数
  * operator/: 演算子版
* sum: 総和を求める関数（各要素に関数適用も可能）
  * sum_row: 2次元配列形式のコンテナの行の総和を求める
  * sum_col: 2次元配列形式のコンテナの列の総和を求める
* product: 総乗を求める関数（各要素に関数適用も可能）
  * product_row: 2次元配列形式のコンテナの行の総乗を求める
  * product_col: 2次元配列形式のコンテナの列の総乗を求める
* average: 平均を求める関数
* variance: 分散を求める関数
* normalize: 正規化を行う関数
* standardize: 標準化を行う関数
* normalize_dist: 確率分布の正規化を行う関数

**\<modify.hpp>**
　コンテナに対する変更操作
* sort: 標準ソート関数のラッパ
* sort\_with\_index: ソート前の位置を保持してソート (ex: [30, 50, -10, 0] -> ([-10, 0, 30, 50], [2, 3, 0, 1]))
* shuffle: 複数のコンテナの要素を対応させながらシャッフル (ex: c1[1, 2, 3], c2[1, 2, 3] -> c1'[3, 1, 2], c2'[3, 1, 2])
* remove\_duplicates: コンテナの要素から重複したものを削除
* remove\_one: コンテナから指定要素を1つだけ削除
 * remove\_one\_if: 要素のかわりに述語関数で判別
* remove\_all: コンテナから指定要素を全削除
 * remove_\all\_if: 要素のかわりに述語関数で判別

**\<string.hpp>** 
　文字列処理関連
* regex\_search: regex\_search(std::,boost::)のラッパ関数.探索結果を添字アクセスできる形([ ][ ])で取得
 * escape\_regex: 正規表現の特殊文字をエスケープ
 * make\_regex: エスケープ処理を行い、regex or wregexを返す (外部取得したテキスト内の文字を使って検索する時などに)
* split: 文字列をある文字列(デリミタ)を目印に分割する
* cat: コンテナに格納された全文字列を結合して1つの文字列にする(区切り文字の指定可)
* wstr\_to\_str: ワイド文字 -> マルチバイト文字 (ex: Windows環境では UTF-16 -> Shift-JIS)
* str\_to\_wstr: マルチバイト文字 -> ワイド文字 (ex: Windows環境では Shift-JIS -> UTF-16)
* utf8\_to\_utf16: UTF-8 -> UTF-16
* utf16\_to\_utf8: UTF-16 -> UTF-8
* utf8\_to\_utf32: UTF-8 -> UTF-32
* utf32\_to\_utf8: UTF-32 -> UTF-8
* sjis\_to\_utf16: ShiftJIS -> UTF-16
* utf16\_to\_sjis: UTF-16 -> ShiftJIS
* sjis\_to\_utf8: ShiftJIS -> UTF-8
* utf8\_to\_sjis: UTF-8 -> ShiftJIS
* class ZenHanReplace: 全角・半角文字の置換処理を行う (アルファベット, カタカナ, 数字)

**\<file.hpp>**
　入出力関連
* get\_file\_names: 指定ディレクトリにあるファイル名を取得(option:隠しファイル識別、拡張子指定)
* get\_folder\_names: 指定ディレクトリにあるフォルダ名を取得(option:隠しファイル識別、拡張子指定)
* clear\_file: ファイル内容の初期化
* save\_line: 文字列or文字列のコンテナを渡し、1行ずつ保存 
* save\_num: 数値or数値のコンテナを渡し、改行やデリミタで区切って保存(行列形式の保存も可)
* load\_line: ファイルから文字列を1行ずつ読み込む
* load\_num: ファイルから数値を改行やデリミタを目印に読み込む
  * load_num2d 行列形式の数値の読み込み

**\<tool.hpp>** 
　便利ツール
* class SimpleRandom: 初期化時に指定した範囲の一様分布乱数を発生(データ型指定、値域指定、乱数生成アルゴリズム指定)
  * random\_unique\_numbers: 重複の無い一様分布の整数乱数を生成(値域指定、個数指定)
* class ManageConvergence: 反復計算における収束判定と管理を行うクラス
  * class ManageConvergenceSimple: 収束判定の管理を行うクラス
* class TimeWatch: 高機能ストップウォッチ(ラップ、スプリット、一時停止、複数保存)
* class Histgram: ヒストグラム(データ型指定、ビン数指定、値域指定、出力・保存機能)
* class Percent: パーセント型

**\<distance.hpp>** 
　距離・類似度関数
* norm: Pノルム(L1, L2,...), 最大ノルム
* minkowski_distance: ミンコフスキー距離(マンハッタン距離, ユークリッド距離,...)
* canberra_distance: キャンベラ距離
* binary_distance: バイナリ距離
* cosine_similarity: コサイン類似度
* KL_divergence: KL情報量
* JS_divergence: JS情報量

**\<helper.hpp>**
　雑多な補助モジュール
* And: constexpr 可変長and
* Or: constexpr 可変長or
* Xor: constexpr xor
* Consistency: constexpr !xor
* min: constexpr可変長min
* max: constexpr可変長max
* greater: constexpr 大小比較
* less: constexpr 大小比較
* abs_delta: constexpr 差の絶対値を求める
* equal: 厳密な計算でない場合に使用する簡易等値比較 (浮動小数点型の誤差をある程度許容)
* equal\_tolerant: 指定範囲内の誤差を許した等値比較
* check\_range: 範囲チェック (min ≦ val ≦ max)
* modify\_range: 範囲自動修正 (min ≦ val ≦ max)

**\<container_helper.hpp>**
　主に可変長なコンテナやイテレータに対する補助モジュール
* iterative_make: 複数のイテレータに対して、loop回数だけ繰り返しデリファレンス+関数適用して結果をdestに格納
* iterative_assign: 複数のイテレータに対して、loop回数だけ繰り返しデリファレンス+関数適用(副作用あり)
* struct has\_random\_access\_op: ランダムアクセス演算子([ ])をメンバに持っているか判定
* erase: コンテナ毎のerase処理の差異を吸収するための関数
* copy: 別のコンテナに要素をコピーする

※対応コンテナ：STL (vector, deque, list, set, multiset, unordered\_set, unordered\_multiset), sig::array
 一部制限あり：STL（map, multimap, unordered_map, unordered_multimap）
