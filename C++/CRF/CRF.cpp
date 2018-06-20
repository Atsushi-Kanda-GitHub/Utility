#include "CRF.h"
#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <float.h>
#include <string.h>

using std::set;
using std::map;
using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::make_pair;

/* ����                        */
/* @param results ���茋��     */
/* @param inputs  ���肷���� */
void CRF::judge(
  vector<int64_t>& results,
  const vector<int64_t>& inputs) const
{
  const size_t i_input_size(inputs.size());
  vector<vector<pair<int64_t, double>>> viterbis(
    i_input_size + 1,  /* EOS���܂߂�̂�+1 */
    vector<pair<int64_t, double>>(static_cast<size_t>(i_label_size_),
           pair<int64_t, double>(0, 0.0)));

  for (size_t i = 0; i <= i_input_size; ++i) {
    if (i ==0) { /*BOS*/
      for (auto n = 0; n < i_label_size_; ++n) {
        const auto i_index = getFeatureIndex(inputs[i], n, I_CRF_BOS);
        if (i_index >= 0) {
          auto& viterbi_pair = viterbis[i][n];
          const auto d_value = features_[static_cast<size_t>(i_index)];
          if (viterbi_pair.second < d_value) {
            viterbi_pair.first  = 0;
            viterbi_pair.second = d_value;
          }
        }
      }
    }
    else if (i == i_input_size) {
      for (auto n = 0; n < i_label_size_; ++n) {
        const auto i_index = getFeatureIndex(I_CRF_EOS, I_CRF_EOS, n);
        if (i_index >= 0) {
          auto& viterbi_pair = viterbis[i][I_CRF_EOS];
          const auto d_value = viterbis[i - 1][n].second + features_[static_cast<size_t>(i_index)];
          if (viterbi_pair.second < d_value) {
            viterbi_pair.first  = n;
            viterbi_pair.second = d_value;
          }
        }
      }
    }
    else {
      for (auto n = 0; n < i_label_size_; ++n) {
        for (auto g = 0; g < i_label_size_; ++g) {
          const auto i_index = getFeatureIndex(inputs[i], n, g);
          if (i_index >= 0) {
            auto& viterbi_pair = viterbis[i][n];
            const auto d_value = viterbis[i - 1][g].second + features_[i_index];
            if (viterbi_pair.second < d_value) {
              viterbi_pair.first  = g;
              viterbi_pair.second = d_value;
            }
          }
        }
      }
    }
  }

  auto i_current(I_CRF_EOS);
  results.resize(i_input_size);
  for (auto i = i_input_size; i > 0; --i) {
    results[static_cast<size_t>(i - 1)] = viterbis[static_cast<size_t>(i)][static_cast<size_t>(i_current)].first;
    i_current = viterbis[static_cast<size_t>(i)][static_cast<size_t>(i_current)].first;
  }
}


/* ����                                */
/* @param results ���茋��             */
/* @param inputs  ���肷������t�� */
/* @return �œK���[�g�l                */
double CRF::judge(
  vector<int64_t>& results,
  const vector<pair<int64_t, const vector<int64_t>*>>& inputs) const
{
  const size_t i_input_size(inputs.size());
  vector<vector<pair<int64_t, double>>> viterbis(i_input_size + 1); /*EOS�̗̈敪��+1 */

  for (size_t i = 0; i < i_input_size; ++i) {
    viterbis[i].resize(inputs[i].second->size() + 1, pair<int64_t, double>(0, 0.0));  /* EOS�̗̈敪�� + 1 */
  }
  viterbis.back().resize(inputs.back().second->size() + 1, pair<int64_t, double>(0, 0.0));

  for (size_t i = 0; i <= i_input_size; ++i) {
    if (i == 0) { /* B0S */
      const auto& candidates = *inputs[i].second;
      for (size_t n = 0, i_candidate_size = candidates.size(); n<i_candidate_size; ++n) {
        const auto i_index = getFeatureIndex(inputs[i].first, candidates[n], I_CRF_BOS);
        if (i_index >= 0) {
          auto& viterbi_pair = viterbis[i][n];
          const auto d_value = features_[(size_t)i_index];
          if (viterbi_pair.second < d_value) {
            viterbi_pair.first  = I_CRF_BOS;
            viterbi_pair.second = d_value;
          }
        }
      }
    }
    else if (i == i_input_size) {
      const auto& candidates = *inputs[i - 1].second;
      for (size_t n = 0, i_candidate_size = candidates.size(); n < i_candidate_size; ++n) {
        const auto i_index = getFeatureIndex(I_CRF_EOS, I_CRF_EOS, candidates[n]);
        if (i_index >= 0) {
          auto& viterbi_pair = viterbis[i][I_CRF_EOS];
          const auto d_value = viterbis[i - 1][n].second + features_[(size_t)i_index];
          if (viterbi_pair.second < d_value) {
            viterbi_pair.first  = n;
            viterbi_pair.second = d_value;
          }
        }
      }
    }
    else {
      const auto& befores    = *inputs[i - 1].second;
      const auto& candidates = *inputs[i].second;
      for (size_t n = 0, i_candidate_size = candidates.size(); n < i_candidate_size; ++n) {
        for (size_t g = 0, i_before_size = befores.size(); g < i_before_size; ++g) {
          const auto i_index = getFeatureIndex(inputs[i].first, candidates[n], befores[g]);
          if (i_index >= 0) {
            auto& viterbi_pair = viterbis[i][n];
            const auto d_value = viterbis[i - 1][g].second + features_[(size_t)i_index];
            if (viterbi_pair.second < d_value) {
              viterbi_pair.first  = g;
              viterbi_pair.second = d_value;
            }
          }
        }
      }
    }
  }

  double d_value(0.0);
  auto i_current(I_CRF_EOS);
  results.resize(i_input_size);
  for (int64_t i = i_input_size; i > 0; --i) {
    const size_t i_cast_index(static_cast<size_t>(i));
    const size_t i_cast_current(static_cast<size_t>(i_current));
    const size_t i_cast_viterbi_first(static_cast<size_t>(viterbis[i_cast_index][i_cast_current].first));
    results[i_cast_index - 1] = (*inputs[i_cast_index - 1].second)[i_cast_viterbi_first];
    i_current = viterbis[i_cast_index][i_cast_current].first;
    d_value += viterbis[i_cast_index][i_cast_current].second;
  }
  return d_value;
}


