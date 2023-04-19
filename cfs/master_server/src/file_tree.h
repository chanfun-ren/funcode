#pragma once

#include <iostream>
#include <string_view>
#include <vector>

namespace cfs {
struct FileNode {
  // `file_name` is a bit redunlant.
  std::string file_path;
  std::string file_name;
  size_t file_size;
  // TODO: {file_path, file_name} => class FileMeta
  // using cfs::common::FileMeta;
  // FileMeta file_meta_;
  // FileNode* parent = nullptr;
  std::vector<FileNode*> children;
  FileNode(std::string_view path, std::string_view name, size_t size)
      : file_path(path), file_name(name), file_size(size) {}
};
class FileTree {
 private:
  FileNode* root_;

 public:
  FileTree();
  ~FileTree();

  // for debugging
  void PrintTree();

  // get,stat file info; list the subdir and files
  FileNode* FindNode(std::string_view file_path);

  // make dir, put file
  FileNode* CreateNode(std::string_view file_path, size_t file_size);

  // delete file
  void DeleteNode(std::string_view file_path);

 private:
  void Destory(FileNode* root);
  FileNode* FindNodeHelper(FileNode* root, std::string_view file_path);
};

}  // namespace cfs
