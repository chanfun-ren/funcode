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
  void reorderList(ListNode* head) {
    if (!head) return;
    ListNode dummy;
    dummy.next = head;

    // 找到链表中点
    ListNode *fast = &dummy, *slow = &dummy;
    while (fast && fast->next) {
      fast = fast->next->next;
      slow = slow->next;
    }
    // 0 - 1;
    ListNode* right_head = slow->next;
    slow->next = nullptr;

    // 反转后半段
    right_head = reverse_list(right_head);

    // 合并两段
    dummy.next = merge_list(dummy.next, right_head);
  }
  // 1->nullptr;
  // 2->3->nullptr;

  ListNode* merge_list(ListNode* list1, ListNode* list2) {
    if (!list1) return list2;
    if (!list2) return list1;
    ListNode dummy;
    dummy.next = list1;
    ListNode* curr = &dummy;
    int i = 1;
    while (list1 && list2) {
      if ((i++) % 2) {
        curr->next = list1;
        list1 = list1->next;
      } else {
        curr->next = list2;
        list2 = list2->next;
      }
      curr = curr->next;
    }
    if (list1) curr->next = list1;
    if (list2) curr->next = list2;
    return dummy.next;
  }

  ListNode* reverse_list(ListNode* head) {
    if (!head) return nullptr;
    ListNode* prev = nullptr;
    while (head) {
      ListNode* tmp = head->next;
      head->next = prev;
      prev = head;
      head = tmp;
    }
    return prev;
  }
};

int main() {
  vector<int> v = {1, 2, 3, 4, 5};
  auto* head = gen_list(v);
  std::cout << head << std::endl;

  Solution s;
  s.reorderList(head);
  std::cout << head << std::endl;

  return 0;
}