/* �w�K                                     */
/* @param i_label_size     Label��          */
/* @param learn_datas      �w�K�f�[�^       */
/* @param i_roop           �w�K�J��Ԃ��� */
/* @param d_eta            �w�K��           */
/* @baram d_regularization ������           */
/* @return 0 : ����I�� 0�ȊO : �ُ�I��    */
int CRF::learn(
  const int64_t i_label_size,
  const vector<vector<CRFFeatureIndex>>& learn_datas,
  const int i_roop,
  const double d_eta,
  const double d_regularization)
{
  deleteMemory();
  i_label_size_ = i_label_size; /*Label��*/
  size_t i_feature_count(1);
  createFeaturelnfo(i_feature_count, learn_datas); /*�f���쐬*/
  features_.resize(i_feature_count - 1, 0.0); /*�f���l i_feature_count��1����n�߂Ă���̂�-1 */

  map<int64_t, double> updates;
  for (int i = 0; i < i_roop; ++i) {
    cout << i << endl;
    for (const auto& learn_data : learn_datas) {
      calcGradient(updates, learn_data);

      for (auto& update : updates) {
        features_[(size_t)update.first] += (d_eta* (update.second - (d_regularization * features_[(size_t)update.first])));
      }
      updates.clear();
    }
  }

  return 0;
}


/* �w�K�f�[�^����f�������쐬          */
/* @param i_feature_count �쐬�����f���� */
/* @param learn_datas     �w�K�f�[�^     */
void CRF::createFeaturelnfo(
  size_t& i_feature_count,
  const vector<vector<CRFFeatureIndex>>& learn_datas)
{
  ByteArrayDatas byte_array_datas;
  for (const auto& learn_data : learn_datas) {
    const auto& datas = learn_data;
    for (size_t i = 1, i_size = datas.size(); i < i_size; ++i) {
      char* c_key;
      int i_key_length;
      createFeatureKey(c_key, i_key_length, datas[i].i_word_index_, datas[i].i_label_, datas[i - 1].i_label_);
      byte_array_datas.addData(c_key, i_key_length, i_feature_count++);
      delete[] c_key;
    }
  }
  feature_array_.createDoubleArray(byte_array_datas);
}


