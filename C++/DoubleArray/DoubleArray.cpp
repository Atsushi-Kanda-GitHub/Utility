#include "DoubleArray.h"
#include <fstream>
#include <limits>
#include <unordered_map>
#include <string.h>

using namespace std;

constexpr char C_TAIL_CHAR         = 0x00;  /* TAIL�̖�������         */
constexpr int I_ARRAY_NO_DATA      =   -1;  /* �z�񏉊��l             */
constexpr int I_DEFAULT_ARRAY_SIZE =  256;  /* default�̔z��T�C�Y    */
constexpr double D_EXTEND_MEMORY   =  1.5;  /* �z��g���{��           */
constexpr int64_t I_SAME_DATA      =   -1;  /* �\�z���ɓ���f�[�^���� */
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


/* �������m��                              */
/* @param b_init_size �T�C�Y�����������邩 */
/* @return Error Code                      */
int DoubleArray::keepMemory(
  const bool b_init_size)
{
  deleteMemory(b_init_size);  /* �����̃f�[�^�\����j�� */

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


/* �������j��                              */
/* @param b_init_size �T�C�Y�����������邩 */
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


/* �f�[�^���쐬����Ă��邩�`�F�b�N              */
/* @return true : �f�[�^�쐬����Ă�  false : �� */
bool DoubleArray::checkInit() const
{
  return i_base_ != nullptr;
}


/* BaseCheck�̃������g��                   */
/* @param i_extend_size �g������z��T�C�Y */
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


/* Tail�̃������g��   */
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


/* Memory�T�C�Y���œK������                                           */
/* @param i_tail_last_index Tail�z��̃f�[�^���i�[����Ă���ŏIIndex */
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
  i_array_size_ += static_cast<int>(0xff);  /* �������ɕs���̈���Q�Ƃ��Ȃ��΍� */

  try {
    /* �z���� */
    int* i_new_base  = new int[i_array_size_];
    int* i_new_check = new int[i_array_size_];
    memcpy(i_new_base,  i_base_,  sizeof(i_new_base[0])  * i_array_size_);
    memcpy(i_new_check, i_check_, sizeof(i_new_check[0]) * i_array_size_);
    delete[] i_base_;
    delete[] i_check_;
    i_base_  = i_new_base;
    i_check_ = i_new_check;

    /* Tail�������� */
    char* c_new_tail_char = new char[i_tail_char_size_];
    memcpy(c_new_tail_char, c_tail_char_, sizeof(c_new_tail_char[0]) * i_tail_char_size_);
    delete[] c_tail_char_;
    c_tail_char_ = c_new_tail_char;

    if (i_tail_result_size_) {
      i_tail_result_size_ = i_tail_char_size_;  /* �œK���T�C�Y�ɏ������� */
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


/* DoubleArray���\�z����                  */
/* ����add_datas��method�����Ŕj������    */
/* @param add_datas DoubleArray�\�z�f�[�^ */
/* @param i_option  �\�z�I�v�V����        */
/* @return 0 : ����I��  0�ȊO : �ُ�I�� */
int DoubleArray::createDoubleArray(
  ByteArrayDatas& add_datas,
  const int i_option)
{
  if (add_datas.empty())
    return I_NO_ERROR;

  add_datas.sort(); /* Sort */

  /* Trie�\�z */
  TrieArray trie_array;
  if (createTrie(trie_array, add_datas)) {
    return I_FAILED_TRIE;
  }

  for (auto& data : add_datas) {
    data.deleteByteData();
  }
  add_datas.clear();
  ByteArrayDatas(add_datas).swap(add_datas);  /* ���f�[�^�̃������j��(vector��clear�ł̓������j������Ȃ�) */

  if (keepMemory(true)) { /* �������m�� */
    return I_FAILED_MEMORY;
  }

  /* DoubleArray�\�z */
  int base_value_array[256] = { 1 };    /* ���򂷂�Node��Base�őS�Ă̕����ɓK������l��T���̂Ɏg�p */
  int i_tail_index(1), i_base_index(0); /* Base��Tail�̏����l */
  if (recursiveCreateDoubleArray(i_tail_index, base_value_array, i_base_index, 0, i_option, trie_array)) {
    return I_FAILED_MEMORY;
  }

  /* Trie�\���̃������j�� */
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


/* ���̓f�[�^����TRIE�\�����\�z����         */
/* @param trie_array       �\�z����TRIE�\�� */
/* @param byte_array_datas ��ɂ���f�[�^   */
/* @return Error Code                       */
int DoubleArray::createTrie(
  TrieArray& trie_array,
  const ByteArrayDatas& byte_array_datas) const
{
  int i_current(0);
  trie_array.reserve(1000); /* �Ƃ肠�����A1000���̃������m�� */
  for (const auto& data : byte_array_datas) {
    if (data.i_byte_length_ != 0) {
      int64_t i_tail_index(0);
      searchAddDataPosition(i_tail_index, i_current, byte_array_datas); /* Tail�ʒu���擾 */
      if (i_tail_index != I_SAME_DATA) {
        if (createTrieParts(trie_array, i_tail_index, data))
          return I_FAILED_TRIE;
      }
    }
    ++i_current;
  }

  return I_NO_ERROR;
}


/* TrieParts���쐬����                  */
/* @param trie_array   �\�z����Trie�\�� */
/* @param i_tail_index Tail�̑Ώ�Index  */
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



/* @param i_tail_index     �������݊J�nTailIndex           */
/* @param base_value_array BaseValue�̒l�����肷��̂Ɏg�p */
/* @param i_base_index     ���BaseCheckIndex            */
/* @param i_trie_vec_index �Ώۂ�Trie�m�[�h�Q              */
/* @param i_option         �\�z�I�v�V����                  */
/* @param trie_array       TRIE�S�f�[�^                    */
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
    return I_FAILED_MEMORY; /* �\�z���s */
  }
  i_base_[i_base_index] = i_base_value;

  /* �ċA��������O��check��ݒ� */
  for (const auto& trie : trie_layers) {
    int i_insert_index(trie.c_byte_ + i_base_value);
    i_check_[i_insert_index] = i_base_index;
  }

  for (const auto& trie : trie_layers) {
    int i_insert_index(trie.c_byte_ + i_base_value);
    if (trie.trie_parts_) {
      i_base_[i_insert_index] = i_tail_index * (-1);  /* TailIndex�̓}�C�i�X�l */

      if (setTailInfo(i_tail_index, trie.trie_parts_)) {
        return I_FAILED_MEMORY; /* �\�z���s */
      }
    }
    if (trie.i_next_trie_index_ != I_TRIE_TAIL_VALUE) {
      if (recursiveCreateDoubleArray(i_tail_index, base_value_array, i_insert_index, trie.i_next_trie_index_, i_option, trie_array)) {
        return I_FAILED_MEMORY; /* �\�z���s */
      }
    }
  }

  return I_NO_ERROR;
}


/* Base�̒l�����߂�                                        */
/* @param i_base_value     ���߂�Base�l                    */
/* @param base_value_array BaseValue�̒l�����肷��̂Ɏg�p */
/* @param i_option         �\�z�I�v�V����                  */
/* @param tries            TRIE�ł̓�����                */
/* @return Error Code                                      */
int DoubleArray::getBaseValue(
  int& i_base_value,
  int* base_value_array,
  const int i_option,
  const vector<TrieLayerData>& tries)
{
  /* �����̃o�C�g���̍ő�Base�l������ */
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
        if (baseCheckExtendMemory()) {  /* ���������s�� �g�� */
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
      if (baseCheckExtendMemory()) {  /* ���������s�� �g�� */
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


/* Tail�ɏ���ݒ肷��                 */
/* @param i_tail_index Tail�i�[�J�n�ʒu */
/* @param trie_parts   TrieParts        */
/* @return Error code                   */
int DoubleArray::setTailInfo(
  int& i_tail_index,
  const TrieParts* trie_parts)
{
  while (i_tail_index + trie_parts->i_tail_size_ >= i_tail_char_size_) {
    if (tailExtendMemory()) { /* ���������s�������̂Ŋg�� */
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


/* �����̃f�[�^�Ƃ̏d���������`�F�b�N            */
/* @param i_tail_index Tail�Ɋi�[���镶����Index */
/* @param i_current    �����Ώۂ�index           */
/* @param datas        �S�ǉ��f�[�^              */
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
      return; /* ����f�[�^���� */
    }
  }

  i_tail_index = (i_before_same_index < i_after_same_index ? i_after_same_index : i_before_same_index);
}


/* ��������                                       */
/* c_byte�ɂ�NULL���r���Ɋ܂܂��\��������ׁA */
/* �o�C�g�����n���Ȃ��ƃ_���B                     */
/* �����̃o�C�g�񖖔���NULL���g�p����             */
/* @param c_byte        search bytes              */
/* @param i_byte_length search data length        */
/* @return search result                          */
int64_t DoubleArray::search(
  const char* c_byte,
  uint64_t i_byte_length) const noexcept
{
  int i_base_index(0);
  for (uint64_t i = 0; i <= i_byte_length; ++i) { /* �I�[�L���̕�������̂�<=�Ƃ��� */
    int i_check_index(i_base_[i_base_index] + static_cast<unsigned char>(c_byte[i]));
    if (i_check_[i_check_index] != i_base_index) {
      return I_SEARCH_NOHIT;  /* �f�[�^�����݂��Ȃ� */
    }

    if (i_base_[i_check_index] >= 0) {
      i_base_index = i_check_index;
      continue;
    }

    /* Tail���� */
    uint64_t i_tail_index(-i_base_[i_check_index]); /* Tail�˓��_�@�̃}�C�i�X�l���v���X�ɕϊ� */
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

    return I_SEARCH_NOHIT;  /* Tail�ɗ��ăq�b�g���Ȃ������̂Ŗ��q�b�g */
  }

  return I_SEARCH_NOHIT;
}


/* �r���o�ߏ�Ԃ��擾���Ȃ��猟������                        */
/* @param sarch_parts   search position                      */
/* @param result        search result                        */
/* @param c_byte        search bytes                         */
/* @param i_byte_length search data length                   */
/* @return true : ���������镶����  false : ���������������� */
bool DoubleArray::searchContinue(
  DASearchParts& search_parts,
  int64_t& result,
  const char* c_byte,
  const uint64_t i_byte_length) const noexcept
{
  result = I_SEARCH_NOHIT;
  if (i_base_ == nullptr) {
    return false; /* �f�[�^�[�� */
  }

  unsigned int i(0);
  bool b_continue(false);
  for (; i < i_byte_length; ++i) {
    if (search_parts.i_tail_ == 0) {  /* �O��Tail�܂ŏ������i��ł�����A�X�L�b�v */
      search_parts.i_check_ = i_base_[search_parts.i_base_] + static_cast<unsigned char>(c_byte[i]);
      if (i_check_[search_parts.i_check_] != search_parts.i_base_) {
        return false; /* �f�[�^�����݂��Ȃ� */
      }
    }

    if (search_parts.i_tail_ == 0
    &&  i_base_[search_parts.i_check_] >= 0) {
      search_parts.i_base_ = search_parts.i_check_;
      continue;
    }

    /* Tail���� */
    uint64_t i_byte_index(i);
    if (search_parts.i_tail_ == 0) {  /* ����Tail�˓������� */
      ++i_byte_index;
      search_parts.i_tail_ = -i_base_[search_parts.i_check_]; /* �}�C�i�X�l���v���X�ɕϊ� */
    }

    uint64_t i_compare_size(i_byte_length - i_byte_index);
    if (memcmp(&c_tail_char_[search_parts.i_tail_], &c_byte[i_byte_index], i_compare_size) != 0) {
      break;  /* �f�[�^�����݂��Ȃ��@Tail�̓r���ŕs��v */
    }
    i_byte_index         += i_compare_size;
    search_parts.i_tail_ += static_cast<int>(i_compare_size);

    if (i_byte_index >= i_byte_length) {
      if (c_tail_char_[search_parts.i_tail_] == C_TAIL_CHAR) {
        if (i_tail_result_) result = i_tail_result_[search_parts.i_tail_];  /* �q�b�g */
        else                result = I_HIT_DEFAULT;
      } else {
        b_continue = true;
      }
    }
    break;  /* Tail�ɗ�����A�q�b�g���q�b�g�֌W�Ȃ��I�� */
  }

  if (i >= i_byte_length) {
    if (i_check_[i_base_[search_parts.i_base_]] == search_parts.i_base_) {
      const int i_tail_index(i_base_[i_base_[search_parts.i_base_]]);
      if (i_tail_index < 0) {
        if (i_tail_result_) result = i_tail_result_[-i_tail_index]; /* �q�b�g */
        else                result = I_HIT_DEFAULT;
      }
      b_continue = true;
    }
  }

  return b_continue;
}


/* DoubleArray������������                         */
/* @param i_write_size �������񂾃f�[�^�T�C�Y        */
/* @param fp           OutputFileStream              */
/* @return I_DA_NO_ERROR : ����I�� 0�ȊO : �ُ�I�� */
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


/* DoubleArray����ǂݍ���                 */
/* @param i_read_size �ǂݍ��񂾃f�[�^�T�C�Y */
/* @param fp InputFileStream                 */
/* @return 0 : ����I�� 0�ȊO : �ُ�I��     */
int DoubleArray::readBinary(
  int64_t& i_read_size,
  FILE* fp)
{
  try {
    if (1 != fread(&i_array_size_, sizeof(i_array_size_), 1, fp)) /* �z��T�C�Y */
      throw;

    if (i_array_size_) {
      if ((1 != fread(&i_tail_char_size_,   sizeof(i_tail_char_size_),   1, fp))  /* Tail������T�C�Y */
      ||  (1 != fread(&i_tail_result_size_, sizeof(i_tail_result_size_), 1, fp)) /* Tail���ʃT�C�Y   */
      ||  (keepMemory())) /* �z��T�C�Y���m�肵���̂Ń������m�� */
        throw;

      if ((static_cast<size_t>(i_array_size_)     != fread(i_base_,      sizeof(i_base_[0]),      i_array_size_,     fp))   /* Base     */
      ||  (static_cast<size_t>(i_array_size_)     != fread(i_check_,     sizeof(i_check_[0]),     i_array_size_,     fp))   /* Check    */
      ||  (static_cast<size_t>(i_tail_char_size_) != fread(c_tail_char_, sizeof(c_tail_char_[0]), i_tail_char_size_, fp)))  /* Tail���� */
        throw;

      if (i_tail_result_size_) {
        if (static_cast<size_t>(i_tail_result_size_) != fread(i_tail_result_, sizeof(i_tail_result_[0]), i_tail_result_size_, fp)) /* Tail���� */
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


/* �����f�[�^���擾����                       */
/* @param i_array_size       �z��T�C�Y       */
/* @param i_tail_char_size   Tail������T�C�Y */
/* @param i_tail_result_size Tail���ʃT�C�Y   */
/* @param i_base             Base�z��         */
/* @param i_check            Check�z��        */
/* @param i_tail_result      Tail���ʔz��     */
/* @param c_tail_char        Tail�����z��     */
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


/* �����f�[�^���O������ݒ肷��               */
/* @param i_array_size       �z��T�C�Y       */
/* @param i_tail_char_size   Tail������T�C�Y */
/* @param i_tail_result_size Tail���ʃT�C�Y   */
/* @param i_base             Base�z��         */
/* @param i_check            Check�z��        */
/* @param i_tail_result      Tail���ʔz��     */
/* @param c_tail_char        Tail�����z��     */
int DoubleArray::setDoubleArrayData(
  int i_array_size,
  int i_tail_char_size,
  int i_tail_result_size,
  const int* i_base,
  const int* i_check,
  const int64_t* i_tail_result,
  const char* c_tail_char)
{
  i_array_size_       = i_array_size;       /* �z��T�C�Y       */
  i_tail_char_size_   = i_tail_char_size;   /* Tail������T�C�Y */
  i_tail_result_size_ = i_tail_result_size; /* Tail���ʃT�C�Y   */

  if (keepMemory()) /* �z��T�C�Y���m�肵���̂Ń������m�� */
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


/* DoubleArray�`������f�[�^���Đ� */
/* @param origins �Đ������f�[�^   */
void DoubleArray::reproductionData(
  vector<pair<char*, int64_t> >& origins) const
{
  if (!checkInit()                /* ����������Ă��Ȃ�           */
  || i_tail_result_ == nullptr) { /* ���ʏ�񂪓��ꂳ��Ă������ */
    return;
  }

  vector<int> tail_results; /* ���ʂ��i�[����Ă���Index */
  for (int i = 0; i < i_tail_result_size_; ++i) {
    if (i_tail_result_[i]) {
      tail_results.push_back(i);
    }
  }

  unordered_map<int, int> tail_indexes; /* Tail�ɓ˓�����BaseIndex first:TailIndex(���l) second:BaseIndex */
  for (int i = 0; i < i_array_size_; ++i) {
    if (i_base_[i] < 0) {
      tail_indexes.insert(make_pair(i_base_[i] * -1, i)); /* ���l�𐳒l�ɕϊ� */
    }
  }

  int i_tail_end(0);
  vector<char> datas;
  for (const auto& tail : tail_results) {
    datas.clear();
    int i;
    for (i = tail - 1; i > i_tail_end; --i) { /* �I�[�L�������O����̂�-1 */
      datas.push_back(c_tail_char_[i]); /* Tail�����̃f�[�^ */
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
    c_data[i_index] = 0x00; /* �I�[�L�� */

    origins.push_back(make_pair(c_data, i_tail_result_[tail]));
    i_tail_end = tail;
  }
}


/* ����Index����Byte���𕜌�����                               */
/* @param c_info         ��������Byte��� �Ăяo������delete���� */
/* @param i_reuslt_index �������錋��Index                       */
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
    return; /* �w���Index�͖��� */
  }

  vector<char> datas;
  int i_tail_index(--i_tail_last_index);  /* �I�[�L�����w���Ă���̂ň�O�ɂ��� */
  while (c_tail_char_[i_tail_index]) {
    datas.push_back(c_tail_char_[i_tail_index--]);  /* Tail�����̃f�[�^ */
  }
  ++i_tail_index; /* Tail�擪�̈ʒu�ɂ���̂�Increment */

  int i_array_index(0); /* base�ʒu���� */
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
  c_info[i_index] = 0x00; /* �I�[�L�� */
}
