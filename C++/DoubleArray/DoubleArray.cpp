#include "DoubleArray.h"
#include <fstream>
#include <limits>
#include <unordered_map>
#include <string.h>

using namespace std;

constexpr char C_TAIL_CHAR         = 0x00;  /* TAILの末尾文字         */
constexpr int I_ARRAY_NO_DATA      =   -1;  /* 配列初期値             */
constexpr int I_DEFAULT_ARRAY_SIZE =  256;  /* defaultの配列サイズ    */
constexpr double D_EXTEND_MEMORY   =  1.5;  /* 配列拡張倍率           */
constexpr int64_t I_SAME_DATA      =   -1;  /* 構築時に同一データ混在 */
constexpr size_t I_TRIE_TAIL_VALUE = numeric_limits<size_t>::max();


/* init only */
DoubleArray::DoubleArray() : i_base_(nullptr), i_check_(nullptr), c_tail_char_(nullptr), i_tail_result_(nullptr),
                             i_array_size_(I_DEFAULT_ARRAY_SIZE),
                             i_tail_char_size_(I_DEFAULT_ARRAY_SIZE),
                             i_tail_result_size_(I_DEFAULT_ARRAY_SIZE)
{
}


/* delete memory */
DoubleArray::~DoubleArray()
{
  deleteMemory();
}


