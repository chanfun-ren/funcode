#include <iostream>
#include <vector>

using std::vector;

struct ListNode {
  int val;
  ListNode* next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode* next) : val(x), next(next) {}
};

ListNode* gen_list(const vector<int>& v) {
  ListNode dummy;
  ListNode* curr = &dummy;
  for (size_t i = 0; i < v.size(); i++) {
    curr->next = new ListNode(v[i]);
    curr = curr->next;
  }
  curr->next = nullptr;
  return dummy.next;
}

std::ostream& operator<<(std::ostream& os, ListNode* head) {
  while (head) {
    os << head->val << "->";
    head = head->next;
  }
  os << "nullptr\n";
  return os;
}

class Solution {
 public:
  ListNode* reverseBetween(ListNode* head, int left, int right) {
    ListNode* new_tail = nullptr;
    ListNode* new_head = nullptr;
    ListNode dummy;
    dummy.next = head;

    ListNode* before_node = &dummy;
    ListNode* curr = &dummy;
    for (int i = 0; i < left; i++) {
      if (i == left - 1) {
        before_node = curr;
      }
      curr = curr->next;
    }
    new_tail = curr;

    ListNode* prev = nullptr;
    for (int i = left; i < right; i++) {
      ListNode* next = curr->next;
      curr->next = prev;
      prev = curr;
      curr = next;
    }

    ListNode* next_node = curr->next;
    curr->next = prev;
    new_head = curr;

    before_node->next = new_head;
    new_tail->next = next_node;

    return dummy.next;
  }
};

int main() {
  vector<int> v = {1, 2, 3, 4, 5};
  ListNode* head = gen_list(v);
  std::cout << head;

  Solution s;
  std::cout << s.reverseBetween(head, 2, 4) << std::endl;

  return 0;
}