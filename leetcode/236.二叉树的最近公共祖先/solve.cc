#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::unordered_map;
using std::unordered_set;
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
  vector<TreeNode *> nodes(n, nullptr);
  for (size_t i = 0; i < n; i++) {
    nodes[i] = v[i] == -1 ? nullptr : new TreeNode(v[i]);
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

void print_tree(TreeNode *root) {
  if (!root) return;
  std::cout << root->val << " ";
  print_tree(root->left);
  print_tree(root->right);
}

TreeNode *find_node(TreeNode *root, int val) {
  if (!root) return nullptr;
  if (root->val == val) return root;
  TreeNode *find_res = find_node(root->left, val);
  return find_res ? find_res : find_node(root->right, val);
}

class Solution {
 public:
  TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *p, TreeNode *q) {
    if (!root) return nullptr;

    // 0. 遍历整棵树，记录各个节点的父亲节点
    // 1. 从 p 节点出发向上走，标记路径
    // 2. 从 q 节点出发向上走，如果该节点被标记过，即为所求
    unordered_map<TreeNode *, TreeNode *> parent_map;
    parent_map[root] = nullptr;
    traverse(parent_map, root);

    unordered_set<TreeNode *> path;
    while (p) {
      path.insert(p);
      p = parent_map[p];
    }

    while (q) {
      if (path.count(q)) {
        return q;
      }
      q = parent_map[q];
    }
    return nullptr;
  }

 private:
  void traverse(unordered_map<TreeNode *, TreeNode *> &parent_map,
                TreeNode *root) {
    if (!root) return;
    if (root->left) parent_map[root->left] = root;
    if (root->right) parent_map[root->right] = root;
    traverse(parent_map, root->left);
    traverse(parent_map, root->right);
  }
};

int main() {
  vector<int> v = {3, 5, 1, 6, 2, 0, 8, -1, -1, 7, 4};
  TreeNode *root = build_tree(v);
  TreeNode *p = find_node(root, 1);
  TreeNode *q = find_node(root, 4);
  Solution s;

  std::cout << s.lowestCommonAncestor(root, p, q)->val << std::endl;

  return 0;
}