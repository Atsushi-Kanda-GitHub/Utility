#ifndef DOUBLEARRAY_H
#define DOUBLEARRAY_H

/**
 * DoubleArray<br/>
 * �v�f��1�o�C�g�Ƃ��ď������Ă���B<br/>
 * �f�[�^�\���\�z��A�ǉ������ɂ͑Ή����Ă��Ȃ��B<br/>
 * �������ʂ̃f�[�^�ɂ��ẮA�Ăяo�����ŊǗ����Ă��炢�A<br/>
 * �_�u���z����ł̓������Ǘ��͂��Ȃ��B<br/>
 * �܂��ACode�̔z��͂Ȃ��B�������1byte�����̂܂܎g�p���Ă���
 *
 * @brief�_�u���z��
 * @file DoubleArray.h
 * @author dev.atsushi.kanda@gmail.com
 * @date 2018/01/014
 */

#include <stdio.h>
#include <string.h>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <limits>

class TrieNodeParts;
class TrieNode;
class DASearchParts;
class ByteArray;
class ByteArrays;

/** DoubleArray�̍\�z&���� */
class DoubleArray
{
  using TrieArray = std::vector<std::vector<TrieNode>>;

public:
  static constexpr int I_NO_ERROR         = 0x00; /* Normal                     */
  static constexpr int I_FAILED_TRIE      = 0x01; /* failed to create TRIE      */
  static constexpr int I_FAILED_MEMORY    = 0x02; /* Memory�֘AERROR            */
  static constexpr int I_FAIELD_FILE_IO   = 0x04; /* FILE ERROR                 */
  static constexpr int I_NO_OPTION        = 0x00; /* no option                  */
  static constexpr int I_TAIL_UNITY       = 0x01; /* �������ʂ�true/false�ɕϊ� */
  static constexpr int64_t I_HIT_DEFAULT  = 0x01; /* �������ʓ������̕Ԃ�l     */
  static constexpr int64_t I_SEARCH_NOHIT = 0x00; /* search no result           */

  static constexpr int I_ARRAY_NO_DATA      =  -1;  /* �z�񏉊��l          */
  static constexpr int I_EXTEND_MEMORY      =   2;  /* �z��g���{��        */
  static constexpr int I_DEFAULT_ARRAY_SIZE = 256;  /* default�̔z��T�C�Y */
  static constexpr char C_TAIL_CHAR         = 0x00; /* TAIL�̖�������      */
  static constexpr size_t I_TRIE_TAIL_VALUE = std::numeric_limits<size_t>::max();

public:
  /** init only */
  DoubleArray();

  /** delete memory */
  ~DoubleArray();

  /** DoubleArray���\�z����
  * @param add_datas DoubleArray�\�z�f�[�^
  * @param i_option  �\�z�I�v�V����
  * @return 0 : ����I��  0�ȊO : �ُ�I��
  */
  int createDoubleArray(
    ByteArrays& add_datas,
    const int i_option = I_NO_OPTION) noexcept;

  /** ��������
  * c_byte�ɂ�NULL���r���Ɋ܂܂��\��������ׁA
  * �o�C�g�����n���Ȃ��ƃ_���B
  * �����̃o�C�g�񖖔���NULL���g�p����
  * @param c_byte        search bytes
  * @param i_byte_length search data length
  * @return search result
  */
  int64_t search(
    const char* c_byte,
    const uint64_t i_byte_length) const noexcept;

  /** �r���o�ߏ�Ԃ��擾���Ȃ��猟������
  * @param sarch_parts   search position
  * @param result        search result
  * @param c_byte        search bytes
  * @param i_byte_length search data length
  * @return true : ����������  false : ����������
  */
  bool searchContinue(
    DASearchParts& search_parts,
    int64_t& result,
    const char* c_byte,
    const uint64_t i_byte_length) const noexcept;

  /** DoubleArray������������
  * @param i_write_size �������񂾃f�[�^�T�C�Y
  * @param fp           OutputFileStream
  * @return I_DA_NO_ERROR : ����I�� 0�ȊO : �ُ�I��
  */
  int writeBinary(
    int64_t& i_write_size,
    FILE* fp) const noexcept;

  /** DoubleArray����ǂݍ���
  * @param i_read_size �ǂݍ��񂾃f�[�^�T�C�Y
  * @param fp          InputFileStream
  * @return I_DA_NO_ERROR : ����I�� 0�ȊO : �ُ�I��
  */
  int readBinary(
    int64_t& i_read_size,
    FILE* fp) noexcept;

  /** �����f�[�^���擾����
  * @param i_array_size  �z��T�C�Y
  * @param i_tail_size   Tail������T�C�Y
  * @param i_result_size Tail���ʃT�C�Y
  * @param i_base        Base�z��
  * @param i_check       Check�z��
  * @param i_result      Tail���ʔz��
  * @param c_tail        Tail�����z��
  * @return
  */
  void getDoubleArrayData(
    uint64_t& i_array_size,
    uint64_t& i_tail_size,
    uint64_t& i_result_size,
    const int*& i_base,
    const int*& i_check,
    const int64_t*& i_result,
    const char*& c_tail) const noexcept;

