#ifndef DOUBLEARRAY_H
#define DOUBLEARRAY_H

/**
 * DoubleArray<br/>
 * 要素を1バイトとして処理している。<br/>
 * データ構造構築後、追加処理には対応していない。<br/>
 * 検索結果のデータについては、呼び出し側で管理してもらい、<br/>
 * ダブル配列内ではメモリ管理はしない。<br/>
 * また、Codeの配列はない。文字列の1byteをそのまま使用している
 *
 * @briefダブル配列
 * @file DoubleArray.h
 * @author dev.atsushi.kanda@gmail.com
 * @date 2018/01/014
 */

#include <string>
#include <string.h>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <limits>

class TrieParts;
class TrieLayerData;
class DASearchParts;
class ByteArrayData;
class ByteArrayDatas;

/** DoubleArrayの構築&検索 */
class DoubleArray
{
  using TrieArray = std::vector<std::vector<TrieLayerData>>;

public:
  static constexpr int I_NO_ERROR         = 0x00; /* Normal                     */
  static constexpr int I_FAILED_TRIE      = 0x01; /* failed to create TRIE      */
  static constexpr int I_FAILED_MEMORY    = 0x02; /* Memory関連ERROR            */
  static constexpr int I_FAIELD_FILE_IO   = 0x04; /* FILE ERROR                 */
  static constexpr int I_NO_OPTION        = 0x00; /* no option                  */
  static constexpr int I_SPEED_PRIORITY   = 0x01; /* create speed priority      */
  static constexpr int I_TAIL_UNITY       = 0x02; /* 検索結果をtrue/falseに変換 */
  static constexpr int64_t I_HIT_DEFAULT  = 0x01; /* 検索結果統合時の返り値     */
  static constexpr int64_t I_SEARCH_NOHIT = 0x00; /* search no result           */

  static constexpr int I_ARRAY_NO_DATA      =  -1;  /* 配列初期値          */
  static constexpr int I_EXTEND_MEMORY      =   2;  /* 配列拡張倍率        */
  static constexpr int I_EXTEND_TRIE        =   4;  /* TRIE拡張倍率        */
  static constexpr int I_DEFAULT_ARRAY_SIZE = 256;  /* defaultの配列サイズ */
  static constexpr char C_TAIL_CHAR         = 0x00; /* TAILの末尾文字      */
  static constexpr size_t I_TRIE_TAIL_VALUE = std::numeric_limits<size_t>::max();

public:
  /** init only */
  DoubleArray();

  /** delete memory */
  ~DoubleArray();

  /** DoubleArrayを構築する
  * @param add_datas DoubleArray構築データ
  * @param i_option  構築オプション
  * @return 0 : 正常終了  0以外 : 異常終了
  */
  int createDoubleArray(
    ByteArrayDatas& add_datas,
    const int i_option = I_SPEED_PRIORITY);

  /** 検索する
  * c_byteにはNULLが途中に含まれる可能性がある為、
  * バイト長も渡さないとダメ。
  * 引数のバイト列末尾のNULLも使用する
  * @param c_byte        search bytes
  * @param i_byte_length search data length
  * @return search result
  */
  int64_t search(
    const char* c_byte,
    const uint64_t i_byte_length) const noexcept;

  /** 途中経過状態を取得しながら検索する
  * @param sarch_parts   search position
  * @param result        search result
  * @param c_byte        search bytes
  * @param i_byte_length search data length
  * @return true : 続きがある  false : 続きが無い
  */
  bool searchContinue(
    DASearchParts& search_parts,
    int64_t& result,
    const char* c_byte,
    const uint64_t i_byte_length) const noexcept;

  /** DoubleArray情報を書き込む
  * @param i_write_size 書き込んだデータサイズ
  * @param fp           OutputFileStream
  * @return I_DA_NO_ERROR : 正常終了 0以外 : 異常終了
  */
  int writeBinary(
    int64_t& i_write_size,
    FILE* fp) const;

  /** DoubleArray情報を読み込む
  * @param i_read_size 読み込んだデータサイズ
  * @param fp          InputFileStream
  * @return I_DA_NO_ERROR : 正常終了 0以外 : 異常終了
  */
  int readBinary(
    int64_t& i_read_size,
    FILE* fp);

  /** 内部データを取得する
  * @param i_array_size       配列サイズ
  * @param i_tail_char_size   Tail文字列サイズ
  * @param i_tail_result_size Tail結果サイズ
  * @param i_base             Base配列
  * @param i_check            Check配列
  * @param i_tail_result      Tail結果配列
  * @param c_tail_char        Tail文字配列
  * @return
  */
  void getDoubleArrayData(
    uint64_t& i_array_size,
    uint64_t& i_tail_char_size,
    uint64_t& i_tail_result_size,
    const int*& i_base,
    const int*& i_check,
    const int64_t*& i_tail_result,
    const char*& c_tail_char) const;

