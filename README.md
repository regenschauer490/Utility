SigUtil (version 1.00)
=======
汎用性と使いやすさを重視したC++11ユーティリティ(template ライブラリ).   
コンテナに対してはboost::rangeのようにイテレータを介さず使用できる.  
Linux環境ではboostの併用推奨(optional, regex, filesystem 等のため). boost無しの場合、一部使用できない機能あり.

[動作確認環境]
* Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)
* g++ (Ubuntu 4.8.1-2ubuntu1~12.04) 4.8.1
* Ubuntu clang version 3.4-1~exp1 (trunk) (based on LLVM 3.4)

#### ～おしながき～   
**\<array.hpp>**
* class array: std::arrayの利便性を拡張し、固定長なstd::vectorの感覚で使用できる静的配列

**\<functional.hpp>**
　関数型プログラミング支援
* map: 1引数高階関数 (a -> b) -> [a] -> [b]
* zipWith:  2引数高階関数 (a -> b -> c) -> [a] -> [b] -> [c]
* variadicZipWith: map,zipWith の可変長化　(a -> b -> ...) -> [a] -> [b] -> ...
* foldl: コンテナの先頭からたたみ込み (a -> b -> a) -> a -> [b] -> a
* foldr: コンテナの末尾からたたみ込み (a -> b -> b) -> b -> [a] -> b
* zip: 複数のコンテナから、タプルのコンテナを作る　[a] -> [b] -> ... -> [(a, b, ...)]
* zip: コンテナのタプルから、タプルのコンテナを作る　([a], [b], ...) -> [(a, b, ...)]
* unzip: タプルのコンテナから、リストのタプルを作る　[(a, b, ...)] -> ([a], [b], ...)
* replicate: 値を複製したコンテナを返す　uint -> a -> [a]
* seq: 等差数列作成(初項, 公差, 項数)　a -> b -> uint -> [common_type(a,b)]
* reverse: コンテナの要素を逆転させたコンテナを返す　[a] -> [a]
* merge: コンテナの結合　[a] -> [a] -> [a]
* take: コンテナの先頭からn個を取り出したコンテナを返す uint -> [a] -> [a]
* drop: コンテナの先頭からn個を削除したコンテナを返す　uint -> [a] -> [a]
* sort: 任意の比較関数でソートしたコンテナを返す　(a -> a -> bool) -> [a] -> [a]

※対応コンテナ：STL (vector, deque, list, set, multiset, unordered\_set, unordered\_multiset), sig::array

**\<iteration.hpp>**
　かゆいところに手が届く反復処理関数
* for\_each: std::for\_eachの可変長版(複数コンテナを反復処理)
* for\_each: 上記に加えて、添字の設定と使用もできる
* compound\_assignment: コンテナへの代入演算を述語関数を指定して行う

**\<calculation.hpp>**
　スカラ変数とベクトル変数(+コンテナの種類)を気にせず使える演算関数
* plus: 汎用的な加算関数
* minus: 汎用的な減算関数
* multiplies: 汎用的な乗算関数
* divides: 汎用的な除算関数
* sum: 総和を求める関数
* product: 総乗を求める関数

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
* cat\_str: コンテナに格納された全文字列を結合して1つの文字列にする(区切り文字の指定可)
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
* class TagDealer: HTML風にタグをエンコード・デコード
* class ZenHanReplace: 全角・半角文字の置換処理を行う (アルファベット, カタカナ, 数字)

**\<file.hpp>**
　入出力関連
* get\_file\_names: 指定ディレクトリにあるファイル名を取得(option:隠しファイル識別、拡張子指定)
* get\_folder\_names: 指定ディレクトリにあるフォルダ名を取得(option:隠しファイル識別、拡張子指定)
* file_clear: ファイル内容の初期化
* save\_line: 文字列or文字列のコンテナを渡し、1行ずつ保存 
* save\_num: 数値or数値のコンテナを渡し、改行やデリミタで区切って保存(行列形式の保存も可)
* read\_line: ファイルから文字列を1行ずつ読み込む
* read\_num: ファイルから数値を改行やデリミタを目印に読み込む(行列形式の読み込みも可)

**\<tool.hpp>** 
　便利ツール
* class SimpleRandom: 初期化時に指定した範囲の一様分布乱数を発生(データ型指定、値域指定、乱数生成アルゴリズム指定)
  * random\_unique\_numbers: 重複の無い一様分布の整数乱数を生成(値域指定、個数指定)
* class TimeWatch: 高機能ストップウォッチ(ラップ、スプリット、一時停止、複数保存)
* class Histgram: ヒストグラム(データ型指定、ビン数指定、値域指定、出力・保存機能)
* class Percent: パーセント型

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
