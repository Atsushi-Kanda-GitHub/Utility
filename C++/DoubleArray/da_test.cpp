#include "DoubleArray.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>
#include <unordered_map>
#include <random>
#include <chrono>

//#include <crtdbg.h>
//#define malloc(X) _malloc_dbg(X,_NORMAL_BLOCK,__FILE__,__LINE__) 
//#define new ::new(_NORMAL_BLOCK, __FILE__, __LINE__)

using namespace std;

void calcDA(     const vector<pair<string, int>>& addresses, const int i_count);
void calcMap(    const vector<pair<string, int>>& addresses, const int i_count);
void calcHashMap(const vector<pair<string, int>>& addresses, const int i_count);

int main()
{
  //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);

  string s_file_path("da_test_data.txt");
  //cout << "FilePath : ";
  //cin >> s_file_path;

  const int i_count = 50;

  string s_buf;
  int i_index(0);
  vector<pair<string, int>> addresses;
  ifstream fin(s_file_path.c_str());
  while (getline(fin, s_buf)) {
    addresses.push_back(make_pair(s_buf, ++i_index));
  }
  fin.close();

  random_device rd;
  mt19937 mt(rd());
  shuffle(addresses.begin(), addresses.end(), mt);
  
  calcMap(    addresses, i_count);
  calcHashMap(addresses, i_count);
  calcDA(     addresses, i_count);

  return 0;
}

void calcDA(const vector<pair<string, int>>& addresses, const int i_count)
{
  cout << "-------- DoubleArray --------" << endl;

  common_type_t<chrono::system_clock::duration, chrono::system_clock::duration> all_create_time(0), all_search_time(0);

  for (auto i = 0; i < i_count; ++i) {
    auto create_start = chrono::system_clock::now();

    bool b_effective = true;
    ByteArrayDatas byte_datas;
    for (auto address : addresses) {
      if (b_effective) {
        byte_datas.addData(address.first.c_str(), address.first.length(), address.second);
      }
      b_effective = !b_effective;
    }

    DoubleArray da;
    da.createDoubleArray(byte_datas);
    all_create_time += chrono::system_clock::now() - create_start;

    b_effective = true;
    auto search_start = chrono::system_clock::now();
    for (auto address : addresses) {
      int64_t result = da.search(address.first.c_str(), address.first.length());
      if ((b_effective && result != address.second)
      ||  (b_effective == false && result != 0)) {
        std::cout << "NG" << std::endl;
      }
      b_effective = !b_effective;
    }
 
    all_search_time += chrono::system_clock::now() - search_start;
  }

  all_create_time /= i_count;
  all_search_time /= i_count;
  cout << "create average time = " << chrono::duration_cast<std::chrono::milliseconds>(all_create_time).count() << " msec." << endl;
  cout << "search average time = " << chrono::duration_cast<std::chrono::milliseconds>(all_search_time).count() << " msec." << endl << endl;
}

void calcMap(const vector<pair<string, int>>& addresses, const int i_count)
{
  cout << "-------- map --------" << endl;

  common_type_t<chrono::system_clock::duration, chrono::system_clock::duration> all_create_time(0), all_search_time(0);

  for (auto i = 0; i < i_count; ++i) {
    auto create_start = chrono::system_clock::now();

    bool b_effective = true;
    map<string, int> address_map;
    for (auto address : addresses) {
      if (b_effective) {
        address_map.insert(address);
      }
      b_effective = !b_effective;
    }

    all_create_time += chrono::system_clock::now() - create_start;

    b_effective = true;
    auto search_start = chrono::system_clock::now();
    for (auto address : addresses) {
      auto result = address_map.find(address.first);
      if ((b_effective          && (result->second != address.second))
      ||  (b_effective == false && (result         != address_map.end()))) {
        std::cout << "NG" << std::endl;
      }
      b_effective = !b_effective;
    }

    all_search_time += chrono::system_clock::now() - search_start;
  }

  all_create_time /= i_count;
  all_search_time /= i_count;
  cout << "create average time = " << chrono::duration_cast<std::chrono::milliseconds>(all_create_time).count() << " msec." << endl;
  cout << "search average time = " << chrono::duration_cast<std::chrono::milliseconds>(all_search_time).count() << " msec." << endl;
}

void calcHashMap(const vector<pair<string, int>>& addresses, const int i_count)
{
  cout << "-------- unordered_map --------" << endl;
  common_type_t<chrono::system_clock::duration, chrono::system_clock::duration> all_create_time(0), all_search_time(0);

  for (auto i = 0; i < i_count; ++i) {
    auto create_start = chrono::system_clock::now();

    bool b_effective = true;
    unordered_map<string, int> address_map;
    for (auto address : addresses) {
      if (b_effective) {
        address_map.insert(address);
      }
      b_effective = !b_effective;
    }

    all_create_time += chrono::system_clock::now() - create_start;

    b_effective = true;
    auto search_start = chrono::system_clock::now();
    for (auto address : addresses) {
      auto result = address_map.find(address.first);
      if ((b_effective          && (result->second != address.second))
      ||  (b_effective == false && (result != address_map.end()))) {
        std::cout << "NG" << std::endl;
      }
      b_effective = !b_effective;
    }

    all_search_time += chrono::system_clock::now() - search_start;
  }

  all_create_time /= i_count;
  all_search_time /= i_count;
  cout << "create average time = " << chrono::duration_cast<std::chrono::milliseconds>(all_create_time).count() << " msec." << endl;
  cout << "search average time = " << chrono::duration_cast<std::chrono::milliseconds>(all_search_time).count() << " msec." << endl;
}
