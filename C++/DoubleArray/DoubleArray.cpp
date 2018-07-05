#include "DoubleArray.h"
#include <fstream>
#include <unordered_map>

using namespace std;

/* init only */
DoubleArray::DoubleArray() : i_base_(nullptr), i_check_(nullptr), c_tail_(nullptr), i_result_(nullptr),
                             i_array_size_(I_DEFAULT_ARRAY_SIZE),
                             i_tail_size_(I_DEFAULT_ARRAY_SIZE),
                             i_result_size_(I_DEFAULT_ARRAY_SIZE)
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
  const bool b_init_size) noexcept
{
  deleteMemory(b_init_size);  /* �����̃f�[�^�\����j�� */

  try {
    i_base_  = new int[i_array_size_];
    i_check_ = new int[i_array_size_];
    c_tail_  = new char[i_tail_size_];
    if (i_result_size_) {
      i_result_ = new int64_t[i_result_size_];
    }

    memset(i_base_,  0,               sizeof(i_base_[0])  * i_array_size_);
    memset(i_check_, I_ARRAY_NO_DATA, sizeof(i_check_[0]) * i_array_size_);
    memset(c_tail_,  0,               sizeof(c_tail_[0])  * i_tail_size_);
    if (i_result_size_) {
      memset(i_result_, 0, sizeof(i_result_[0]) * i_result_size_);
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
  const bool b_init_size) noexcept
{
  if (i_base_)   delete[] i_base_;
  if (i_check_)  delete[] i_check_;
  if (c_tail_)   delete[] c_tail_;
  if (i_result_) delete[] i_result_;

  i_base_   = nullptr;
  i_check_  = nullptr;
  c_tail_   = nullptr;
  i_result_ = nullptr;

  if (b_init_size) {
    i_array_size_  = I_DEFAULT_ARRAY_SIZE;
    i_tail_size_   = I_DEFAULT_ARRAY_SIZE;
    i_result_size_ = I_DEFAULT_ARRAY_SIZE;
  }
}


/* �f�[�^���쐬����Ă��邩�`�F�b�N              */
/* @return true : �f�[�^�쐬����Ă�  false : �� */
bool DoubleArray::checkInit() const noexcept
{
  return i_base_ != nullptr;
}


/* BaseCheck�̃������g��             */
/* @param i_lower_limit �g���Œ�̈� */
/* @return Error Code                */
int DoubleArray::baseCheckExtendMemory(
  const uint64_t i_lower_limit) noexcept
{
  try {
    uint64_t i_new_array_size(i_array_size_);
    do {
      i_new_array_size *= I_EXTEND_MEMORY;
    } while (i_new_array_size <= i_lower_limit);

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
int DoubleArray::tailExtendMemory() noexcept
{
  try {
    int i_new_tail_size(static_cast<int>(i_tail_size_ * I_EXTEND_MEMORY));
    char* c_tail      = new char[i_new_tail_size];
    int64_t* i_result = new int64_t[i_new_tail_size];

    memset(c_tail,   0,         sizeof(c_tail[0])   * i_new_tail_size);
    memset(i_result, 0,         sizeof(i_result[0]) * i_new_tail_size);
    memcpy(c_tail,   c_tail_,   sizeof(c_tail[0])   * i_tail_size_);
    memcpy(i_result, i_result_, sizeof(i_result[0]) * i_result_size_);

    delete[] c_tail_;
    delete[] i_result_;
    c_tail_        = c_tail;
    i_result_      = i_result;
    i_tail_size_   = i_new_tail_size;
    i_result_size_ = i_new_tail_size;
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
  const int i_tail_last_index) noexcept
{
  if (i_array_size_ == 0)
    return I_NO_ERROR;

  for (int64_t i = i_array_size_ - 1; i >= 0; --i) {
    if (i_base_[i]) {
      i_array_size_ = i + 1;
      break;
    }
  }

  i_tail_size_ = i_tail_last_index;
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
    char* c_new_tail = new char[i_tail_size_];
    memcpy(c_new_tail, c_tail_, sizeof(c_new_tail[0]) * i_tail_size_);
    delete[] c_tail_;
    c_tail_ = c_new_tail;

    if (i_result_size_) {
      i_result_size_ = i_tail_size_;  /* �œK���T�C�Y�ɏ������� */
      int64_t* i_new_result = new int64_t[i_result_size_];
      memcpy(i_new_result, i_result_, sizeof(i_new_result[0]) * i_result_size_);
      delete[] i_result_;
      i_result_ = i_new_result;
    }
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* DoubleArray���\�z����                  */
/* @param add_datas DoubleArray�\�z�f�[�^ */
/* @param i_option  �\�z�I�v�V����        */
/* @return 0 : ����I��  0�ȊO : �ُ�I�� */
int DoubleArray::createDoubleArray(
  ByteArrays& add_datas,
  const int i_option) noexcept
{
  if (add_datas.empty())
    return I_NO_ERROR;

  add_datas.sort(); /* Sort */

  /* Trie�\�z */
  TrieArray trie_array;
  createTrie(trie_array, add_datas);

  if (keepMemory(true)) { /* �������m�� */
    return I_FAILED_MEMORY;
  }

  /* DoubleArray�\�z */
  unsigned int base_array[256] = { 1 }; /* ���򂷂�Node��Base�őS�Ă̕����ɓK������l��T���̂Ɏg�p */
  int i_tail_index(1), i_base_index(0); /* Base��Tail�̏����l */
  if (recursiveCreateDoubleArray(i_tail_index, base_array, trie_array, i_base_index, 0)) {
    return I_FAILED_MEMORY;
  }

  if (i_option & I_TAIL_UNITY) {
    delete[] i_result_;
    i_result_      = nullptr;
    i_result_size_ = 0;
  }

  return optimizeMemory(i_tail_index);
}


/* ���̓f�[�^����TRIE�\�����\�z����    */
/* @param trie_array  �\�z����TRIE�\�� */
/* @param byte_arrays ��ɂ���f�[�^   */
void DoubleArray::createTrie(
  TrieArray& trie_array,
  const ByteArrays& byte_arrays) const noexcept
{
  uint64_t i_max_length;
  size_t i_trie_array_size(byte_arrays.size());  /* �K���ɓ��̓f�[�^��(�����Ȃ�) */
  trie_array.resize(i_trie_array_size);
  vector<pair<uint64_t, uint64_t>> positions(byte_arrays.size(), make_pair(0, 0)); /* TailPosition */
  createOverlapPositions(i_max_length, positions, byte_arrays); /* TailPosition�f�[�^�쐬 */

  size_t i_used_index(0);
  vector<uint64_t> trie_indexes(i_max_length);
  const uint64_t i_max_value = numeric_limits<uint64_t>::max();
  for (size_t i = 0, i_size = byte_arrays.size(); i < i_size; ++i) {
    const auto i_start_index(positions[i].first);
    if (i_start_index == i_max_value) /* �f�[�^���d�����`�F�b�N */
      continue;

    uint64_t i_tail_index(positions[i].second);
    size_t i_trie_index(trie_indexes[i_start_index]);
    const auto& byte_array = byte_arrays[i];
    for (uint64_t n = i_start_index; n < i_tail_index; ++n) {
      unsigned char c_one_word = byte_array.c_byte_[n];
      trie_indexes[n] = i_trie_index;
      auto& layers    = trie_array[i_trie_index];
      auto layer      = lower_bound(begin(layers), end(layers), c_one_word,
                                    [] (const auto& layer, const unsigned char c_byte) {return layer.c_byte_ < c_byte;});
      i_trie_index = (++i_used_index);
      layers.insert(layer, move(TrieLayer(c_one_word, i_used_index, nullptr)));
      if (i_trie_array_size <= i_used_index) {
        i_trie_array_size *= I_EXTEND_TRIE;
        trie_array.resize(i_trie_array_size);
      }
    }

    char* c_tail(nullptr);
    int64_t i_tail_size(0);
    if (i_tail_index < byte_array.i_byte_length_ - 1) {
      i_tail_size = byte_array.i_byte_length_ - i_tail_index - 1;
      c_tail      = new char[i_tail_size];
      memcpy(c_tail, (byte_array.c_byte_ + i_tail_index + 1), i_tail_size);
    }

    ++i_used_index;
    trie_indexes[i_tail_index] = i_trie_index;
    unsigned char c_one_word = byte_array.c_byte_[i_tail_index];
    auto& layers    = trie_array[i_trie_index];
    auto trie_parts = new TrieParts(c_tail, i_tail_size, byte_array.result_);
    auto layer      = lower_bound(begin(layers), end(layers), c_one_word,
                                  [] (const auto& layer, const unsigned char c_byte) {return layer.c_byte_ < c_byte;});
    layers.insert(layer, move(TrieLayer(c_one_word, I_TRIE_TAIL_VALUE, trie_parts)));
  }
  trie_array.resize(i_used_index);
}


/* @param i_tail_index �������݊J�nTailIndex           */
/* @param base_array   BaseValue�̒l�����肷��̂Ɏg�p */
/* @param trie_array   TRIE�S�f�[�^                    */
/* @param i_base_index ���BaseCheckIndex            */
/* @param i_trie_index �Ώۂ�Trie�m�[�h�Q              */
/* @return Error Code                                  */
int DoubleArray::recursiveCreateDoubleArray(
  int& i_tail_index,
  unsigned int* base_array,
  TrieArray& trie_array,
  const int i_base_index,
  const size_t i_trie_index) noexcept
{
  unsigned int i_base_value;
  const auto& layers = trie_array[i_trie_index];
  if (getBaseValue(i_base_value, base_array, layers)) {
    return I_FAILED_MEMORY; /* �\�z���s */
  }
  i_base_[i_base_index] = i_base_value;

  /* �ċA��������O��check��ݒ� */
  for_each (begin(layers), end(layers), [&](const auto& trie) {i_check_[trie.c_byte_ + i_base_value] = i_base_index;});

  for (auto& layer : layers) {
    int i_insert(layer.c_byte_ + i_base_value);
    if (layer.trie_parts_) {
      i_base_[i_insert] = (-i_tail_index);  /* TailIndex�̓}�C�i�X�l */

      if (setTailInfo(i_tail_index, layer.trie_parts_)) {
        return I_FAILED_MEMORY; /* �\�z���s */
      }
      delete layer.trie_parts_;
    }
    if (layer.i_next_trie_index_ != I_TRIE_TAIL_VALUE) {
      if (recursiveCreateDoubleArray(i_tail_index, base_array, trie_array, i_insert, layer.i_next_trie_index_)) {
        return I_FAILED_MEMORY; /* �\�z���s */
      }
    }
  }

  return I_NO_ERROR;
}


/* Base�̒l�����߂�                                    */
/* @param i_base_value ���߂�Base�l                    */
/* @param base_array   BaseValue�̒l�����肷��̂Ɏg�p */
/* @param layers       TRIE�ł̓�����                */
/* @return Error Code                                  */
int DoubleArray::getBaseValue(
  unsigned int& i_base_value,
  unsigned int* base_array,
  const vector<TrieLayer>& layers) noexcept
{
  auto layer_begin = cbegin(layers);
  auto layer_end   = cend(layers);

  /* �����̃o�C�g���̍ő�Base�l������ */
  i_base_value = 0;
  for (auto layer = layer_begin; layer != layer_end; ++layer) {
    if (base_array[layer->c_byte_] > i_base_value) {
      i_base_value = base_array[layer->c_byte_];
    }
  }

  bool b_success(false);
  unsigned char c_byte_max = layers.rbegin()->c_byte_;
  do {
    ++i_base_value;
    if (i_array_size_ <= (c_byte_max + i_base_value)) {
      if (baseCheckExtendMemory(c_byte_max + i_base_value)) {
        return I_FAILED_MEMORY;
      }
    }

    uint64_t i_count = i_array_size_ - (c_byte_max + i_base_value);
    for (uint64_t i = 0; i < i_count; ++i, ++i_base_value) {
      bool b_find(true);
      for (auto layer = layer_begin; layer != layer_end; ++layer) {
        if (i_check_[layer->c_byte_ + i_base_value] != I_ARRAY_NO_DATA) {
          b_find = false;
          break;
        }
      }
      if (b_find) {
        b_success = true;
        break;
      }
    }
  } while (b_success == false);

  for_each (layer_begin, layer_end, [&](const auto& layer) {base_array[layer.c_byte_] = i_base_value;});

  return I_NO_ERROR;
}


/* Tail�ɏ���ݒ肷��                 */
/* @param i_tail_index Tail�i�[�J�n�ʒu */
/* @param trie_parts   TrieParts        */
/* @return Error code                   */
int DoubleArray::setTailInfo(
  int& i_tail_index,
  const TrieParts* trie_parts) noexcept
{
  while (i_tail_index + trie_parts->i_tail_size_ >= i_tail_size_) {
  //if (i_tail_size_ <= i_tail_index + trie_parts->i_tail_size_) {
    if (tailExtendMemory()) { /* ���������s�������̂Ŋg�� */
      return I_FAILED_MEMORY;
    }
  }

  if (trie_parts->c_tail_) {
    for (uint64_t i = 0, i_tail_size = trie_parts->i_tail_size_; i < i_tail_size; ++i) {
      c_tail_[i_tail_index++] = trie_parts->c_tail_[i];
    }
    --i_tail_index;
  } else {
    c_tail_[i_tail_index] = C_TAIL_CHAR;
  }
  i_result_[i_tail_index++] = trie_parts->i_result_;

  return I_NO_ERROR;
}


/* �d��Index���쐬                  */
/* @param positions start,tail Index  */
/* @param datas          �S�ǉ��f�[�^ */
void DoubleArray::createOverlapPositions(
  uint64_t& i_max_length,
  vector<pair<uint64_t, uint64_t>>& positions,
  const ByteArrays& datas) const noexcept
{
  if (datas.size() == 1) {
    i_max_length = datas.front().i_byte_length_;
    return;
  }

  /* �擪 */
  auto top  = datas.cbegin();
  auto next = top;
  ++next;
  i_max_length = top->i_byte_length_;
  uint64_t i_top_same_index(top->i_byte_length_);
  searchSameIndex(i_top_same_index, top->i_byte_length_, top->c_byte_, next->c_byte_);
  if (i_top_same_index == next->i_byte_length_) positions.begin()->first = numeric_limits<uint64_t>::max();
  else                                          positions.begin()->second = i_top_same_index;

  /* ���� */
  auto last    = datas.crbegin();
  auto one_ago = last;
  ++last;
  uint64_t i_last_same_index = 0;
  searchSameIndex(i_last_same_index, min(last->i_byte_length_, one_ago->i_byte_length_), last->c_byte_, one_ago->c_byte_);
  auto& last_position  = *positions.rbegin();
  last_position.first  = i_last_same_index;
  last_position.second = i_last_same_index;
  if (i_max_length < last->i_byte_length_) {
    i_max_length = last->i_byte_length_;
  }

  /* �擪���������� */
  uint64_t i_before_same_index(i_top_same_index);
  for (uint64_t i = 1, i_last = datas.size() - 1; i < i_last; ++i) {
    const auto& current = datas[i];
    const auto& after   = datas[i+1];
    const char* c_current_byte = current.c_byte_;
    uint64_t i_after_same_index(current.i_byte_length_);
    if (i_max_length < i_after_same_index) {
      i_max_length = i_after_same_index;
    }
 
    searchSameIndex(i_after_same_index,  current.i_byte_length_, c_current_byte, after.c_byte_);
    if (i_after_same_index == after.i_byte_length_) {
      positions[i].first = numeric_limits<uint64_t>::max();  /* ����f�[�^���� */
    } else {
      positions[i].first  = i_before_same_index;
      positions[i].second = (i_before_same_index < i_after_same_index ? i_after_same_index : i_before_same_index);
      i_before_same_index = i_after_same_index;
    }
  }
}


/* 2�̔z���擪���猟�����ē��e���قȂ�Index���擾 */
/* @param i_result ��������                           */
/* @param i_length �����ő�Index                      */
/* @param c_first  �����z��1                          */
/* @param c_second �����z��2                          */
void DoubleArray::searchSameIndex(
  uint64_t& i_result,
  const uint64_t i_length,
  const char* c_first,
  const char* c_second) const noexcept
{
  for (uint64_t i = 0; i < i_length; ++i) {
    if (c_first[i] != c_second[i]) {
      i_result = i;
      break;
    }
  }
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
  uint64_t i(0);
  int i_base_index(0), i_check_index(0);
  for (; i <= i_byte_length; ++i) { /* �I�[�L���̕�������̂�<=�Ƃ��� */
    i_check_index  = i_base_[i_base_index];
    i_check_index += static_cast<unsigned char>(c_byte[i]);
    if (i_check_[i_check_index] != i_base_index) {
      return I_SEARCH_NOHIT;  /* �f�[�^�����݂��Ȃ� */
    }

    if (i_base_[i_check_index] < 0) {
      break;
    }
    i_base_index = i_check_index;
  }

  /* Tail���� */
  uint64_t i_tail_index(-i_base_[i_check_index]); /* Tail�˓��_�@�̃}�C�i�X�l���v���X�ɕϊ� */
  if (i < i_byte_length) {
    ++i;
    uint64_t i_compare_length(i_byte_length - i);
    if (memcmp(&c_tail_[i_tail_index], &c_byte[i], i_compare_length + 1) == 0) {
      return (i_result_ == nullptr ? I_HIT_DEFAULT : i_result_[i_tail_index + i_compare_length]);
    }
  } else if (i == i_byte_length) {
    return (i_result_ == nullptr ? I_HIT_DEFAULT : i_result_[i_tail_index]);
  }

  return I_SEARCH_NOHIT;
}


/* �r���o�ߏ�Ԃ��擾���Ȃ��猟������            */
/* @param sarch_parts   search position          */
/* @param result        search result            */
/* @param c_byte        search bytes             */
/* @param i_byte_length search data length       */
/* @return true : ����������  false : ���������� */
bool DoubleArray::searchContinue(
  DASearchParts& search_parts,
  int64_t& result,
  const char* c_byte,
  const uint64_t i_byte_length) const noexcept
{
  result = I_SEARCH_NOHIT;

  uint64_t i(0), i_byte_index(0);
  if (search_parts.i_tail_ == 0) {  /* Tail�܂Ői��ł��Ȃ� */
    for (; i < i_byte_length; ++i) {
      search_parts.i_check_ = i_base_[search_parts.i_base_] + static_cast<unsigned char>(c_byte[i]);
      if (i_check_[search_parts.i_check_] != search_parts.i_base_) {
        return false; /* �f�[�^�����݂��Ȃ� */
      }

      if (i_base_[search_parts.i_check_] < 0) {
        i_byte_index = i + 1;
        search_parts.i_tail_ = -i_base_[search_parts.i_check_]; /* �}�C�i�X�l���v���X�ɕϊ� */
        break;
      }
      search_parts.i_base_ = search_parts.i_check_;
    }

    if (i >= i_byte_length) {
      if (i_check_[i_base_[search_parts.i_base_]] == search_parts.i_base_) {
        const int i_tail_index(i_base_[i_base_[search_parts.i_base_]]);
        if (i_tail_index < 0) {
          if (i_result_) result = i_result_[-i_tail_index]; /* �q�b�g */
          else           result = I_HIT_DEFAULT;
        }
        return true;
      }
    }
  }

  /* Tail���� */
  uint64_t i_compare_size(i_byte_length - i_byte_index);
  if (memcmp(&c_tail_[search_parts.i_tail_], &c_byte[i_byte_index], i_compare_size) != 0) {
    return false;  /* �f�[�^�����݂��Ȃ��@Tail�̓r���ŕs��v */
  }

  i_byte_index         += i_compare_size;
  search_parts.i_tail_ += static_cast<int>(i_compare_size);

  if (i_byte_index >= i_byte_length) {
    if (c_tail_[search_parts.i_tail_] == C_TAIL_CHAR) {
      if (i_result_) result = i_result_[search_parts.i_tail_];  /* �q�b�g */
      else           result = I_HIT_DEFAULT;
    } else {
      return true;
    }
  }

  return false;
}


/* DoubleArray������������                         */
/* @param i_write_size �������񂾃f�[�^�T�C�Y        */
/* @param fp           OutputFileStream              */
/* @return I_DA_NO_ERROR : ����I�� 0�ȊO : �ُ�I�� */
int DoubleArray::writeBinary(
  int64_t& i_write_size,
  FILE* fp) const noexcept
{
  try {
    if (checkInit()) {
      fwrite(&i_array_size_,  sizeof(i_array_size_),  1,             fp);
      fwrite(&i_tail_size_,   sizeof(i_tail_size_),   1,             fp);
      fwrite(&i_result_size_, sizeof(i_result_size_), 1,             fp);
      fwrite(i_base_,         sizeof(i_base_[0]),     i_array_size_, fp);
      fwrite(i_check_,        sizeof(i_check_[0]),    i_array_size_, fp);
      fwrite(c_tail_,         sizeof(c_tail_[0]),     i_tail_size_,  fp);
      if (i_result_size_) {
        fwrite(i_result_, sizeof(i_result_[0]), i_result_size_, fp);
      }

      i_write_size += sizeof(i_array_size_);
      i_write_size += sizeof(i_tail_size_);
      i_write_size += sizeof(i_result_size_);
      i_write_size += sizeof(i_base_[0])  * i_array_size_;
      i_write_size += sizeof(i_check_[0]) * i_array_size_;
      i_write_size += sizeof(c_tail_[0])  * i_tail_size_;
      if (i_result_size_) {
        i_write_size += sizeof(i_result_[0]) * i_result_size_;
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
  FILE* fp) noexcept
{
  try {
    if (1 != fread(&i_array_size_, sizeof(i_array_size_), 1, fp)) /* �z��T�C�Y */
      throw;

    if (i_array_size_) {
      if ((1 != fread(&i_tail_size_,   sizeof(i_tail_size_),   1, fp)) /* Tail������T�C�Y */
      ||  (1 != fread(&i_result_size_, sizeof(i_result_size_), 1, fp)) /* Tail���ʃT�C�Y   */
      ||  (keepMemory())) /* �z��T�C�Y���m�肵���̂Ń������m�� */
        throw;

      if ((static_cast<size_t>(i_array_size_) != fread(i_base_,  sizeof(i_base_[0]),  i_array_size_, fp))   /* Base     */
      ||  (static_cast<size_t>(i_array_size_) != fread(i_check_, sizeof(i_check_[0]), i_array_size_, fp))   /* Check    */
      ||  (static_cast<size_t>(i_tail_size_)  != fread(c_tail_,  sizeof(c_tail_[0]),  i_tail_size_,  fp)))  /* Tail���� */
        throw;

      if (i_result_size_) {
        if (static_cast<size_t>(i_result_size_) != fread(i_result_, sizeof(i_result_[0]), i_result_size_, fp)) /* Tail���� */
          throw;
      } else {
        i_result_ = nullptr;
      }

      i_read_size += sizeof(i_tail_size_);
      i_read_size += sizeof(i_result_size_);
      i_read_size += sizeof(i_base_[0])  * i_array_size_;
      i_read_size += sizeof(i_check_[0]) * i_array_size_;
      if (i_result_size_) {
        i_read_size += sizeof(i_result_[0]) * i_result_size_;
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


/* �����f�[�^���擾����                  */
/* @param i_array_size  �z��T�C�Y       */
/* @param i_tail_size   Tail������T�C�Y */
/* @param i_result_size Tail���ʃT�C�Y   */
/* @param i_base        Base�z��         */
/* @param i_check       Check�z��        */
/* @param i_result      Tail���ʔz��     */
/* @param c_tail        Tail�����z��     */
void DoubleArray::getDoubleArrayData(
  uint64_t& i_array_size,
  uint64_t& i_tail_size,
  uint64_t& i_result_size,
  const int*& i_base,
  const int*& i_check,
  const int64_t*& i_result,
  const char*& c_tail) const noexcept
{
  i_array_size  = i_array_size_;
  i_tail_size   = i_tail_size_;
  i_result_size = i_result_size_;
  i_base        = i_base_;
  i_check       = i_check_;
  i_result      = i_result_;
  c_tail        = c_tail_;
}


/* �����f�[�^���O������ݒ肷��          */
/* @param i_array_size  �z��T�C�Y       */
/* @param i_tail_size   Tail������T�C�Y */
/* @param i_result_size Tail���ʃT�C�Y   */
/* @param i_base        Base�z��         */
/* @param i_check       Check�z��        */
/* @param i_result      Tail���ʔz��     */
/* @param c_tail        Tail�����z��     */
int DoubleArray::setDoubleArrayData(
  int i_array_size,
  int i_tail_size,
  int i_result_size,
  const int* i_base,
  const int* i_check,
  const int64_t* i_result,
  const char* c_tail) noexcept
{
  i_array_size_  = i_array_size;  /* �z��T�C�Y       */
  i_tail_size_   = i_tail_size;   /* Tail������T�C�Y */
  i_result_size_ = i_result_size; /* Tail���ʃT�C�Y   */

  if (keepMemory()) /* �z��T�C�Y���m�肵���̂Ń������m�� */
    return I_FAILED_MEMORY;

  try {
    memcpy(i_base_,  i_base,  sizeof(i_base_[0])  * i_array_size_);
    memcpy(i_check_, i_check, sizeof(i_check_[0]) * i_array_size_);
    memcpy(c_tail_,  c_tail,  sizeof(c_tail_[0])  * i_tail_size_);
    if (i_result) {
      memcpy(i_result_, i_result, sizeof(i_result_[0]) * i_result_size_);
    } else {
      i_result_ = nullptr;
    }
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* ����Index����Byte���𕜌�����                               */
/* @param c_info         ��������Byte��� �Ăяo������delete���� */
/* @param i_reuslt_index �������錋��Index                       */
void DoubleArray::reproductionFromIndex(
  char*& c_info,
  const int64_t i_result_index) const noexcept
{
  c_info = nullptr;
  uint64_t i_last(0);
  for (uint64_t i = 0; i < i_result_size_; ++i) {
    if (i_result_[i] == i_result_index) {
      i_last = i;
    }
  }

  if (i_last == 0) {
    return; /* �w���Index�͖��� */
  }

  vector<char> datas;
  int64_t i_tail_index(--i_last);  /* �I�[�L�����w���Ă���̂ň�O�ɂ��� */
  while (c_tail_[i_tail_index]) {
    datas.push_back(c_tail_[i_tail_index--]);  /* Tail�����̃f�[�^ */
  }
  ++i_tail_index; /* Tail�擪�̈ʒu�ɂ���̂�Increment */

  uint64_t i_array_index(0); /* base�ʒu���� */
  for (uint64_t i = 0; i < i_array_size_; ++i) {
    if ((i_base_[i] < 0)
    &&  ((-i_base_[i]) == i_tail_index)) {
      i_array_index = i;
      break;
    }
  }

  while (i_array_index != 0) {
    uint64_t i_char = (i_array_index - i_base_[i_check_[i_array_index]]) & 0xff;
    datas.push_back(static_cast<char>(i_char));
    i_array_index = i_check_[i_array_index];
  }

  int i_index(0);
  c_info = new char[datas.size() + 1];
  for (auto data = datas.crbegin(), data_end = datas.crend(); data != data_end; ++data) {
    c_info[i_index++] = *data;
  }
  c_info[i_index] = 0x00; /* �I�[�L�� */
}