/* ���z�Z�o                        */
/* @param updates     W�X�V�l      */
/* @param learn_datas FeatureIndex */
void CRF::calcGradient(
  map<int64_t, double>& updates,
  const vector<CRFFeatureIndex>& learn_datas) const
{
  vector<vector<double>> alphas, betas;
  calcAlpha(alphas, learn_datas); /*���Z�o*/
  calcBeta(betas, learn_datas);   /*���Z�o*/

  const size_t i_learn_data_size(learn_datas.size());
  for (size_t i = 1, i_size = i_learn_data_size; i < i_size; ++i) {
    const auto i_feature = getFeatureIndex(learn_datas[i].i_word_index_, learn_datas[i].i_label_, learn_datas[i-1].i_label_);
    auto update = updates.lower_bound(i_feature);
    if (update == updates.end()
    ||  update->first != i_feature) {
      update = updates.insert(update, make_pair(i_feature, 0.0));
    }
    ++update->second;
  }

  double d_max_alpha(std::numeric_limits<double>::min()); /* ��~(N,y(max)N) */
  for (size_t i = 0; i < i_learn_data_size; ++i) {
    if (d_max_alpha < alphas[i][I_CRF_EOS]) {
      d_max_alpha = alphas[i][I_CRF_EOS];
    }
  }

  double d_zeta(0.0); /* Zn */
  for (size_t i = 0; i < i_learn_data_size; ++i) {
    d_zeta += exp(alphas[i][I_CRF_EOS] - d_max_alpha);
  }
  d_zeta = log(d_zeta) + d_max_alpha;

  for (size_t i = 1; i < i_learn_data_size; ++i) {
    const auto& current_feature = learn_datas[i];
    const auto& before_feature  = learn_datas[i-1];
    const auto i_feature = getFeatureIndex(current_feature.i_word_index_, current_feature.i_label_, before_feature.i_label_);
    const auto d_psi     = exp(features_[static_cast<size_t>(i_feature)]); /* ��(yt, yt-1) */
    const auto d_gran    = d_psi * (exp(betas[i][static_cast<size_t>(current_feature.i_label_)]
                           + alphas[i-1][static_cast<size_t>(before_feature.i_label_)] - d_zeta));
                          /* P(yt, yt-1 | xti) = ��(yt, yt-1) exp(��~(t, yt)+��~(t-1, yt-1)-ZN) */
    auto update = updates.find(i_feature);
    update->second += d_gran;
  }
}


/* �w�K���̃����v�Z                                       */
/* learn_labels�̐擪�����ɂ�BOS�AEOS�����鎖��Y�ꂸ�� */
/* @param alphas      ���z��                              */
/* @param learn_datas LearnData���                       */
void CRF::calcAlpha(
  vector<vector<double>>& alphas,
  const vector<CRFFeatureIndex>& learn_datas) const
{
  const size_t i_label_size(i_label_size_);
  const size_t i_learn_size(learn_datas.size());
  alphas.resize(i_learn_size, vector<double>(i_label_size, 0.0));
  alphas[0][0] = 1.0; /* ��` */
  for (size_t i = 1; i < i_learn_size; ++i) { /* CurrentInputIndex�擪��BOS�Ȃ̂Ŕ�΂� */
    double d_max_alpha;
    getMaxFeatureValue(d_max_alpha, i-1, alphas); /* ��~(t-1, yt-1(max)) */
    for (size_t n = 0; n < i_label_size; ++n) { /* CurrentIndex */
      double d_alpha(0.0);
      for (size_t g = 0; g < i_label_size; ++g) { /* BeforeIndex */
        double d_exp = exp(alphas[i-1][g] - d_max_alpha); /* exp(��~(t-1, yt-1) -��~(t-1, y(max)t-1)) */
        double d_phi(1.0);
        const int64_t i_feature = getFeatureIndex(learn_datas[i].i_word_index_, n, g);
        if (i_feature >= 0) {
          d_phi = exp(features_[static_cast<size_t>(i_feature)]);
        }
        d_alpha += (d_exp * d_phi);
      }
      alphas[i][n] = log(d_alpha) + d_max_alpha;
    }
  }
}

/* �w�K���̃����v�Z                                       */
/* learn_labels�̐擪�����ɂ�BOS�AEOS�����鎖��Y�ꂸ�� */
/* @param betas       ���Z���� �n��:(�f��Index)           */
/* @param learn_datas LearnData���                       */
void CRF::calcBeta(
  vector<vector<double>>& betas,
  const vector<CRFFeatureIndex>& learn_datas) const
{
  const size_t i_label_size(i_label_size_);
  const size_t i_learn_size(learn_datas.size());
  betas.resize(i_learn_size, vector<double>(i_label_size, 0.0));
  betas[i_learn_size - 1][1] = 1.0; /* ��` */
  for (size_t i = i_learn_size - 2; i >= 0 && i<i_learn_size; --i) { /* CurrentInputIndex������EOS�Ȃ̂Ŕ�΂� */
    double d_max_beta;
    getMaxFeatureValue(d_max_beta, i+1, betas) ; /*��~(t+1, y(max)t+1) */
    for (size_t n = 0; n < i_label_size; ++n) { /* CurrentIndex */
      double d_beta(0.0);
      for (size_t g = 0; g< i_label_size; ++g) { /* BeforeIndex */
        const double d_exp = exp(betas[i+1][g] - d_max_beta); /*exp(��~(t+1, yt+1) -��~(t+1, y(max)t+1)) */
        double d_phi(1.0);
        const int64_t i_feature = getFeatureIndex(learn_datas[i].i_word_index_, n, g);
        if (i_feature >= 0) {
          d_phi = exp(features_[static_cast<size_t>(i_feature)]);
        }
        d_beta += (d_exp * d_phi);
      }
      betas[i][n] = log(d_beta) + d_max_beta;
    }
  }
}


