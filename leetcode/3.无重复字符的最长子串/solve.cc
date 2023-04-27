#include <iostream>
#include <set>
#include <vector>

class Solution {
 public:
  // 从左到右扫描，维护一个 set 记录不重复的子串，更新 res。
  // 如果 curr 在 set 中，弹出 set 之前所有元素。不如用 map，记录 [val, index].
  // => 记录左右指针，维护一个窗口，当 curr 已经在窗口出现过时，left++, 直至
  // s[left] = curr. set 删除 left++ 过程中的元素
  int lengthOfLongestSubstring(std::string s) {
    int res = 0;
    int left = 0;
    int right = 0;
    std::set<char> set;
    while (right < s.size()) {
      if (set.contains(s[right])) {
        while (set.contains(s[right])) {
          set.erase(s[left]);
          left++;
        }
        set.insert(s[right]);
      } else {
        set.insert(s[right]);
      }
      res = std::max(res, right - left + 1);
      std::cout << left << ", " << right << "\n";
      right++;
    }
    return res;
  }
};

int main() {
  Solution s;
  std::string input = "abcabcbb";
  std::cout << s.lengthOfLongestSubstring(input) << std::endl;

  return 0;
}