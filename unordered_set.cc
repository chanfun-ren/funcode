#include <iostream>
#include <list>
#include <vector>

// template <typename K, typename V> class Set {
// private:
//   Hashmap<K, V> map;
// };

template <typename K, typename V>
struct Node {
  K key;
  V value;
  Node(K k, V v) : key(k), value(v) {}
  ~Node() = default;
};

template <typename K, typename V>
class HashMap {
  using Entry = Node<K, V>;

 public:
  // here for test functions
  void print() { std::cout << load_factor_ << ", " << initial_cap_ << "\n"; }

 public:
  HashMap() { buckets_.resize(8); }

  void Set(K k, V v) {
    // TODO
    // if k exist -> update the k, v
    // if k no exist -> Add(k, v)
  }

  void Delete(K k, V v) {
    // TODO
    // if k exist -> delete the k, v? Or marke it deleted?
    // if k no exist -> return msg.
  }

 private:
  void Add(K k, V v) {
    // TODO
    // 0. if no sufficient capacity, expand the cap. then:
    // 1. get k hashcode.
    // 2. caculate the index -> i.
    // 3. in bucktes_[i]
  }
  void Existed(K k) {}
  void Expand() {
    // TODO
    // 0. allocate a new array (new_size = old_size * 2)
    // 1. move the elements into the new array.(rehash)
  }

 private:
  // std::vector <std::list<Entry>> buckets_;
  std::vector<std::list<Node<K, V>>> buckets_;
  float load_factor_ = 0.75f;
  int initial_cap_ = 8;
};

int main() {
  Node<int, int> node(1, 2);
  HashMap<int, int>().print();
  return 0;
}