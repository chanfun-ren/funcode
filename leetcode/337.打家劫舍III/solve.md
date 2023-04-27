
- 对于某棵以 root 为根节点的子树，可以偷 root，也可以不偷 root，即 root[偷/不偷]，取两种状态的最大值。
- root[偷] = r.left[不偷] + r.right[不偷] + root.val
- root[不偷] = max(r.left[偷], r.left[不偷]) + max(r.right[偷], r.right[不偷])
- 最后返回 max(root[偷], root[不偷])。边界情况：叶子节点 leaf[偷] = leaf.val, leat[不偷] = 0
- 综上分析：后序遍历，先求叶子节点的状态，最终传导到 root 节点