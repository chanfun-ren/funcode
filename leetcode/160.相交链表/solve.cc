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
  ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    if (!headA || !headB) {
      return nullptr;
    }
    ListNode* currA = headA;
    ListNode* currB = headB;
    while (currA != currB) {
      currA = currA ? currA->next : headB;
      currB = currB ? currB->next : headA;
    }
    return currA;
  }
};

int main() {
  vector<int> v = {8, 4, 5};
  ListNode* intersect = gen_list(v);
  ListNode node1(4);
  ListNode node2(1);
  ListNode node3(5);
  ListNode node4(6);
  ListNode node5(1);

  node1.next = &node2;
  node2.next = intersect;
  node3.next = &node4;
  node4.next = &node5;
  node5.next = intersect;

  Solution s;
  std::cout << s.getIntersectionNode(&node1, &node3);

  return 0;
}