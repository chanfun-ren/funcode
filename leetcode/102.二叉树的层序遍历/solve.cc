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
  if (v.empty()) return nullptr;
  size_t n = v.size();
  vector<TreeNode *> nodes;
  nodes.reserve(n);
  for (int val : v) {
    nodes.push_back(val != -1 ? new TreeNode(val) : nullptr);
  }
  for (size_t i = 0; i < n; ++i) {
    if (!nodes[i]) {
      continue;
    }
    nodes[i]->left = 2 * i + 1 < n ? nodes[2 * i + 1] : nullptr;
    nodes[i]->right = 2 * i + 2 < n ? nodes[2 * i + 2] : nullptr;
  }
  return nodes[0];
}

void print_tree(TreeNode *root) {
  if (!root) return;
  std::cout << root->val << " ";
  print_tree(root->left);
  print_tree(root->right);
}

class Solution {
 public:
  vector<vector<int>> levelOrder(TreeNode *root) {
    vector<vector<int>> res;
    if (!root) {
      return res;
    }
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
      res.push_back(level_vals);
    }
    return res;
  }
};

// template <typename T>
// void print_vec(const vector<T> &v) {
//   for (const auto &e : v) {
//     std::cout << e << " ";
//   }
//   std::cout << std::endl;
// }

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
  os << "[";
  for (auto it = vec.begin(); it != vec.end(); it++) {
    os << *it;
    if (it != vec.end() - 1) {
      os << ", ";
    }
  }
  os << "]\n";
  return os;
}

int main() {
  vector<int> v = {3, 9, 20, -1, -1, 15, 7};
  TreeNode *root = build_tree(v);
  // print_tree(root);
  Solution s;
  vector<vector<int>> res = s.levelOrder(root);
  for (vector<int> &vec : res) {
    std::cout << vec;
  }

  return 0;
}