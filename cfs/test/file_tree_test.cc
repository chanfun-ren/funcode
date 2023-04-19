
#include "file_tree.h"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <set>
using namespace cfs;

std::set<std::string> GetChildrenPath(FileTree* ft,
                                      std::string_view node_path) {
  FileNode* node = ft->FindNode(node_path);
  std::set<std::string> children_path;
  for (auto* child : node->children) {
    children_path.insert(child->file_path);
  }
  return children_path;
}

TEST_CASE("File Tree CURD", "[file_tree]") {
  FileTree* ft = new FileTree();
  REQUIRE(ft->FindNode("/")->file_path == "/");
  REQUIRE(ft->FindNode("/")->file_name == "/");

  FileNode* root = ft->FindNode("/");

  SECTION("create new file/dir") {
    std::set<std::string> inserted_files = {"/a", "/a.txt", "/b.md"};
    for (auto& path : inserted_files) {
      ft->CreateNode(path, 1);
    }
    std::set<std::string> result = GetChildrenPath(ft, "/");
    REQUIRE(inserted_files == result);

    // std::vector<std::string> inserted_files = {"/a/b/c/d"};
    ft->CreateNode("/a/b/c", 1);
    ft->CreateNode("/e/f", 1);

    REQUIRE(GetChildrenPath(ft, "/").count("/a"));
    REQUIRE(GetChildrenPath(ft, "/a").count("/a/b"));
    REQUIRE(GetChildrenPath(ft, "/a/b").count("/a/b/c"));
    REQUIRE(GetChildrenPath(ft, "/").count("/e"));
    REQUIRE(GetChildrenPath(ft, "/e").count("/e/f"));
  }

  SECTION("create file and find file") {
    ft->CreateNode("/xx/yy", 1);
    ft->CreateNode("/xx/yy/t.md", 5);

    FileNode* node0 = ft->FindNode("/");
    REQUIRE((node0 != nullptr && node0->file_path == "/" &&
             node0->file_name == "/"));

    FileNode* node1 = ft->FindNode("/xx");
    REQUIRE((node1 != nullptr && node1->file_path == "/xx" &&
             node1->file_name == "xx"));

    FileNode* node2 = ft->FindNode("/xx/yy");
    REQUIRE((node2 != nullptr && node2->file_path == "/xx/yy" &&
             node2->file_name == "yy"));

    FileNode* node3 = ft->FindNode("/xx/yy/t.md");
    REQUIRE((node3 != nullptr && node3->file_path == "/xx/yy/t.md" &&
             node3->file_name == "t.md"));
  }

  SECTION("create file and delete file") {
    ft->CreateNode("/w/z/y.md", 2);
    ft->CreateNode("/w/z/x.md", 3);

    ft->DeleteNode("/w/z/y.md");

    REQUIRE(ft->FindNode("/w/z/y.md") == nullptr);
    REQUIRE(ft->FindNode("/w/z/x.md") != nullptr);

    REQUIRE(GetChildrenPath(ft, "/").count("/w"));
    REQUIRE(GetChildrenPath(ft, "/w").count("/w/z"));
    REQUIRE(GetChildrenPath(ft, "/w/z").count("/w/z/x.md") != 0);
  }

  SECTION("supplement for debugging stat_list_stat") {
    ft->CreateNode("/e", 1);
    FileNode* node0 = ft->FindNode("/e");
    REQUIRE((node0 != nullptr && node0->file_path == "/e" &&
             node0->file_name == "e"));

    // FileNode* node1 = ft->FindNode("/e");
    // REQUIRE((node1 != nullptr && node1->file_path == "/e" &&
    //          node1->file_name == "e"));
    auto children_path = GetChildrenPath(ft, "/");
    REQUIRE(children_path.count("/e"));

    FileNode* node2 = ft->FindNode("/e");
    REQUIRE((node2 != nullptr && node2->file_path == "/e" &&
             node2->file_name == "e"));
  }
}
