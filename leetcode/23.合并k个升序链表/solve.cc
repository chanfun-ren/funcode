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
  using Itertype = vector<ListNode*>::iterator;
  // 分治 => merge2lists(merge(k/2), merge(k/2));
  ListNode* mergeKLists(vector<ListNode*>& lists) {
    if (lists.empty()) return nullptr;
    return mergeKLists(lists, 0, lists.size() - 1);
  }

 private:
  ListNode* mergeKLists(vector<ListNode*>& lists, int begin, int end) {
    if (begin == end) return lists[begin];
    size_t len = end - begin;
    return merge2Lists(mergeKLists(lists, begin, begin + len / 2),
                       mergeKLists(lists, begin + len / 2 + 1, end));
  }
  ListNode* merge2Lists(ListNode* alist, ListNode* blist) {
    if (!alist) return blist;
    if (!blist) return alist;
    ListNode dummy;
    ListNode* curr = &dummy;
    while (alist && blist) {
      if (alist->val > blist->val) {
        curr->next = blist;
        blist = blist->next;
      } else {
        curr->next = alist;
        alist = alist->next;
      }
      curr = curr->next;
    }
    if (!alist) curr->next = blist;
    if (!blist) curr->next = alist;
    return dummy.next;
  }
};

// caller owns the list.(owner delete)
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
  std::cout << "nullptr\n";
}

int main() {
  vector<vector<int>> arr = {{1, 4, 5}, {1, 3, 4}, {2, 6}};
  vector<ListNode*> lists;
  for (const auto& v : arr) {
    lists.push_back(gen_list(v));
    // print_list(gen_list(v));
  }

  Solution s;
  ListNode* res = s.mergeKLists(lists);
  print_list(res);
  return 0;
}