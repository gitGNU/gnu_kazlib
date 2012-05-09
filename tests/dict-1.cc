#include <string>
#include <iostream>
#include "dict.h"

using std::string;
using std::cout;
using std::endl;

using kazlib::dnode;
using kazlib::dict;

using kazlib::dnode_is_member;
using kazlib::key_is_member;
using kazlib::dupes_allowed;
using kazlib::compare_with_function;

class person {
public:
  int id;
  string first_name;
  string last_name;
public:
  dnode id_dn;
  dnode first_name_dn;
  dnode last_name_dn;

  person(int id, const string &fn, const string &ln)
  : id(id), first_name(fn), last_name(ln)
  {
  }

  void print()
  {
    cout << id << ": " << first_name << " " << last_name << endl;
  }
};

int reverse_compare(const string &left, const string &right)
{
  return right.compare(left);
}

int main()
{
  person p[] = {
    person(1, "Mary", "Smith"),
    person(2, "John", "Smith"),
    person(3, "Daisuke", "Takayama"),
    person(4, "Zheng", "Shui-Yun"),
    person(5, "Jaswinder", "Bhatta"),
    person(6, "Jarek", "Kozlowski")
  };

  dict<dnode_is_member<person, &person::id_dn>,
       key_is_member<person, int, &person::id> > by_id;

  dict<dnode_is_member<person, &person::first_name_dn>,
       key_is_member<person, string, &person::first_name>,
       compare_with_function<string, reverse_compare>,
       dupes_allowed> by_first_name;

  dict<dnode_is_member<person, &person::last_name_dn>,
       key_is_member<person, string, &person::last_name>,
       dupes_allowed> by_last_name;

  for (size_t i = 0; i < sizeof p / sizeof p[0]; i++) {
    by_id.insert(p[i]);
    by_first_name.insert(p[i]);
    by_last_name.insert(p[i]);
  }

  person *pi;

  cout << "person records ordered by id:" << endl;

  for (pi = by_id.first(); pi != 0; pi = by_id.next(pi))
    pi->print();

  cout << "person records ordered by last name:" << endl;

  for (pi = by_last_name.first(); pi != 0; pi = by_last_name.next(pi))
    pi->print();

  cout << "person records ordered by first name, descending:" << endl;

  for (pi = by_first_name.first(); pi != 0; pi = by_first_name.next(pi))
    pi->print();

  return 0;
}

