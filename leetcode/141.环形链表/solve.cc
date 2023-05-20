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
  bool hasCycle(ListNode* head) {
    if (!head) return false;
    ListNode dummy;
    dummy.next = head;
    ListNode* fast = &dummy;
    ListNode* slow = &dummy;
    while (fast && fast->next) {
      fast = fast->next->next;
      slow = slow->next;
      if (fast == slow) {
        return true;
      }
    }
    return false;
  }
};
int main() {
  vector<int> v = {3, 2, 0, 4};
  ListNode* head = gen_list(v);
  head->next->next->next->next = head->next;

  Solution s;
  std::cout << s.hasCycle(head) << std::endl;

  return 0;
}