#include "MyApi.hpp"

#include <iostream>
#include <vector>

#include <boost/unordered_set.hpp>

void ServerApi::helloWorld() {
  boost::unordered_set<int> set;
  set.insert(1);
  set.insert(2);

  std::cout << "Hello World!" << std::endl;
  std::cout << set.size() << " Elements" << std::endl;
  std::vector<int> v;
  v.push_back(1);
}
