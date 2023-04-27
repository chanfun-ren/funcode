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

void print_list(ListNode* head) {
  while (head) {
    std::cout << head->val << "->";
    head = head->next;
  }
  std::cout << "nullptr" << std::endl;
}

class Solution {
 public:
  ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
    ListNode dummy;
    ListNode* curr = &dummy;
    while (list1 && list2) {
      if (list1->val < list2->val) {
        curr->next = list1;
        list1 = list1->next;
      } else {
        curr->next = list2;
        list2 = list2->next;
      }
      curr = curr->next;
    }
    curr->next = list1 ? list1 : list2;
    return dummy.next;
  }
};

int main() {
  Solution s;
  vector v = {1, 2, 4};
  vector v1 = {1, 3, 4};
  ListNode* res = s.mergeTwoLists(gen_list(v), gen_list(v1));

  print_list(res);

  return 0;
}