  /** �����f�[�^���O������ݒ肷��
  * @param i_array_size  �z��T�C�Y
  * @param i_tail_size   Tail������T�C�Y
  * @param i_result_size Tail���ʃT�C�Y
  * @param i_base        Base�z��
  * @param i_check       Check�z��
  * @param i_result      Tail���ʔz��
  * @param c_tail        Tail�����z��
  * @return
  */
  int setDoubleArrayData(
    int i_array_size,
    int i_tail_size,
    int i_result_size,
    const int* i_base,
    const int* i_check,
    const int64_t* i_result,
    const char* c_tail) noexcept;

  /** �f�[�^���쐬����Ă��邩�`�F�b�N
  * @param
  * @return true : �f�[�^�쐬����Ă�  false : ��
  */
  bool checkInit() const noexcept;

  /** ����Index����Byte���𕜌�����
  * @param c_info         ��������Byte��� �Ăяo������delete����
  * @param i_result_index �������錋��Index
  * @return
  */
  void reproductionFromIndex(
    char*& c_info,
    const int64_t i_result_index) const noexcept;

private:
  /** �������m��
  * @param b_init_size �T�C�Y�����������邩
  * @return Error Code
  */
  int keepMemory(
    const bool b_init_size = false) noexcept;

  /** �������j��
  * @param b_init_size �T�C�Y�����������邩
  * @return
  */
  void deleteMemory(
    const bool b_init_size = false) noexcept;

  /** BaseCheck�̃������g��
  * @param i_lower_limit �g���Œ�̈�
  * @return Error Code
  */
  int baseCheckExtendMemory(
    const uint64_t i_lower_limit) noexcept;

  /** Tail�̃������g��
  * @param i_lower_limit �g���Œ�̈�
  * @return Error Code
  */
  int tailExtendMemory(
    const uint64_t i_lower_limit) noexcept;

  /** Memory�T�C�Y���œK������
  * @param i_tail_last_index Tail�z��̃f�[�^���i�[����Ă���ŏIIndex
  * @return Error Code
  */
  int optimizeMemory(
    const int i_tail_last_index) noexcept;

  /** ���̓f�[�^����TRIE�\�����\�z����
  * @param trie_array  �\�z����TRIE�\��
  * @param byte_arrays ��ɂ���f�[�^
  * @return
  */
  void createTrie(
    TrieArray& trie_array,
    const ByteArrays& byte_arrays) const noexcept;

  /** Trie�\������ċA�I��DoubleArray���\�z����
  * @param i_tail_index �������݊J�nTailIndex
  * @param base_array   BaseValue�̒l�����肷��̂Ɏg�p
  * @param trie_array   TRIE�S�f�[�^
  * @param i_base_index ���BaseCheckIndex
  * @param i_trie_index �Ώۂ�Trie�m�[�h�Q
  * @return Error Code
  */
  int recursiveCreateDoubleArray(
    int& i_tail_index,
    unsigned int* base_array,
    TrieArray& trie_array,
    const int i_base_index,
    const size_t i_trie_index) noexcept;

  /** Base�̒l�����߂�
  * @param i_base_value ���߂�Base�l
  * @param base_array   BaseValue�̒l�����肷��̂Ɏg�p
  * @param layers       TRIE�ł̓�����
  * @return Error Code
  */
  int getBaseValue(
    unsigned int& i_base_value,
    unsigned int* base_array,
    const std::vector<TrieNode>& layers) noexcept;

  /** Tail�ɏ���ݒ肷��
  * @param i_tail_index Tail�i�[�J�n�ʒu
  * @param node_parts   Trie node parts
  * @return Error code
  */
  int setTailInfo(
    int& i_tail_index,
    const TrieNodeParts* node_parts) noexcept;

  /** SameIndex�����쐬
  * @param i_max_length �Œ��f�[�^��
  * @param positions    start,tail Indexx
  * @param datas        �S�ǉ��f�[�^
  * @return
  */
  void createOverlapPositions(
    uint64_t& i_max_length,
    std::vector<std::pair<uint64_t, uint64_t>>& positions,
    const ByteArrays& datas) const noexcept;

private:
  /** BASE�z�� */
  int* i_base_;

  /** CHECL�z�� */
  int* i_check_;

  /** TAIL�����z�� */
  char* c_tail_;

  /** ���ʔz�� */
  int64_t* i_result_;

  /** �v�f���T�C�Y */
  uint64_t i_array_size_;

  /** Tail������T�C�Y */
  uint64_t i_tail_size_;

  /** Tail���ʔz��T�C�Y */
  uint64_t i_result_size_;
};

/** �����o�ߏ�ԏ�� */
class DASearchParts
{
public:
  /** zero clear */
  DASearchParts() noexcept : i_base_(0), i_check_(0), i_tail_(0) {}