  /** 内部データを外部から設定する
  * @param i_array_size       配列サイズ
  * @param i_tail_char_size   Tail文字列サイズ
  * @param i_tail_result_size Tail結果サイズ
  * @param i_base             Base配列
  * @param i_check            Check配列
  * @param i_tail_result      Tail結果配列
  * @param c_tail_char        Tail文字配列
  * @return
  */
  int setDoubleArrayData(
    int i_array_size,
    int i_tail_char_size,
    int i_tail_result_size,
    const int* i_base,
    const int* i_check,
    const int64_t* i_tail_result,
    const char* c_tail_char);

  /** データが作成されているかチェック
  * @param
  * @return true : データ作成されてる  false : 空
  */
  bool checkInit() const;

  /** 結果IndexからByte情報を復元する
  * @param c_info         復元したByte情報 呼び出し側でdeleteする
  * @param i_result_index 復元する結果Index
  * @return
  */
  void reproductionFromIndex(
    char*& c_info,
    const int64_t i_result_index) const;

private:
  /** メモリ確保
  * @param b_init_size サイズを初期化するか
  * @return Error Code
  */
  int keepMemory(
    const bool b_init_size = false);

  /** メモリ破棄
  * @param b_init_size サイズを初期化するか
  * @return
  */
  void deleteMemory(
    const bool b_init_size = false);

  /** BaseCheckのメモリ拡張
  * @param i_extend_size 拡張する配列サイズ
  * @return Error Code
  */
  int baseCheckExtendMemory(
    const int i_extend_size = 0);

  /** Tailのメモリ拡張
  * @param
  * @return Error Code
  */
  int tailExtendMemory();

  /** Memoryサイズを最適化する
  * @param i_tail_last_index Tail配列のデータが格納されている最終Index
  * @return Error Code
  */
  int optimizeMemory(
    const int i_tail_last_index);

  /** 入力データからTRIE構造を構築する
  * @param trie_array       構築したTRIE構造
  * @param byte_array_datas 基にするデータ
  * @return
  */
  void createTrie(
    TrieArray& trie_array,
    const ByteArrayDatas& byte_array_datas) const noexcept;

  /** Trie構造から再帰的にDoubleArrayを構築する
  * @param i_tail_index     書き込み開始TailIndex
  * @param base_value_array BaseValueの値を決定するのに使用
  * @param trie_array       TRIE全データ
  * @param i_base_index     基準のBaseCheckIndex
  * @param i_trie_index     対象のTrieノード群
  * @param i_option         構築オプション
  * @return Error Code
  */
  int recursiveCreateDoubleArray(
    int& i_tail_index,
    unsigned int* base_value_array,
    TrieArray& trie_array,
    const int i_base_index,
    const size_t i_trie_index,
    const int i_option) noexcept;

  /** Baseの値を求める
  * @param i_base_value     求めたBase値
  * @param base_value_array BaseValueの値を決定するのに使用
  * @param i_option         構築オプション
  * @param tries            TRIEでの同列情報
  * @return Error Code
  */
  int getBaseValue(
    unsigned int& i_base_value,
    unsigned int* base_value_array,
    const int i_option,
    const std::vector<TrieLayerData>& tries) noexcept;

  /** Tailに情報を設定する
  * @param i_tail_index Tail格納開始位置
  * @param trie_parts   TrieParts
  * @return Error code
  */
  int setTailInfo(
    int& i_tail_index,
    const TrieParts* trie_parts) noexcept;

  /** 重複Index情報を作成
  * @param tail_positions Tailに格納する文字列Index
  * @param datas          全追加データ
  * @return
  */
  void createOverlapPositions(
    std::vector<uint64_t>& tail_positions,
    const ByteArrayDatas& datas) const noexcept;

private:
  /** BASE配列 */
  int* i_base_;

  /** CHECL配列 */
  int* i_check_;

  /** TAIL文字配列 */
  char* c_tail_char_;

  /** TAIL結果配列 */
  int64_t* i_tail_result_;

  /** 要素数サイズ */
  uint64_t i_array_size_;

  /** Tail文字列サイズ */
  uint64_t i_tail_char_size_;

  /** Tail結果配列サイズ */
  uint64_t i_tail_result_size_;
};

/** 検索経過状態情報 */
class DASearchParts
{
public:
  /** zero clear */
  DASearchParts() : i_base_(0), i_check_(0), i_tail_(0) {}

  /** cost削減のためvirtualは付加しない */
  ~DASearchParts() {}

  /** Copy */
  DASearchParts(const DASearchParts& parts) : i_base_(parts.i_base_), i_check_(parts.i_check_), i_tail_(parts.i_tail_) {}

  /** init */
  void init() {
    i_base_ = 0;
    i_check_ = 0;
    i_tail_ = 0;
  }

public:
  /** base position */
  int i_base_;

  /** check position */
  int i_check_;

  /** tail position */
  int i_tail_;
};

/** Trie構造の任意のLayerデータ */
class TrieLayerData
{
public:
  /** initのみ */
  TrieLayerData(unsigned char c_byte, size_t i_next_trie_index, TrieParts* trie_parts)
    : c_byte_(c_byte), i_next_trie_index_(i_next_trie_index), trie_parts_(trie_parts) {}

