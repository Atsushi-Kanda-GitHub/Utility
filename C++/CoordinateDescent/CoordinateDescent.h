#ifndef COORDINATEDESCENT_H
#define COORDINATEDESCENT_H

/**
* CoordinateDescent
*
* @brief  CoordinateDescent
* @file   CoordinateDescent.h
* @author dev.atsushi.kanda@gmail.com
* @date   2018/06/12
*/

#include <vector>
#include <utility>


/* first : 素性Index second : 値 */
using FeatureValues = std::vector<std::pair<int, double>>;

class LearnData;

class CoordinateDescent
{
public:
  static constexpr int    I_POSITIVE            =  1;
  static constexpr int    I_NEGATIVE            = -1;
  static constexpr double D_DEFAULT_SOFT_MARGIN =  1.0;

public:
  /** コンストラクタ */
  CoordinateDescent() {}

  /** デストラクタ */
  ~CoordinateDescent() {};

  /** CoordinateDescentによる学習
  * @param learn_data    学習データ
  * @param d_soft_margin ソフトマージン
  * @return
  */
  void learn(
    const std::vector<LearnData>& learn_data,
    const double d_soft_margin = D_DEFAULT_SOFT_MARGIN);

  /** 判別
  * @param d_result 算出結果
  * @param inputs   入力情報
  * @return I_POSITIVE_EXAMPLE : 正例  I_NEGATIVE_EXAMPLE : 負例
  */
  int judge(
    double& d_result,
    const FeatureValues& inputs) const;

  /** CoordinateDescentデータをファイルから読み込み
  * @param c_file_path ファイルパス
  * @return false : 正常終了  true : 異常
  */
  bool readCoordinateDescentData(
    const char* c_file_path);

  /** CoordinateDescentデータをファイル出力
  * @param c_file_path ファイルパス
  * @return false : 正常終了  true : 異常
  */
  bool writeCoordinateDescentData(
    const char* c_file_path) const;

  /** CoordinateDescentデータを設定
  * @param i_feature_size 素性数
  * @param feature_values 素性値
  * @return
  */
  void setCoordinateDescentData(
    const std::vector<double>& feature_values);

  /** CoordinateDescentデータを取得
  * @return 素性値
  */
  const std::vector<double>& getCoordinateDescentData() const;

  /** 指定の素性Indexから素性値を取得する
  * @param d_feature_value 素性値
  * @param i_feature_index 素性Index
  * @return
  */
  void getFeatureValue(
    double& d_featrue_value,
    const int i_feature_index) const;

private:
  /** 最大素性Indexを検索
  * @param i_max_feature_index 最大素性Index
  * @param learn_data          学習データ
  * @return
  */
  void getMaxFeatureIndex(
    int& i_feature_size,
    const std::vector<LearnData>& learn_data) const;

  /** CoordinateDescent計算
  * @param d_soft_margin ソフトマージン
  * @param values     全値の2乗加算値
  * @param learn_data 学習データ
  * @return
  */
  void coordinateDescentSolve(
    const double& d_soft_margin,
    const std::vector<double>& values,
    const std::vector<LearnData>& learn_data);

  /** CoordinateDescent更新
  * @param indexes      Class配列
  * @param alphas       更新時に使用する領域
  * @param d_min_value  勾配最小値
  * @param d_max_value  勾配最大値
  * @param i_update     更新するIndex
  * @param i_last_index 更新範囲の最終Index
  * @param d_min_old    直前の勾配最小値
  * @param d_max_old    直前の勾配最大値
  * @param values       全値の2乗加算値
  * @param learn_data   学習データ
  * @return
  */
  bool coordinateUpdate(
    std::vector<int>& indexes,
    std::vector<double>& alphas,
    double& d_min_value,
    double& d_max_value,
    const int i_update,
    const int i_last_index,
    const double& d_min_old,
    const double& d_max_old,
    const double& d_soft_margin,
    const std::vector<double>& values,
    const std::vector<LearnData>& learn_data);

  /** 引数の配列の内容をランダムに入れ替える
  * @param indexesc     配列データ
  * @param i_learn_size 学習コーパス数
  * @return
  */
  void shuffle(
    std::vector<int>& indexes,
    const int i_learn_size) const;

private:
  /** 素性値 */
  std::vector<double> feature_values_;
};


/** 行列を構成するデータ */
class LearnData
{
public:
  /** コンストラクタ */
  LearnData() : i_class_(0) {}

  /** デストラクタ */
  ~LearnData() {};

  /** 正例負例 */
  int i_class_;

  /** first : 素性Index second : 値 */
  FeatureValues feature_values_;
};

#endif