/* �w��InputIndex�̍ő�l���擾              */
/* @param d_max_feature_value �擾�����ő�l */
/* @param i_input_index       �Ώ�Index      */
/* @param datas               ���z�񂩃��z�� */
void CRF::getMaxFeatureValue(
  double& d_max_feature_value,
  const size_t i_input_index,
  const vector<vector<double>>& datas) const
{
  d_max_feature_value = std::numeric_limits<double>::min();
  const auto& values = datas[i_input_index];
  for (const auto& value : values) {
    if (d_max_feature_value < value) {
      d_max_feature_value = value;
    }
  }
}


/* DoubleArray����KEY���쐬                */
/* ��������KEY�͎󂯎�葤�ŉ�����邱��   */
/* @param c_key           �쐬����KEY      */
/* @param i_key_length    KeyLength        */
/* @param i_input         InputIndex       */
/* @param i_label         InputLabel       */
/* @param i__before_label Input8eforeLabel */
void CRF::createFeatureKey(
  char*& c_key,
  int& i_key_length,
  const int64_t i_input_index,
  const int64_t i_label,
  const int64_t i_before_label) const
{
  i_key_length = sizeof(i_input_index) + sizeof(i_label) + sizeof(i_before_label) + 1;  /* +1�͏I�[�L���� */
  c_key = new char[i_key_length];

  int i_current(0);
  memcpy(c_key, &i_input_index, sizeof(i_input_index));
  i_current += sizeof(i_input_index);
  memcpy(&c_key[i_current], &i_label, sizeof(i_label));
  i_current += sizeof(i_label);
  memcpy(&c_key[i_current], &i_before_label, sizeof(i_before_label));
  i_current += sizeof(i_before_label);
  c_key[i_current] = (char)NULL;
}


/* �f��Index���擾����                    */
/* @param i_input_index  InputOataIndex   */
/* @Param i_label        InputLabel       */
/* @param i_before_label InputBeforeLabel */
/* @return �f��Index                      */
int64_t CRF::getFeatureIndex(
  const int64_t i_input_index,
  const int64_t i_label,
  const int64_t i_before_label) const
{
  char* c_key;
  int i_key_length;
  createFeatureKey(c_key, i_key_length, i_input_index, i_label, i_before_label);

  int64_t i_result = feature_array_.search(c_key, i_key_length) -1;
  delete[] c_key;
  return i_result;
}


/* Member�ϐ������������� */
void CRF::deleteMemory()
{
  i_label_size_ = 0;
  features_.clear();
}


/* CRF����Binary�ŏ����o��             */
/* @param i_write_size �����o����Size    */
/* @param fp           OutputFileStream  */
/* @return 0 : ����I�� 0�ȊO : �ُ�I�� */
int CRF::writeBinary(
  int64_t& i_write_size,
  FILE* fp) const
{
  try {
    fwrite(&i_label_size_, sizeof(i_label_size_), 1, fp);
    const uint64_t i_feature_size = features_.size();
    fwrite(&i_feature_size, sizeof(i_feature_size), 1, fp);
    i_write_size += sizeof(i_label_size_);
    i_write_size += sizeof(i_feature_size);
    if (i_feature_size) {
      fwrite(&features_[0], sizeof(features_.front()), i_feature_size, fp);
      i_write_size += (sizeof(features_.front()) * i_feature_size);
    }
  }
  catch (...) {
    return I_CRF_IO_ERROR;
  }

  return I_CRF_NORMAL;
}

/* CRF����Binary�œǂݍ���             */
/* @param i_read_size �ǂݍ���Size       */
/* @param fp          InputFileStream    */
/* @return 0 : ����I�� 0�ȊO : �ُ�I�� */
int CRF::readBinary(
  int64_t& i_read_size,
  FILE* fp)
{
  deleteMemory();

  try {
    uint64_t i_feature_size;
    if ((1 != fread(&i_label_size_,  sizeof(i_label_size_),  1, fp))
    ||  (1 != fread(&i_feature_size, sizeof(i_feature_size), 1, fp)))
      throw;

    i_read_size += sizeof(i_label_size_);
    i_read_size += sizeof(i_feature_size);
    if (i_feature_size) {
      features_.resize(i_feature_size);
      if (i_feature_size != fread(&features_[0], sizeof(features_[0]), i_feature_size, fp))
        throw;

      i_read_size += (sizeof(features_.front()) * i_feature_size);
    }
    feature_array_.readBinary(i_read_size, fp);
  }
  catch (...) {
    return I_CRF_IO_ERROR;
  }

  return I_CRF_NORMAL;
}

