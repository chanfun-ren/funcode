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

class Solution {
 public:
  ListNode* reverseKGroup(ListNode* list, int k) {
    // 思路：每 k 个翻转链表. 拼接起来
    // 关键点：定义 prev, head, tail, next 四个变量，方便进行拼接
    if (!list || !list->next || k == 1) return list;
    ListNode dummy;
    dummy.next = list;
    ListNode* prev = &dummy;
    ListNode* head = dummy.next;
    while (head != nullptr) {
      ListNode* tail = head;
      for (int i = 0; i < k - 1 && tail; ++i) {
        tail = tail->next;
        if (!tail) return dummy.next;
      }
      ListNode* next = tail->next;
      // prev -> head -> ... -> tail -> next
      tail->next = nullptr;
      ListNode* new_head = reverse(head);
      // 拼接
      prev->next = new_head;
      head->next = next;
      prev = head;  // wrong
      head = prev->next;
    }
    return dummy.next;
  }
  ListNode* reverse(ListNode* head) {
    if (!head) return nullptr;
    ListNode* prev = nullptr;
    ListNode* curr = head;
    while (curr) {
      ListNode* tmp = curr->next;
      curr->next = prev;
      prev = curr;
      curr = tmp;
    }
    return prev;
  }
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

int main() {
  Solution s;
  auto* list = gen_list({1, 2, 3, 4, 5});
  auto* new_list = s.reverseKGroup(list, 1);
  print_list(new_list);

  return 0;
}
