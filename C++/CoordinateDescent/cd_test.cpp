#include "CoordinateDescent.h"
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>


using namespace std;

void createLearnData(vector<LearnData>& learn_datas, const vector<string>& text_datas);
void split(vector<string>& result, const string& s_str, const char* c_sepa);

int main()
{
  string s_buf;
  vector<string> text_datas;
  ifstream fin("training.txt");
  while (getline(fin, s_buf)) text_datas.push_back(s_buf);
  fin.close();

  vector<LearnData> learn_datas(text_datas.size());
  createLearnData(learn_datas, text_datas);

  auto cd = CoordinateDescent();
  cd.learn(learn_datas);

  int i_correct(0);
  for (const auto& feature : learn_datas) {
    double d_result;
    if (cd.judge(d_result, feature.feature_values_) == feature.i_class_) {
      ++i_correct;
    }
    cout << feature.i_class_ << " " << d_result << endl;
  }

  double d_accuracy = static_cast<double>(i_correct) / static_cast<double>(learn_datas.size()) * 100.0;
  cout << "Accuracy : " << d_accuracy << endl;
  cout << "           " << i_correct << " / " << learn_datas.size() << endl;

  return 0;
}

void createLearnData(
  vector<LearnData>& learn_datas,
  const vector<string>& text_datas)
{
  const char C_SEPA[] = ",";
  vector<string> splits;
  map<string, int> index_relations;
  for (size_t i = 0, i_size = learn_datas.size(); i < i_size; ++i) {
    splits.clear();
    split(splits, text_datas[i], C_SEPA);
    LearnData& data = learn_datas[i];
    data.i_class_   = (splits[0] == "1" ? CoordinateDescent::I_POSITIVE : CoordinateDescent::I_NEGATIVE);
    for (size_t n = 1, i_feature_size = splits.size() - 1; n < i_feature_size; ++n) {
      auto ite_index = index_relations.lower_bound(splits[n]);
      if (ite_index        == index_relations.end()
      ||  ite_index->first != splits[n]) {
        ite_index = index_relations.insert(ite_index, make_pair(splits[n], index_relations.size()));
      }
      data.feature_values_.push_back(make_pair(ite_index->second, 1.0));
    }
  }
}


void split(
  vector<string>& result,
  const string& s_str,
  const char* c_sepa)
{
  size_t i_find_pos = 0, i_start_pos(0);
  while ((i_find_pos = s_str.find(c_sepa, i_start_pos)) != string::npos) {
    result.push_back(s_str.substr(i_start_pos, i_find_pos - i_start_pos));
    i_start_pos = i_find_pos + strlen(c_sepa);
  }
  if (result.empty()) {
    result.push_back(s_str);
  }
}

