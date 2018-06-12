#ifndef CRF_H
#define CRF_H

/**
* Conditional Random Fields
*
* @brief  �@�B�w�KCRF
* @file   CRF.h
* @author dev.atsushi.kanda@gmail.com
* @date   2018/06/12
*/

#include "DoubleArray.h"
#include <set>
#include <map>
#include <vector>

class CRFFeatureIndex;

class CRF
{
public:
  static constexpr int     I_CRF_NORMAL   =  0;
  static constexpr int     I_CRF_IO_ERROR =  1;
  static constexpr int64_t I_UNKNOWN      = -1;
  static constexpr int64_t I_CRF_BOS      =  0;
  static constexpr int64_t I_CRF_EOS      =  1;

public:
  /** �������Ȃ� */
  CRF() {}
  
  /** �������Ȃ� */
  virtual ~CRF() {}

  /** �w�K
   * @param i_label_size     Label��
   * @param learn_datas      �w�K�f�[�^
   * @param i_roop           �w�K�J��Ԃ���
   * @param d_eta            �w�K��
   * @baram d_regularization ������
   * @return 0 : ����I�� 0�ȊO : �ُ�I��
   */
  int learn(
    const int64_t i_label_size,
    const std::vector<std::vector<CRFFeatureIndex>>& learn_datas,
    const int i_roop = 5,
    const double d_eta = 1.0,
    const double d_regularization = 10);

  /** ����
   * @param results ���茋��
   * @param inputs  ���肷����
   * @return
   */
  void judge(
    std::vector<int64_t>& results,
    const std::vector<int64_t>& inputs) const;

  /** ����
   * @param results ���茋��
   * @param inputs  ���肷������t��
   * @return �œK���[�g�l
   */
  double judge(
    std::vector<int64_t>& results,
    const std::vector<std::pair<int64_t, const std::vector<int64_t>*>>& inputs) const;

  /** CRF����Binary�ŏ����o��
   * @param i_write_size �����o����Size
   * @param fp           OutputFileStream
   * @return 0 : ����I�� 0�ȊO : �ُ�I��
   */
  int writeBinary(
    int64_t& i_write_size,
    FILE* fp) const;

  /** CRF����Binary�œǂݍ���
   * @param i_read_size �ǂݍ���Size
   * @param fp          InputFileStream
   * @return 0 : ����I�� 0�ȊO : �ُ�I��
   */
  int readBinary(
    int64_t& i_read_size,
    FILE* fp);

private:
  /** �w�K�f�[�^����f�������쐬
   * @param i_feature_count �쐬�����f����
   * @param learn_datas     �w�K�f�[�^
   * @return
   */
  void createFeaturelnfo(
    size_t& i_feature_count,
    const std::vector<std::vector<CRFFeatureIndex>>& learn_datas);

  /** ���z�Z�o
   * @param updates     W�X�V�l
   * @param learn_datas FeatureIndex
   * @return
   */
  void calcGradient(
    std::map<int64_t, double>& updates,
    const std::vector<CRFFeatureIndex>& learn_datas) const;

  /** �w�K���̃����v�Z
   * learn_labels�̐擪�����ɂ�BOS�AEOS�����鎖��Y�ꂸ��
   * @param alphas      ���z��
   * @param learn_datas LearnData���
   * @return
   */
  void calcAlpha(
    std::vector<std::vector<double>>& alphas,
    const std::vector<CRFFeatureIndex>& learn_datas) const;

  /** �w�K���̃����v�Z
   * learn_labels�̐擪�����ɂ�BOS�AEOS�����鎖��Y�ꂸ��
   * @param betas       ���Z���� �n��:(�f��Index)
   * @param learn_datas LearnData���
   * @return
   */
  void calcBeta(
    std::vector<std::vector<double>>& betas,
    const std::vector<CRFFeatureIndex>& learn_datas) const;

  /** �w��InputIndex�̍ő�l���擾
   * @param d_max_feature_value �擾�����ő�l
   * @param i_input_index       �Ώ�Index
   * @param datas               ���z�񂩃��z��
   * @return
   */
  void getMaxFeatureValue(
    double& d_max_feature_value,
    const size_t i_input_index,
    const std::vector<std::vector<double>>& datas) const;

  /** DoubleArray����KEY���쐬
   * ��������KEY�͎󂯎�葤�ŉ�����邱��
   * @param c_key           �쐬����KEY
   * @param i_key_length    KeyLength
   * @param i_input         InputIndex
   * @param i_label         InputLabel
   * @param i__before_label Input8eforeLabel
   * @return
   */
  void createFeatureKey(
    char*& c_key,
    int& i_key_length,
    const int64_t i_input_index,
    const int64_t i_label,
    const int64_t i_before_label) const;

  /** �f��Index���擾����
   * @param i_input_index  InputOataIndex
   * @Param i_label        InputLabel
   * @param i_before_label InputBeforeLabel
   * @return �f��Index
   */
  int64_t getFeatureIndex(
    const int64_t i_input_index,
    const int64_t i_label,
    const int64_t i_before_label) const;

  /** Member�ϐ�������������
   * @paral
   * @return
   */
  void deleteMemory();

private:
  /** Label��
   * ���ۂ�Label�l��0����̘A���l
   */
  int64_t i_label_size_;

  /** �f���l �f��Index == vectorIndex */
  std::vector<double> features_;

  /** �OCIass�Ǝ��ʑΏۂ���f��Index���擾����Data�\��
   * KEY : InputCataIndex + Input_label + InputbeforeLabel
   */
   DoubleArray feature_array_;
};


/** CRF�w�K���Ɏg�p����f�[�^�\�� */
class CRFFeatureIndex
{
public:
  /** �������Ȃ� */
  CRFFeatureIndex() : i_word_index_(0), i_label_(0) {};

  /** �������̂� */
  CRFFeatureIndex(
    const int64_t i_word_index,
    const int64_t i_label) : i_word_index_(i_word_index), i_label_(i_label) {}

  /** �R�X�g�팸�̂���virtual�͕t���Ȃ� */
  ~CRFFeatureIndex() {}

public:
  /** WordIndex */
  int64_t i_word_index_;

  /** Label */
  int64_t i_label_;
};

#endif
