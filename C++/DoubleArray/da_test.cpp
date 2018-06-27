#include "DoubleArray.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>
#include <unordered_map>
#include <random>
#include <chrono>


using namespace std;

void calcDA(     const vector<pair<string, int>>& test_datas, const int i_count);
void calcMap(    const vector<pair<string, int>>& test_datas, const int i_count);
void calcHashMap(const vector<pair<string, int>>& test_datas, const int i_count);

int main()
{
  string s_file_path("da_test_data.txt");
  //cout << "FilePath : ";
  //cin >> s_file_path;

  const int i_count = 50;

  string s_buf;
  int i_index(0);
  vector<pair<string, int>> test_datas;
  ifstream fin(s_file_path.c_str());
  while (getline(fin, s_buf)) {
    test_datas.push_back(make_pair(s_buf, ++i_index));
  }
  fin.close();

  random_device rd;
  mt19937 mt(rd());
  shuffle(test_datas.begin(), test_datas.end(), mt);
  
  calcMap(    test_datas, i_count);
  calcHashMap(test_datas, i_count);
  calcDA(     test_datas, i_count);

  return 0;
}

void calcDA(const vector<pair<string, int>>& test_datas, const int i_count)
{
  cout << "-------- DoubleArray --------" << endl;

  common_type_t<chrono::system_clock::duration, chrono::system_clock::duration> all_create_time(0), all_search_time(0);

  for (auto i = 0; i < i_count; ++i) {
    auto create_start = chrono::system_clock::now();

    bool b_effective = true;
    ByteArrayDatas byte_datas;
    for (const auto& test_data : test_datas) {
      if (b_effective) {
        byte_datas.addData(test_data.first.c_str(), test_data.first.length(), test_data.second);
      }
      b_effective = !b_effective;
    }

    DoubleArray da;
    da.createDoubleArray(byte_datas);
    all_create_time += chrono::system_clock::now() - create_start;

    b_effective = true;
    auto search_start = chrono::system_clock::now();
    for (const auto& test_data : test_datas) {
      int64_t result = da.search(test_data.first.c_str(), test_data.first.length());
      if ((b_effective && result != test_data.second)
      ||  (b_effective == false && result != 0)) {
        std::cout << "NG" << std::endl;
      }
      b_effective = !b_effective;
    }
 
    all_search_time += chrono::system_clock::now() - search_start;
  }

  all_create_time /= i_count;
  all_search_time /= i_count;
  cout << "create average time = " << chrono::duration_cast<chrono::milliseconds>(all_create_time).count() << " msec." << endl;
  cout << "search average time = " << chrono::duration_cast<chrono::milliseconds>(all_search_time).count() << " msec." << endl;
}

void calcMap(const vector<pair<string, int>>& test_datas, const int i_count)
{
  cout << "-------- map --------" << endl;

  common_type_t<chrono::system_clock::duration, chrono::system_clock::duration> all_create_time(0), all_search_time(0);

  for (auto i = 0; i < i_count; ++i) {
    auto create_start = chrono::system_clock::now();

    bool b_effective = true;
    map<string, int> data_map;
    for (const auto& test_data : test_datas) {
      if (b_effective) {
        data_map.insert(test_data);
      }
      b_effective = !b_effective;
    }

    all_create_time += chrono::system_clock::now() - create_start;

    b_effective = true;
    auto search_start = chrono::system_clock::now();
    for (const auto& test_data : test_datas) {
      auto result = data_map.find(test_data.first);
      if ((b_effective          && (result->second != test_data.second))
      ||  (b_effective == false && (result         != data_map.end()))) {
        std::cout << "NG" << std::endl;
      }
      b_effective = !b_effective;
    }

    all_search_time += chrono::system_clock::now() - search_start;
  }

  all_create_time /= i_count;
  all_search_time /= i_count;
  cout << "create average time = " << chrono::duration_cast<chrono::milliseconds>(all_create_time).count() << " msec." << endl;
  cout << "search average time = " << chrono::duration_cast<chrono::milliseconds>(all_search_time).count() << " msec." << endl;
}

void calcHashMap(const vector<pair<string, int>>& test_datas, const int i_count)
{
  cout << "-------- unordered_map --------" << endl;
  common_type_t<chrono::system_clock::duration, chrono::system_clock::duration> all_create_time(0), all_search_time(0);

  for (auto i = 0; i < i_count; ++i) {
    auto create_start = chrono::system_clock::now();

    bool b_effective = true;
    unordered_map<string, int> data_map;
    for (auto test_data : test_datas) {
      if (b_effective) {
        data_map.insert(test_data);
      }
      b_effective = !b_effective;
    }

    all_create_time += chrono::system_clock::now() - create_start;

    b_effective = true;
    auto search_start = chrono::system_clock::now();
    for (auto test_data : test_datas) {
      auto result = data_map.find(test_data.first);
      if ((b_effective          && (result->second != test_data.second))
      ||  (b_effective == false && (result != data_map.end()))) {
        std::cout << "NG" << std::endl;
      }
      b_effective = !b_effective;
    }

    all_search_time += chrono::system_clock::now() - search_start;
  }

  all_create_time /= i_count;
  all_search_time /= i_count;
  cout << "create average time = " << chrono::duration_cast<chrono::milliseconds>(all_create_time).count() << " msec." << endl;
  cout << "search average time = " << chrono::duration_cast<chrono::milliseconds>(all_search_time).count() << " msec." << endl;
}
