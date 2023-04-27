#include <iostream>
#include <vector>

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

class Solution {
 public:
  int rob(TreeNode *root) {
    if (!root) return 0;
    auto [do_steal, dont] = dfs(root);
    return std::max(do_steal, dont);
  }
  std::pair<int, int> dfs(TreeNode *root) {
    if (!root) return {0, 0};
    auto [left_do_steal, left_dont] = dfs(root->left);
    auto [right_do_steal, right_dont] = dfs(root->right);
    // 当前节点的更新依赖于其子节点的值
    int do_steal = left_dont + right_dont + root->val;
    int dont = std::max(left_do_steal, left_dont) +
               std::max(right_do_steal, right_dont);
    return {do_steal, dont};
  }
};

// -1 表示空节点
TreeNode *build_tree(vector<int> &v) {
  if (v.empty()) return nullptr;
  vector<TreeNode *> nodes;
  for (int value : v) {
    nodes.emplace_back(new TreeNode(value));
  }
  int n = nodes.size();
  for (size_t i = 0; i < n; ++i) {
    if (nodes[i]->val == -1) {
      // null node
      continue;
    }
    TreeNode *left = (2 * i + 1 < n) ? nodes[2 * i + 1] : nullptr;
    TreeNode *right = (2 * i + 2 < n) ? nodes[2 * i + 2] : nullptr;
    nodes[i]->left = left;
    nodes[i]->right = right;
  }
  return nodes[0];
}
void print_tree(TreeNode *root) {
  if (!root) return;
  std::cout << root->val << " ";
  print_tree(root->left);
  print_tree(root->right);
}

int main() {
  vector<int> v = {3, 2, 3, -1, 3, -1, 1};
  TreeNode *root = build_tree(v);
  print_tree(root);
  Solution s;
  std::cout << "\n" << s.rob(root) << std::endl;

  return 0;
}