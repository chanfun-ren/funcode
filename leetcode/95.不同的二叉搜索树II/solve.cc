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
  vector<TreeNode *> generateTrees(int n) { return generateTrees(1, n); }

 private:
  vector<TreeNode *> generateTrees(int begin, int end) {
    if (begin > end) return {nullptr};
    vector<TreeNode *> res;
    for (int i = begin; i <= end; ++i) {
      vector<TreeNode *> left_choices = generateTrees(begin, i - 1);
      vector<TreeNode *> right_choices = generateTrees(i + 1, end);
      for (auto l : left_choices) {
        for (auto r : right_choices) {
          TreeNode *root = new TreeNode(i);
          root->left = l;
          root->right = r;
          res.push_back(root);
        }
      }
    }
    return res;
  }
};

void print_tree(TreeNode *root) {
  if (!root) return;
  std::cout << root->val << " ";
  print_tree(root->left);
  print_tree(root->right);
}

int main() {
  Solution s;
  auto res = s.generateTrees(3);
  for (auto tr : res) {
    print_tree(tr);
    std::cout << std::endl;
  }

  return 0;
}