  /** cost削減のためvirtualは付加しない */
  ~TrieLayerData() {}

  /** move */
  TrieLayerData(TrieLayerData&& trie_layer_data)
    : c_byte_(trie_layer_data.c_byte_),
      i_next_trie_index_(trie_layer_data.i_next_trie_index_),
      trie_parts_(trie_layer_data.trie_parts_) {}

  /** = operator */
  TrieLayerData& operator = (const TrieLayerData& trie_layer_data)
  {
    c_byte_            = trie_layer_data.c_byte_;
    i_next_trie_index_ = trie_layer_data.i_next_trie_index_;
    trie_parts_        = trie_layer_data.trie_parts_;
    return *this;
  }

public:
  /** LayerでのByte情報 */
  unsigned char c_byte_;

  /** 続きのTrieArrayIndex */
  size_t i_next_trie_index_;

  /** TriePartsInfo */
  TrieParts* trie_parts_;
};

/** Trie Data Parts DoubleArray構築時に使用 */
class TrieParts
{
public:
  TrieParts() : c_tail_(nullptr), i_tail_size_(0), i_result_(0) {}
  TrieParts(
    char* c_tail,
    const uint64_t i_tail_size,
    const int64_t result) : c_tail_(c_tail), i_tail_size_(i_tail_size), i_result_(result) {}

  ~TrieParts()
  {
    if (c_tail_) {
      delete[] c_tail_;
      c_tail_ = 0;
    }
  }

public:
  /** Tailに格納可能な場合に使うデータ */
  char* c_tail_;

  /** Tailのデータサイズ */
  uint64_t i_tail_size_;

  /** 検索データが存在した場合の結果 */
  int64_t i_result_;
};

/** DoubleArray作成する際の1つのデータ構造 */
class ByteArrayData
{
public:
  ByteArrayData() : c_byte_(nullptr), i_byte_length_(0), result_(0) {}

  /** コンストラクタ */
  ByteArrayData(
    const char* c_byte,
    const uint64_t i_byte_length,
    const int64_t result) : i_byte_length_(i_byte_length), result_(result)
  {
    c_byte_ = new char[i_byte_length_ + 1]; /* 終端記号分+1 */
    memcpy(c_byte_, c_byte, i_byte_length_);
    c_byte_[i_byte_length_] = 0x00; /* 終端記号 */
    ++i_byte_length_; /* 終端記号まで含めたLengthにする */
  }

  /** copy */
  ByteArrayData(const ByteArrayData& byte_array) : i_byte_length_(byte_array.i_byte_length_), result_(byte_array.result_)
  {
    if (i_byte_length_) {
      exit(0);
      c_byte_ = new char[i_byte_length_];
      memcpy(c_byte_, byte_array.c_byte_, i_byte_length_);
    }
  }

  /** 何もしない */
  ~ByteArrayData() {}

  /** move */
  ByteArrayData(ByteArrayData&& byte_array) noexcept
  {
    c_byte_        = byte_array.c_byte_;
    i_byte_length_ = byte_array.i_byte_length_;
    result_        = byte_array.result_;
  }

  /** = operator */
  ByteArrayData& operator = (const ByteArrayData& byte_array)
  {
    c_byte_        = byte_array.c_byte_;
    i_byte_length_ = byte_array.i_byte_length_;
    result_        = byte_array.result_;
    return *this;
  }

  /** ByteDataを削除 */
  void deleteByteData()
  {
    if (c_byte_) {
      delete[] c_byte_;
    }
  }

public:
  /** Byte Data */
  char* c_byte_;

  /** Byte Length */
  uint64_t i_byte_length_;

  /** 検索データが存在した場合の結果 */
  int64_t result_;
};

/** DoubleArray構築に使用するData */
class ByteArrayDatas : public std::vector<ByteArrayData>
{
public:
  ByteArrayDatas() {}
  ~ByteArrayDatas()
  {
    for (auto& data : *this) {
      data.deleteByteData();
    }
  }

  /** add Data
  * @param c_byte   byte data
  * @param i_length byte length
  * @param result   result data
  */
  void addData(
    const char* c_byte,
    const uint64_t i_byte_length,
    const int64_t result)
  {
    if (i_byte_length != 0) {
      push_back(std::move(ByteArrayData(c_byte, i_byte_length, result)));
    }
  }

  /** sort */
  void sort()
  {
    std::sort(begin(), end(), [] (const ByteArrayData& first, const ByteArrayData& second) {
      bool b_second_large(first.i_byte_length_ < second.i_byte_length_);
      const int64_t i_low_size(b_second_large ? first.i_byte_length_ : second.i_byte_length_);
      int i_result(memcmp(first.c_byte_, second.c_byte_, i_low_size));
      if (     i_result < 0)   return true;
      else if (i_result > 0)   return false;
      else if (b_second_large) return true;
      else                     return false;});
  }
};

#endif
  
