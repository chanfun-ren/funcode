#include "file_tree.h"

#include <queue>

#include "spdlog/spdlog.h"

namespace cfs {
FileTree::FileTree() { root_ = new FileNode("/", "/", 1); }

FileTree::~FileTree() { Destory(root_); }

void FileTree::PrintTree() {
  std::cout << "current_tree:\n";
  std::queue<FileNode*> q;
  q.push(root_);
  while (!q.empty()) {
    int level_size = q.size();
    for (int i = 0; i < level_size; ++i) {
      FileNode* node = q.front();
      q.pop();
      // Do something with node
      std::cout << node->file_name << " " << node->file_size << "\t";
      // Push children nodes to queue
      for (auto child : node->children) {
        q.push(child);
      }
    }
    std::cout << "\n";
  }
}

FileNode* FileTree::FindNode(std::string_view file_path) {
  // std::cout << "findnode-" << file_path << " begin : ";
  PrintTree();
  auto res = FindNodeHelper(root_, file_path);
  // std::cout << "findnode-" << file_path << " end: ";
  PrintTree();
  return res;
}

// 使用该函数者必须确保传入的路径是 "/a/b/c" 形式且传入的路径不存在文件树中
FileNode* FileTree::CreateNode(std::string_view file_path, size_t file_size) {
  // std::cout << "begin create node: " << file_path << " ...\n";
  spdlog::debug("begin create node: {}...\n", file_path);
  if (file_path.empty()) return nullptr;
  std::vector<int> sep_index;
  for (size_t i = 0; i < file_path.size(); i++) {
    if (file_path[i] == '/') {
      sep_index.push_back(i);
    }
  }
  sep_index.push_back(file_path.size());

  FileNode* curr = root_;
  std::string intermediate_path;
  std::string parent_name = "/";
  int index = 0;
  while (intermediate_path != file_path) {
    // std::string searched_file_name = file_path.substr(
    //     sep_index[index] + 1, sep_index[index + 1] - sep_index[index] - 1);
    std::string searched_file_name(file_path.begin() + sep_index[index] + 1,
                                   file_path.begin() + sep_index[index + 1]);
    ++index;

    // intermediate_path: in the form of "/a", "/a/b"
    intermediate_path += "/";
    intermediate_path += searched_file_name;

    FileNode* child = FindNodeHelper(curr, intermediate_path);
    if (child == nullptr) {
      child = new FileNode(intermediate_path, searched_file_name, file_size);
      curr->children.push_back(child);
    }
    curr = child;
  }
  // std::cout << "create " << curr->file_path << " done\n";
  PrintTree();
  return curr;
}

void FileTree::DeleteNode(std::string_view file_path) {
  // std::cout << "deletenode begin: ";
  PrintTree();
  if (file_path == "/") {
    Destory(root_);
    return;
  }

  FileNode* find_res = FindNodeHelper(root_, file_path);
  if (find_res == nullptr) {
    return;
  }

  // Find its parent-node. `parent-pointer` can be set in `FileNode` to make
  // this simpler.
  int last_slash_index = file_path.rfind('/');
  FileNode* parent_node = root_;
  if (last_slash_index != 0) {
    // std::string parent_path = file_path.substr(0, last_slash_index);
    std::string parent_path(file_path.begin(),
                            file_path.begin() + last_slash_index);
    parent_node = FindNodeHelper(root_, parent_path);
  }

  // delete the child from the parent, and destory the subtree.
  std::vector<FileNode*>& sons = parent_node->children;
  for (auto it = sons.begin(); it != sons.end(); ++it) {
    if (*it == find_res) {
      sons.erase(it);
      break;
    }
  }
  // std::cout << "delete: " << find_res->file_path << "\n";
  Destory(find_res);
  // std::cout << "deletenode done: ";
  PrintTree();
}

FileNode* FileTree::FindNodeHelper(FileNode* root, std::string_view file_path) {
  if (!root) return nullptr;
  if (root->file_path == file_path) return root;
  for (size_t i = 0; i < root->children.size(); i++) {
    FileNode* find_res = FindNodeHelper(root->children[i], file_path);
    if (find_res != nullptr) {
      return find_res;
    }
  }
  return nullptr;
}

void FileTree::Destory(FileNode* root) {
  // dfs traverse delete.
  if (!root) return;
  for (size_t i = 0; i < root->children.size(); i++) {
    Destory(root->children[i]);
  }
  delete root;
  root = nullptr;
}

}  // namespace cfs