/* メモリ確保                              */
/* @param b_init_size サイズを初期化するか */
/* @return Error Code                      */
int DoubleArray::keepMemory(
  const bool b_init_size)
{
  deleteMemory(b_init_size);  /* 既存のデータ構造を破棄 */

  try {
    i_base_      = new int[i_array_size_];
    i_check_     = new int[i_array_size_];
    c_tail_char_ = new char[i_tail_char_size_];
    if (i_tail_result_size_) {
      i_tail_result_ = new int64_t[i_tail_result_size_];
    }

    memset(i_base_,      0,               sizeof(i_base_[0])      * i_array_size_);
    memset(i_check_,     I_ARRAY_NO_DATA, sizeof(i_check_[0])     * i_array_size_);
    memset(c_tail_char_, 0,               sizeof(c_tail_char_[0]) * i_tail_char_size_);
    if (i_tail_result_size_) {
      memset(i_tail_result_, 0, sizeof(i_tail_result_[0]) * i_tail_result_size_);
    }
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* メモリ破棄                              */
/* @param b_init_size サイズを初期化するか */
void DoubleArray::deleteMemory(
  const bool b_init_size)
{
  if (i_base_)        delete[] i_base_;
  if (i_check_)       delete[] i_check_;
  if (c_tail_char_)   delete[] c_tail_char_;
  if (i_tail_result_) delete[] i_tail_result_;

  i_base_        = nullptr;
  i_check_       = nullptr;
  c_tail_char_   = nullptr;
  i_tail_result_ = nullptr;

  if (b_init_size) {
    i_array_size_       = I_DEFAULT_ARRAY_SIZE;
    i_tail_char_size_   = I_DEFAULT_ARRAY_SIZE;
    i_tail_result_size_ = I_DEFAULT_ARRAY_SIZE;
  }
}


/* データが作成されているかチェック              */
/* @return true : データ作成されてる  false : 空 */
bool DoubleArray::checkInit() const
{
  return i_base_ != nullptr;
}


/* BaseCheckのメモリ拡張                   */
/* @param i_extend_size 拡張する配列サイズ */
/* @return Error Code                      */
ent DoubleArray::baseCheckExtendMemory(
  const int i_extend_size)
{
  try {
    int i_new_array_size(i_extend_size ? i_array_size_ + i_extend_size : static_cast<int>(i_array_size_ * D_EXTEND_MEMORY));

    int* i_new_base  = new int[i_new_array_size];
    int* i_new_check = new int[i_new_array_size];
    memset(i_new_base,  0,               sizeof(i_new_base[0])  * i_new_array_size);
    memset(i_new_check, I_ARRAY_NO_DATA, sizeof(i_new_check[0]) * i_new_array_size);
    memcpy(i_new_base,  i_base_,         sizeof(i_new_base[0])  * i_array_size_);
    memcpy(i_new_check, i_check_,        sizeof(i_new_check[0]) * i_array_size_);

    delete[] i_base_;
    delete[] i_check_;
    i_base_       = i_new_base;
    i_check_      = i_new_check;
    i_array_size_ = i_new_array_size;
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* Tailのメモリ拡張   */
/* @return Error Code */
int DoubleArray::tailExtendMemory()
{
  try {
    int i_new_tail_size(static_cast<int>(i_tail_char_size_ * D_EXTEND_MEMORY));
    char* c_tail_char      = new char[i_new_tail_size];
    int64_t* i_tail_result = new int64_t[i_new_tail_size];

    memset(c_tail_char,   0,              sizeof(c_tail_char[0])   * i_new_tail_size);
    memset(i_tail_result, 0,              sizeof(i_tail_result[0]) * i_new_tail_size);
    memcpy(c_tail_char,   c_tail_char_,   sizeof(c_tail_char[0])   * i_tail_char_size_);
    memcpy(i_tail_result, i_tail_result_, sizeof(i_tail_result[0]) * i_tail_result_size_);

    delete[] c_tail_char_;
    delete[] i_tail_result_;
    c_tail_char_        = c_tail_char;
    i_tail_result_      = i_tail_result;
    i_tail_char_size_   = i_new_tail_size;
    i_tail_result_size_ = i_new_tail_size;
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* Memoryサイズを最適化する                                           */
/* @param i_tail_last_index Tail配列のデータが格納されている最終Index */
/* @return Error Code                                                 */
int DoubleArray::optimizeMemory(
  const int i_tail_last_index)
{
  for (int i = i_array_size_ - 1; i >= 0; --i) {
    if (i_base_[i]) {
      i_array_size_ = i + 1;
      break;
    }
  }

  i_tail_char_size_ = i_tail_last_index;
  i_array_size_ += static_cast<int>(0xff);  /* 検索時に不正領域を参照しない対策 */

  try {
    /* 配列情報 */
    int* i_new_base  = new int[i_array_size_];
    int* i_new_check = new int[i_array_size_];
    memcpy(i_new_base,  i_base_,  sizeof(i_new_base[0])  * i_array_size_);
    memcpy(i_new_check, i_check_, sizeof(i_new_check[0]) * i_array_size_);
    delete[] i_base_;
    delete[] i_check_;
    i_base_  = i_new_base;
    i_check_ = i_new_check;

    /* Tail文字列情報 */
    char* c_new_tail_char = new char[i_tail_char_size_];
    memcpy(c_new_tail_char, c_tail_char_, sizeof(c_new_tail_char[0]) * i_tail_char_size_);
    delete[] c_tail_char_;
    c_tail_char_ = c_new_tail_char;

    if (i_tail_result_size_) {
      i_tail_result_size_ = i_tail_char_size_;  /* 最適化サイズに書き換え */
      int64_t* i_new_tail_result = new int64_t[i_tail_result_size_];
      memcpy(i_new_tail_result, i_tail_result_, sizeof(i_new_tail_result[0]) * i_tail_result_size_);
      delete[] i_tail_result_;
      i_tail_result_ = i_new_tail_result;
    }
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* DoubleArrayを構築する                  */
/* 引数add_datasはmethod内部で破棄する    */
/* @param add_datas DoubleArray構築データ */
/* @param i_option  構築オプション        */
/* @return 0 : 正常終了  0以外 : 異常終了 */
int DoubleArray::createDoubleArray(
  ByteArrayDatas& add_datas,
  const int i_option)
{
  if (add_datas.empty())
    return I_NO_ERROR;

  add_datas.sort(); /* Sort */

  /* Trie構築 */
  TrieArray trie_array;
  if (createTrie(trie_array, add_datas)) {
    return I_FAILED_TRIE;
  }

  for (auto& data : add_datas) {
    data.deleteByteData();
  }
  add_datas.clear();
  ByteArrayDatas(add_datas).swap(add_datas);  /* 元データのメモリ破棄(vectorはclearではメモリ破棄されない) */

  if (keepMemory(true)) { /* メモリ確保 */
    return I_FAILED_MEMORY;
  }

  /* DoubleArray構築 */
  int base_value_array[256] = { 1 };    /* 分岐するNodeのBaseで全ての分岐先に適応する値を探すのに使用 */
  int i_tail_index(1), i_base_index(0); /* BaseとTailの初期値 */
  if (recursiveCreateDoubleArray(i_tail_index, base_value_array, i_base_index, 0, i_option, trie_array)) {
    return I_FAILED_MEMORY;
  }

  /* Trie構造のメモリ破棄 */
  for (auto& trie : trie_array) {
    for (auto& parts : trie) {
      delete parts.trie_parts_;
    }
  }
  if (i_option & I_TAIL_UNITY) {
    delete[] i_tail_result_;
    i_tail_result_      = nullptr;
    i_tail_result_size_ = 0;
  }

  return optimizeMemory(i_tail_index);
}


/* 入力データからTRIE構造を構築する         */
/* @param trie_array       構築したTRIE構造 */
/* @param byte_array_datas 基にするデータ   */
/* @return Error Code                       */
int DoubleArray::createTrie(
  TrieArray& trie_array,
  const ByteArrayDatas& byte_array_datas) const
{
  int i_current(0);
  trie_array.reserve(1000); /* とりあえず、1000個分のメモリ確保 */
  for (const auto& data : byte_array_datas) {
    if (data.i_byte_length_ != 0) {
      int64_t i_tail_index(0);
      searchAddDataPosition(i_tail_index, i_current, byte_array_datas); /* Tail位置を取得 */
      if (i_tail_index != I_SAME_DATA) {
        if (createTrieParts(trie_array, i_tail_index, data))
          return I_FAILED_TRIE;
      }
    }
    ++i_current;
  }

  return I_NO_ERROR;
}


/* TriePartsを作成する                  */
/* @param trie_array   構築したTrie構造 */
/* @param i_tail_index Tailの対象Index  */
/* @param byte_data    ByteArrayData    */
/* @return Error Code                   */
int DoubleArray::createTrieParts(
  TrieArray& trie_array,
  const int64_t i_tail_index,
  const ByteArrayData& byte_data) const
{
  try {
    size_t i_trie_index(0);
    bool b_tail(false);
    for (int64_t i = 0; i < static_cast<int64_t>(byte_data.i_byte_length_); ++i) {
      if (i_tail_index == 0 || i_tail_index == i) b_tail = true;

      if (trie_array.size() <= i_trie_index) {
        trie_array.push_back(move(vector<TrieLayerData>()));
      }
      auto& trie_layer = trie_array[i_trie_index];

      unsigned char c_one_word = byte_data.c_byte_[i];
      auto trie = lower_bound(trie_layer.begin(), trie_layer.end(), c_one_word,
                              [](const TrieLayerData& trie, const unsigned char c_byte) {return trie.c_byte_ < c_byte;});
      if (trie == trie_layer.end()
      ||  trie->c_byte_ != c_one_word) {
        char* c_tail = 0;
        int64_t i_tail_size(0);
        if (b_tail && i < static_cast<int64_t>(byte_data.i_byte_length_) - 1) {
          i_tail_size = byte_data.i_byte_length_ - i - 1;
          c_tail = new char[i_tail_size];
          memcpy(c_tail, (byte_data.c_byte_ + i + 1), i_tail_size);
        }

        size_t i_next_index(trie_array.size());
        TrieParts* trie_parts = (b_tail ? new TrieParts(c_tail, i_tail_size, byte_data.result_) : nullptr);
        trie_layer.insert(trie, move(TrieLayerData(c_one_word, (b_tail ? I_TRIE_TAIL_VALUE : i_next_index), trie_parts)));
        i_trie_index = i_next_index;
        if (b_tail) {
          break;
        }
      } else {
        i_trie_index = trie->i_next_trie_index_;
        if (i_trie_index == I_TRIE_TAIL_VALUE) {
          i_trie_index = trie_array.size();
          trie->i_next_trie_index_ = i_trie_index;
        }
      }
    }
  } catch (...) {
    return I_FAILED_TRIE;
  }

  return I_NO_ERROR;
}



/* @param i_tail_index     書き込み開始TailIndex           */
/* @param base_value_array BaseValueの値を決定するのに使用 */
/* @param i_base_index     基準のBaseCheckIndex            */
/* @param i_trie_vec_index 対象のTrieノード群              */
/* @param i_option         構築オプション                  */
/* @param trie_array       TRIE全データ                    */
/* @return Error Code                                      */
int DoubleArray::recursiveCreateDoubleArray(
  int& i_tail_index,
  int* base_value_array,
  const int i_base_index,
  const size_t i_trie_vec_index,
  const int i_option,
  const TrieArray& trie_array)
{
  int i_base_value;
  const auto& trie_layers = trie_array[i_trie_vec_index];
  if (getBaseValue(i_base_value, base_value_array, i_option, trie_layers)) {
    return I_FAILED_MEMORY; /* 構築失敗 */
  }
  i_base_[i_base_index] = i_base_value;

  /* 再帰処理する前にcheckを設定 */
  for (const auto& trie : trie_layers) {
    int i_insert_index(trie.c_byte_ + i_base_value);
    i_check_[i_insert_index] = i_base_index;
  }

  for (const auto& trie : trie_layers) {
    int i_insert_index(trie.c_byte_ + i_base_value);
    if (trie.trie_parts_) {
      i_base_[i_insert_index] = i_tail_index * (-1);  /* TailIndexはマイナス値 */

      if (setTailInfo(i_tail_index, trie.trie_parts_)) {
        return I_FAILED_MEMORY; /* 構築失敗 */
      }
    }
    if (trie.i_next_trie_index_ != I_TRIE_TAIL_VALUE) {
      if (recursiveCreateDoubleArray(i_tail_index, base_value_array, i_insert_index, trie.i_next_trie_index_, i_option, trie_array)) {
        return I_FAILED_MEMORY; /* 構築失敗 */
      }
    }
  }

  return I_NO_ERROR;
}


/* Baseの値を求める                                        */
/* @param i_base_value     求めたBase値                    */
/* @param base_value_array BaseValueの値を決定するのに使用 */
/* @param i_option         構築オプション                  */
/* @param tries            TRIEでの同列情報                */
/* @return Error Code                                      */
int DoubleArray::getBaseValue(
  int& i_base_value,
  int* base_value_array,
  const int i_option,
  const vector<TrieLayerData>& tries)
{
  /* 既存のバイト情報の最大Base値を検索 */
  i_base_value = 0;
  if (i_option & I_SPEED_PRIORITY) {
    for (const auto& trie : tries) {
      if (base_value_array[trie.c_byte_] > i_base_value) {
        i_base_value = base_value_array[trie.c_byte_];
      }
    }
  }

  bool b_success(false);
  while (b_success == false) {
    for (++i_base_value; i_base_value < i_array_size_; ++i_base_value) {
      int i_index(tries.rbegin()->c_byte_ + i_base_value);
      while (i_array_size_ <= i_index) {
        if (baseCheckExtendMemory()) {  /* メモリが不足 拡張 */
          return I_FAILED_MEMORY;
        }
      }

      bool b_find(true);
      for (const auto& trie : tries) {
        int i_check_index(trie.c_byte_ + i_base_value);
        if (i_check_[i_check_index] != I_ARRAY_NO_DATA) {
          b_find = false;
          break;
        }
      }
      if (b_find) {
        b_success = true;
        break;
      }
    }
    if (b_success == false) {
      if (baseCheckExtendMemory()) {  /* メモリが不足 拡張 */
        return I_FAILED_MEMORY;
      }
    }
  }

  if (i_option & I_SPEED_PRIORITY) {
    for (const auto& trie : tries) {
      base_value_array[trie.c_byte_] = i_base_value;
    }
  }

  return I_NO_ERROR;
}


/* Tailに情報を設定する                 */
/* @param i_tail_index Tail格納開始位置 */
/* @param trie_parts   TrieParts        */
/* @return Error code                   */
int DoubleArray::setTailInfo(
  int& i_tail_index,
  const TrieParts* trie_parts)
{
  while (i_tail_index + trie_parts->i_tail_size_ >= i_tail_char_size_) {
    if (tailExtendMemory()) { /* メモリが不足したので拡張 */
      return I_FAILED_MEMORY;
    }
  }

  if (trie_parts->c_tail_) {
    for (int i = 0; i < trie_parts->i_tail_size_; ++i) {
      c_tail_char_[i_tail_index++] = trie_parts->c_tail_[i];
    }
    --i_tail_index;
  } else {
    c_tail_char_[i_tail_index] = C_TAIL_CHAR;
  }
  i_tail_result_[i_tail_index++] = trie_parts->i_result_;

  return I_NO_ERROR;
}


/* 既存のデータとの重複部分をチェック            */
/* @param i_tail_index Tailに格納する文字列Index */
/* @param i_current    処理対象のindex           */
/* @param datas        全追加データ              */
void DoubleArray::searchAddDataPosition(
  int64_t& i_tail_index,
  const int i_current,
  const ByteArrayDatas& datas) const
{
  auto current = datas.cbegin() + i_current;
  if (datas.size() == 1) {
    i_tail_index = current->i_byte_length_;
    return;
  }

  int64_t i_before_same_index(0), i_after_same_index(0);
  const char* c_current_byte = current->c_byte_;

  if (current != datas.cbegin()) {
    auto before = current;
    --before;
    const char* c_before_byte = before->c_byte_;
    const int64_t i_length(current->i_byte_length_);
    for (auto i = 0; i < i_length; ++i) {
      if (c_current_byte[i] != c_before_byte[i]) {
        i_before_same_index = i;
        break;
      }
    }
  }

  auto after = current;
  ++after;
  if (after != datas.cend()) {
    const char* c_after_byte = after->c_byte_;
    i_after_same_index = current->i_byte_length_;
    for (auto i = 0; i < i_after_same_index; ++i) {
      if (c_current_byte[i] != c_after_byte[i]) {
        i_after_same_index = i;
        break;
      }
    }
    if (i_after_same_index == static_cast<int64_t>(after->i_byte_length_)) {
      i_tail_index = I_SAME_DATA;
      return; /* 同一データ混在 */
    }
  }

  i_tail_index = (i_before_same_index < i_after_same_index ? i_after_same_index : i_before_same_index);
}


/* 検索する                                       */
/* c_byteにはNULLが途中に含まれる可能性がある為、 */
/* バイト長も渡さないとダメ。                     */
/* 引数のバイト列末尾のNULLも使用する             */
/* @param c_byte        search bytes              */
/* @param i_byte_length search data length        */
/* @return search result                          */
int64_t DoubleArray::search(
  const char* c_byte,
  uint64_t i_byte_length) const noexcept
{
  int i_base_index(0);
  for (uint64_t i = 0; i <= i_byte_length; ++i) { /* 終端記号の分があるので<=とする */
    int i_check_index(i_base_[i_base_index] + static_cast<unsigned char>(c_byte[i]));
    if (i_check_[i_check_index] != i_base_index) {
      return I_SEARCH_NOHIT;  /* データが存在しない */
    }

    if (i_base_[i_check_index] >= 0) {
      i_base_index = i_check_index;
      continue;
    }

    /* Tail処理 */
    uint64_t i_tail_index(-i_base_[i_check_index]); /* Tail突入契機のマイナス値をプラスに変換 */
    if (i < i_byte_length) {
      ++i;
      uint64_t i_compare_length(i_byte_length - i);
      if (memcmp(&c_tail_char_[i_tail_index], &c_byte[i], i_compare_length + 1) == 0) {
        if (i_tail_result_ == nullptr) return I_HIT_DEFAULT;
        else                           return i_tail_result_[i_tail_index + i_compare_length];
      }
    } else if (i == i_byte_length) {
      if (i_tail_result_ == nullptr) return I_HIT_DEFAULT;
        else                         return i_tail_result_[i_tail_index];
    }

    return I_SEARCH_NOHIT;  /* Tailに来てヒットしなかったので無ヒット */
  }

  return I_SEARCH_NOHIT;
}


/* 途中経過状態を取得しながら検索する                        */
/* @param sarch_parts   search position                      */
/* @param result        search result                        */
/* @param c_byte        search bytes                         */
/* @param i_byte_length search data length                   */
/* @return true : 続きがある文字列  false : 続きが無い文字列 */
bool DoubleArray::searchContinue(
  DASearchParts& search_parts,
  int64_t& result,
  const char* c_byte,
  const uint64_t i_byte_length) const noexcept
{
  result = I_SEARCH_NOHIT;
  if (i_base_ == nullptr) {
    return false; /* データゼロ */
  }

  unsigned int i(0);
  bool b_continue(false);
  for (; i < i_byte_length; ++i) {
    if (search_parts.i_tail_ == 0) {  /* 前回Tailまで処理が進んでいたら、スキップ */
      search_parts.i_check_ = i_base_[search_parts.i_base_] + static_cast<unsigned char>(c_byte[i]);
      if (i_check_[search_parts.i_check_] != search_parts.i_base_) {
        return false; /* データが存在しない */
      }
    }

    if (search_parts.i_tail_ == 0
    &&  i_base_[search_parts.i_check_] >= 0) {
      search_parts.i_base_ = search_parts.i_check_;
      continue;
    }

    /* Tail処理 */
    uint64_t i_byte_index(i);
    if (search_parts.i_tail_ == 0) {  /* 初のTail突入時処理 */
      ++i_byte_index;
      search_parts.i_tail_ = -i_base_[search_parts.i_check_]; /* マイナス値をプラスに変換 */
    }

    uint64_t i_compare_size(i_byte_length - i_byte_index);
    if (memcmp(&c_tail_char_[search_parts.i_tail_], &c_byte[i_byte_index], i_compare_size) != 0) {
      break;  /* データが存在しない　Tailの途中で不一致 */
    }
    i_byte_index         += i_compare_size;
    search_parts.i_tail_ += static_cast<int>(i_compare_size);

    if (i_byte_index >= i_byte_length) {
      if (c_tail_char_[search_parts.i_tail_] == C_TAIL_CHAR) {
        if (i_tail_result_) result = i_tail_result_[search_parts.i_tail_];  /* ヒット */
        else                result = I_HIT_DEFAULT;
      } else {
        b_continue = true;
      }
    }
    break;  /* Tailに来たら、ヒット無ヒット関係なく終了 */
  }

  if (i >= i_byte_length) {
    if (i_check_[i_base_[search_parts.i_base_]] == search_parts.i_base_) {
      const int i_tail_index(i_base_[i_base_[search_parts.i_base_]]);
      if (i_tail_index < 0) {
        if (i_tail_result_) result = i_tail_result_[-i_tail_index]; /* ヒット */
        else                result = I_HIT_DEFAULT;
      }
      b_continue = true;
    }
  }

  return b_continue;
}


/* DoubleArray情報を書き込む                         */
/* @param i_write_size 書き込んだデータサイズ        */
/* @param fp           OutputFileStream              */
/* @return I_DA_NO_ERROR : 正常終了 0以外 : 異常終了 */
int DoubleArray::writeBinary(
  int64_t& i_write_size,
  FILE* fp) const
{
  try {
    if (checkInit()) {
      fwrite(&i_array_size_,       sizeof(i_array_size_),       1,                 fp);
      fwrite(&i_tail_char_size_,   sizeof(i_tail_char_size_),   1,                 fp);
      fwrite(&i_tail_result_size_, sizeof(i_tail_result_size_), 1,                 fp);
      fwrite(i_base_,              sizeof(i_base_[0]),          i_array_size_,     fp);
      fwrite(i_check_,             sizeof(i_check_[0]),         i_array_size_,     fp);
      fwrite(c_tail_char_,         sizeof(c_tail_char_[0]),     i_tail_char_size_, fp);
      if (i_tail_result_size_) {
        fwrite(i_tail_result_, sizeof(i_tail_result_[0]), i_tail_result_size_, fp);
      }

      i_write_size += sizeof(i_array_size_);
      i_write_size += sizeof(i_tail_char_size_);
      i_write_size += sizeof(i_tail_result_size_);
      i_write_size += sizeof(i_base_[0])      * i_array_size_;
      i_write_size += sizeof(i_check_[0])     * i_array_size_;
      i_write_size += sizeof(c_tail_char_[0]) * i_tail_char_size_;
      if (i_tail_result_size_) {
        i_write_size += sizeof(i_tail_result_[0]) * i_tail_result_size_;
      }
    } else {
      int i_zero(0);
      fwrite(&i_zero, sizeof(i_zero), 1, fp);
      i_write_size += sizeof(i_zero);
    }
  }
  catch (...) {
    return I_FAIELD_FILE_IO;
  }

  return I_NO_ERROR;
}


/* DoubleArray情報を読み込む                 */
/* @param i_read_size 読み込んだデータサイズ */
/* @param fp InputFileStream                 */
/* @return 0 : 正常終了 0以外 : 異常終了     */
int DoubleArray::readBinary(
  int64_t& i_read_size,
  FILE* fp)
{
  try {
    if (1 != fread(&i_array_size_, sizeof(i_array_size_), 1, fp)) /* 配列サイズ */
      throw;

    if (i_array_size_) {
      if ((1 != fread(&i_tail_char_size_,   sizeof(i_tail_char_size_),   1, fp))  /* Tail文字列サイズ */
      ||  (1 != fread(&i_tail_result_size_, sizeof(i_tail_result_size_), 1, fp)) /* Tail結果サイズ   */
      ||  (keepMemory())) /* 配列サイズが確定したのでメモリ確保 */
        throw;

      if ((static_cast<size_t>(i_array_size_)     != fread(i_base_,      sizeof(i_base_[0]),      i_array_size_,     fp))   /* Base     */
      ||  (static_cast<size_t>(i_array_size_)     != fread(i_check_,     sizeof(i_check_[0]),     i_array_size_,     fp))   /* Check    */
      ||  (static_cast<size_t>(i_tail_char_size_) != fread(c_tail_char_, sizeof(c_tail_char_[0]), i_tail_char_size_, fp)))  /* Tail文字 */
        throw;

      if (i_tail_result_size_) {
        if (static_cast<size_t>(i_tail_result_size_) != fread(i_tail_result_, sizeof(i_tail_result_[0]), i_tail_result_size_, fp)) /* Tail結果 */
          throw;
      } else {
        i_tail_result_ = nullptr;
      }

      i_read_size += sizeof(i_tail_char_size_);
      i_read_size += sizeof(i_tail_result_size_);
      i_read_size += sizeof(i_base_[0])  * i_array_size_;
      i_read_size += sizeof(i_check_[0]) * i_array_size_;
      if (i_tail_result_size_) {
        i_read_size += sizeof(i_tail_result_[0]) * i_tail_result_size_;
      }
    } else {
      i_read_size += sizeof(i_array_size_);
    }
  } catch (...) {
    deleteMemory();
    return I_FAIELD_FILE_IO;
  }

  return I_NO_ERROR;
}


/* 内部データを取得する                       */
/* @param i_array_size       配列サイズ       */
/* @param i_tail_char_size   Tail文字列サイズ */
/* @param i_tail_result_size Tail結果サイズ   */
/* @param i_base             Base配列         */
/* @param i_check            Check配列        */
/* @param i_tail_result      Tail結果配列     */
/* @param c_tail_char        Tail文字配列     */
void DoubleArray::getDoubleArrayData(
  int& i_array_size,
  int& i_tail_char_size,
  int& i_tail_result_size,
  const int*& i_base,
  const int*& i_check,
  const int64_t*& i_tail_result,
  const char*& c_tail_char) const
{
  i_array_size       = i_array_size_;
  i_tail_char_size   = i_tail_char_size_;
  i_tail_result_size = i_tail_result_size_;
  i_base             = i_base_;
  i_check            = i_check_;
  i_tail_result      = i_tail_result_;
  c_tail_char        = c_tail_char_;
}


/* 内部データを外部から設定する               */
/* @param i_array_size       配列サイズ       */
/* @param i_tail_char_size   Tail文字列サイズ */
/* @param i_tail_result_size Tail結果サイズ   */
/* @param i_base             Base配列         */
/* @param i_check            Check配列        */
/* @param i_tail_result      Tail結果配列     */
/* @param c_tail_char        Tail文字配列     */
int DoubleArray::setDoubleArrayData(
  int i_array_size,
  int i_tail_char_size,
  int i_tail_result_size,
  const int* i_base,
  const int* i_check,
  const int64_t* i_tail_result,
  const char* c_tail_char)
{
  i_array_size_       = i_array_size;       /* 配列サイズ       */
  i_tail_char_size_   = i_tail_char_size;   /* Tail文字列サイズ */
  i_tail_result_size_ = i_tail_result_size; /* Tail結果サイズ   */

  if (keepMemory()) /* 配列サイズが確定したのでメモリ確保 */
    return I_FAILED_MEMORY;

  try {
    memcpy(i_base_,      i_base,      sizeof(int)  * i_array_size_);
    memcpy(i_check_,     i_check,     sizeof(int)  * i_array_size_);
    memcpy(c_tail_char_, c_tail_char, sizeof(char) * i_tail_char_size_);
    if (i_tail_result) {
      memcpy(i_tail_result_, i_tail_result, sizeof(int) * i_tail_result_size_);
    } else {
      i_tail_result_ = nullptr;
    }
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* DoubleArray形式からデータを再生 */
/* @param origins 再生したデータ   */
void DoubleArray::reproductionData(
  vector<pair<char*, int64_t> >& origins) const
{
  if (!checkInit()                /* 初期化されていない           */
  || i_tail_result_ == nullptr) { /* 結果情報が統一されているもの */
    return;
  }

  vector<int> tail_results; /* 結果が格納されているIndex */
  for (int i = 0; i < i_tail_result_size_; ++i) {
    if (i_tail_result_[i]) {
      tail_results.push_back(i);
    }
  }

  unordered_map<int, int> tail_indexes; /* Tailに突入するBaseIndex first:TailIndex(正値) second:BaseIndex */
  for (int i = 0; i < i_array_size_; ++i) {
    if (i_base_[i] < 0) {
      tail_indexes.insert(make_pair(i_base_[i] * -1, i)); /* 負値を正値に変換 */
    }
  }

  int i_tail_end(0);
  vector<char> datas;
  for (const auto& tail : tail_results) {
    datas.clear();
    int i;
    for (i = tail - 1; i > i_tail_end; --i) { /* 終端記号を除外するので-1 */
      datas.push_back(c_tail_char_[i]); /* Tail部分のデータ */
    }

    int i_array_index(tail_indexes.find(i + 1)->second);
    while (i_array_index != 0) {
      int i_char = (i_array_index - i_base_[i_check_[i_array_index]]) & 0xff;
      datas.push_back((char)i_char);
      i_array_index = i_check_[i_array_index];
    }

    int i_index(0);
    char* c_data = new char[datas.size() + 1];
    for (const auto& data : datas) {
      c_data[i_index++] = data;
    }
    c_data[i_index] = 0x00; /* 終端記号 */

    origins.push_back(make_pair(c_data, i_tail_result_[tail]));
    i_tail_end = tail;
  }
}


/* 結果IndexからByte情報を復元する                               */
/* @param c_info         復元したByte情報 呼び出し側でdeleteする */
/* @param i_reuslt_index 復元する結果Index                       */
void DoubleArray::reproductionFromIndex(
  char*& c_info,
  const int64_t i_result_index) const
{
  c_info = nullptr;
  int i_tail_last_index(0);
  for (auto i = 0; i < i_tail_result_size_; ++i) {
    if (i_tail_result_[i] == i_result_index) {
      i_tail_last_index = i;
    }
  }

  if (i_tail_last_index == 0) {
    return; /* 指定のIndexは無し */
  }

  vector<char> datas;
  int i_tail_index(--i_tail_last_index);  /* 終端記号を指しているので一つ前にする */
  while (c_tail_char_[i_tail_index]) {
    datas.push_back(c_tail_char_[i_tail_index--]);  /* Tail部分のデータ */
  }
  ++i_tail_index; /* Tail先頭の位置にするのでIncrement */

  int i_array_index(0); /* base位置特定 */
  for (int i = 0; i < i_array_size_; ++i) {
    if ((i_base_[i] < 0)
      && ((-i_base_[i]) == i_tail_index)) {
      i_array_index = i;
      break;
    }
  }

  while (i_array_index != 0) {
    int i_char = (i_array_index - i_base_[i_check_[i_array_index]]) & 0xff;
    datas.push_back((char)i_char);
    i_array_index = i_check_[i_array_index];
  }

  int i_index(0);
  c_info = new char[datas.size() + 1];
  for (auto data = datas.crbegin(), data_end = datas.crend(); data != data_end; ++data) {
    c_info[i_index++] = *data;
  }
  c_info[i_index] = 0x00; /* 終端記号 */
}