  /** cost�팸�̂���virtual�͕t�����Ȃ� */
  ~DASearchParts() noexcept {}

  /** Copy */
  DASearchParts(const DASearchParts& parts) noexcept
    : i_base_(parts.i_base_), i_check_(parts.i_check_), i_tail_(parts.i_tail_) {}

  /** init */
  void init() noexcept {
    i_base_  = 0;
    i_check_ = 0;
    i_tail_  = 0;
  }

public:
  /** base position */
  int i_base_;

  /** check position */
  int i_check_;

  /** tail position */
  int i_tail_;
};

/** TrieiNode */
class TrieNode
{
public:
  /** init�̂� */
  TrieNode(unsigned char c_byte, uint64_t i_next_trie_index, TrieNodeParts* node_parts) noexcept
    : c_byte_(c_byte), i_next_trie_index_(i_next_trie_index), node_parts_(node_parts) {}

  /** cost�팸�̂���virtual�͕t�����Ȃ� */
  ~TrieNode() noexcept {}

  /** move */
  TrieNode(TrieNode&& trie_node) noexcept
    : c_byte_(trie_node.c_byte_),
      i_next_trie_index_(trie_node.i_next_trie_index_),
      node_parts_(trie_node.node_parts_) {}

  /** = operator */
  TrieNode& operator = (const TrieNode& trie_node)
  {
    c_byte_            = trie_node.c_byte_;
    i_next_trie_index_ = trie_node.i_next_trie_index_;
    node_parts_        = trie_node.node_parts_;
    return *this;
  }

public:
  /** Layer�ł�Byte��� */
  unsigned char c_byte_;

  /** ������TrieArrayIndex */
  uint64_t i_next_trie_index_;

  /** TrieNodePartsInfo */
  TrieNodeParts* node_parts_;
};

/** Trie Node Parts DoubleArray�\�z���Ɏg�p */
class TrieNodeParts
{
public:
  TrieNodeParts() : c_tail_(nullptr), i_tail_size_(0), i_result_(0) {}
  TrieNodeParts(
    char* c_tail,
    const uint64_t i_tail_size,
    const int64_t result) : c_tail_(c_tail), i_tail_size_(i_tail_size), i_result_(result) {}

  ~TrieNodeParts() noexcept
  {
    if (c_tail_) {
      delete[] c_tail_;
      c_tail_ = 0;
    }
  }

public:
  /** Tail�Ɋi�[�\�ȏꍇ�Ɏg���f�[�^ */
  char* c_tail_;

  /** Tail�̃f�[�^�T�C�Y */
  uint64_t i_tail_size_;

  /** �����f�[�^�����݂����ꍇ�̌��� */
  int64_t i_result_;
};

/** DoubleArray�Ɋi�[����f�[�^�\�� */
class ByteArray
{
public:
  ByteArray() : c_byte_(nullptr), i_byte_length_(0), result_(0) {}

  /** �R���X�g���N�^ */
  ByteArray(
    const char* c_byte,
    const uint64_t i_byte_length,
    const int64_t result) : i_byte_length_(i_byte_length), result_(result)
  {
    c_byte_ = new char[i_byte_length_ + 1]; /* �I�[�L����+1 */
    memcpy(c_byte_, c_byte, i_byte_length_);
    c_byte_[i_byte_length_] = 0x00; /* �I�[�L�� */
    ++i_byte_length_; /* �I�[�L���܂Ŋ܂߂�Length�ɂ��� */
  }

  /** copy */
  ByteArray(const ByteArray& byte_array) : i_byte_length_(byte_array.i_byte_length_), result_(byte_array.result_)
  {
    if (i_byte_length_) {
      c_byte_ = new char[i_byte_length_];
      memcpy(c_byte_, byte_array.c_byte_, i_byte_length_);
    }
  }

  /** �������Ȃ� */
  ~ByteArray() {}

  /** move */
  ByteArray(ByteArray&& byte_array) noexcept
    : c_byte_(byte_array.c_byte_), i_byte_length_(byte_array.i_byte_length_), result_(byte_array.result_) {}

  /** = operator */
  ByteArray& operator = (const ByteArray& byte_array)
  {
    c_byte_        = byte_array.c_byte_;
    i_byte_length_ = byte_array.i_byte_length_;
    result_        = byte_array.result_;
    return *this;
  }

  /** ByteData���폜 */
  void deleteByteArray() noexcept
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

  /** �����f�[�^�����݂����ꍇ�̌��� */
  int64_t result_;
};

/** DoubleArray�\�z�Ɏg�p����Data */
class ByteArrays : public std::vector<ByteArray>
{
public:
  ByteArrays() {}
  ~ByteArrays() noexcept
  {
    for (auto& data : *this) {
      data.deleteByteArray();
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
    const int64_t result) noexcept
  {
    if (i_byte_length != 0) {
      emplace_back(c_byte, i_byte_length, result);
    }
  }

  /** sort */
  void sort() noexcept
  {
    std::sort(begin(), end(), [] (const auto& first, const auto& second) {
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
  
