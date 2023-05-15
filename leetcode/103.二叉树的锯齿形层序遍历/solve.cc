#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using std::queue;
using std::vector;

// Definition for a binary tree node.
struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : val(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right)
      : val(x), left(left), right(right) {}
};

TreeNode *build_tree(vector<int> &v) {
  if (v.empty()) {
    return nullptr;
  }
  size_t n = v.size();
  vector<TreeNode *> nodes;
  for (int val : v) {
    nodes.push_back(val == -1 ? nullptr : new TreeNode(val));
  }
  for (size_t i = 0; i < n; i++) {
    if (!nodes[i]) {
      continue;
    }
    nodes[i]->left = 2 * i + 1 < n ? nodes[2 * i + 1] : nullptr;
    nodes[i]->right = 2 * i + 2 < n ? nodes[2 * i + 2] : nullptr;
  }
  return nodes[0];
}

template <typename T>
std::ostream &operator<<(std::ostream &os, std::vector<T> &vec) {
  os << "[";
  for (auto it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it;
    if (it != vec.end() - 1) {
      std::cout << ", ";
    }
  }
  os << "]\n";
  return os;
}

class Solution {
 public:
  vector<vector<int>> zigzagLevelOrder(TreeNode *root) {
    vector<vector<int>> res;
    if (!root) {
      return res;
    }
    bool reverse_flag = false;
    queue<TreeNode *> q;
    q.push(root);
    while (!q.empty()) {
      size_t n = q.size();
      vector<int> level_vals;
      for (size_t i = 0; i < n; i++) {
        TreeNode *node = q.front();
        q.pop();
        level_vals.push_back(node->val);
        if (node->left) {
          q.push(node->left);
        }
        if (node->right) {
          q.push(node->right);
        }
      }
      if (reverse_flag) {
        std::reverse(level_vals.begin(), level_vals.end());
      }
      reverse_flag = !reverse_flag;
      res.push_back(level_vals);
    }
    return res;
  }
};

int main() {
  vector<int> v = {3, 9, 20, -1, -1, 15, 7};
  TreeNode *root = build_tree(v);
  Solution s;
  auto res = s.zigzagLevelOrder(root);
  for (auto vec : res) {
    std::cout << vec;
  }
  return 0;
}