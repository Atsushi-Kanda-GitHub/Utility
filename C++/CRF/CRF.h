#ifndef CRF_H
#define CRF_H

/**
* Conditional Random Fields
*
* @brief  機械学習CRF
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
  /** 何もしない */
  CRF() {}
  
  /** 何もしない */
  virtual ~CRF() {}

  /** 学習
   * @param i_label_size     Label数
   * @param learn_datas      学習データ
   * @param i_roop           学習繰り返し回数
   * @param d_eta            学習率
   * @baram d_regularization 正則化
   * @return 0 : 正常終了 0以外 : 異常終了
   */
  int learn(
    const int64_t i_label_size,
    const std::vector<std::vector<CRFFeatureIndex>>& learn_datas,
    const int i_roop = 5,
    const double d_eta = 1.0,
    const double d_regularization = 10);

  /** 判定
   * @param results 判定結果
   * @param inputs  判定する情報
   * @return
   */
  void judge(
    std::vector<int64_t>& results,
    const std::vector<int64_t>& inputs) const;

  /** 判定
   * @param results 判定結果
   * @param inputs  判定する情報候補付き
   * @return 最適ルート値
   */
  double judge(
    std::vector<int64_t>& results,
    const std::vector<std::pair<int64_t, const std::vector<int64_t>*>>& inputs) const;

  /** CRF情報をBinaryで書き出し
   * @param i_write_size 書き出したSize
   * @param fp           OutputFileStream
   * @return 0 : 正常終了 0以外 : 異常終了
   */
  int writeBinary(
    int64_t& i_write_size,
    FILE* fp) const;

  /** CRF情報をBinaryで読み込み
   * @param i_read_size 読み込みSize
   * @param fp          InputFileStream
   * @return 0 : 正常終了 0以外 : 異常終了
   */
  int readBinary(
    int64_t& i_read_size,
    FILE* fp);

private:
  /** 学習データから素性情報を作成
   * @param i_feature_count 作成した素性数
   * @param learn_datas     学習データ
   * @return
   */
  void createFeaturelnfo(
    size_t& i_feature_count,
    const std::vector<std::vector<CRFFeatureIndex>>& learn_datas);

  /** 勾配算出
   * @param updates     W更新値
   * @param learn_datas FeatureIndex
   * @return
   */
  void calcGradient(
    std::map<int64_t, double>& updates,
    const std::vector<CRFFeatureIndex>& learn_datas) const;

  /** 学習時のαを計算
   * learn_labelsの先頭末尾にはBOS、EOSを入れる事を忘れずに
   * @param alphas      α配列
   * @param learn_datas LearnData情報
   * @return
   */
  void calcAlpha(
    std::vector<std::vector<double>>& alphas,
    const std::vector<CRFFeatureIndex>& learn_datas) const;

  /** 学習時のβを計算
   * learn_labelsの先頭末尾にはBOS、EOSを入れる事を忘れずに
   * @param betas       演算結果 系列:(素性Index)
   * @param learn_datas LearnData情報
   * @return
   */
  void calcBeta(
    std::vector<std::vector<double>>& betas,
    const std::vector<CRFFeatureIndex>& learn_datas) const;

  /** 指定InputIndexの最大値を取得
   * @param d_max_feature_value 取得した最大値
   * @param i_input_index       対象Index
   * @param datas               α配列かβ配列
   * @return
   */
  void getMaxFeatureValue(
    double& d_max_feature_value,
    const size_t i_input_index,
    const std::vector<std::vector<double>>& datas) const;

  /** DoubleArray検索KEYを作成
   * 生成したKEYは受け取り側で解放すること
   * @param c_key           作成したKEY
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

  /** 素性Indexを取得する
   * @param i_input_index  InputOataIndex
   * @Param i_label        InputLabel
   * @param i_before_label InputBeforeLabel
   * @return 素性Index
   */
  int64_t getFeatureIndex(
    const int64_t i_input_index,
    const int64_t i_label,
    const int64_t i_before_label) const;

  /** Member変数を初期化する
   * @paral
   * @return
   */
  void deleteMemory();

private:
  /** Label数
   * 実際のLabel値は0からの連続値
   */
  int64_t i_label_size_;

  /** 素性値 素性Index == vectorIndex */
  std::vector<double> features_;

  /** 前CIassと識別対象から素性Indexを取得するData構造
   * KEY : InputCataIndex + Input_label + InputbeforeLabel
   */
   DoubleArray feature_array_;
};


/** CRF学習時に使用するデータ構造 */
class CRFFeatureIndex
{
public:
  /** 何もしない */
  CRFFeatureIndex() : i_word_index_(0), i_label_(0) {};

  /** 初期化のみ */
  CRFFeatureIndex(
    const int64_t i_word_index,
    const int64_t i_label) : i_word_index_(i_word_index), i_label_(i_label) {}

  /** コスト削減のためvirtualは付けない */
  ~CRFFeatureIndex() {}

public:
  /** WordIndex */
  int64_t i_word_index_;

  /** Label */
  int64_t i_label_;
};

#endif
