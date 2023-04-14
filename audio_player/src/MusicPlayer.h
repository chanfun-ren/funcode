#pragma once
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

// 引入libvlc头文件
#include <vlc/vlc.h>

using namespace std;

// 音乐文件结构体
struct Music {
  string path;   // 音乐文件路径
  string name;   // 音乐文件名
  int duration;  // 音乐时长 ms
};

// 播放器状态枚举
enum PlayerState {
  STOPPED = 0,  // 停止状态
  PLAYING,      // 播放状态
  PAUSED        // 暂停状态
};

// 自定义播放模式。libvlc 只有三种：repeat, loop, default. 没有随机播放模式
enum PlayMode {
  REPEAT = 0,  // 单曲循环
  LOOP,        // 列表循环
  RANDOM,      // 随机播放
  DEFAULT      // 播放完当前列表停止
};

// 播放器类
class MusicPlayer {
 private:
  int getRandomInt(size_t n) const;

 private:
  // 初始化播放器
  void init();

  // 初始化 VLC 播放器
  void initVLC();

  // 清空播放列表
  void clearPlaylist();

  // 播放器线程函数
  void playerThreadFunc();

  // 播放器状态
  PlayerState state_;

  // 播放列表
  vector<Music> playlist_;

  // 当前播放歌曲索引
  int currentMusicIndex_;

  // 播放器控制锁 -> 保护 state_
  mutex playerMutex_;

  // 播放器控制条件变量
  condition_variable playerCV_;

  // VLC 播放器实例:
  libvlc_instance_t* vlcInstance_;

  // VLC 播放器媒体播放器
  // The normal `libvlc_media_player_t` LibVLC media player can only play a
  // single media, and does not handle playlist files properly.
  libvlc_media_player_t* vlcMediaPlayer_;

  // The LibVLC media list player plays a `libvlc_media_list_t` list of
  // media, in a certain order.
  libvlc_media_list_player_t* vlcMediaListPlayer_;

  // VLC 播放列表媒体
  libvlc_media_list_t* vlcMediaList_;

  // 播放模式: vlc 貌似只有三种：repeat, loop, default. 没有随机播放模式
  PlayMode playMode_;

  // 音量
  int volume_;

  // 播放器线程
  thread playerThread_;

  // 是否退出播放器线程
  bool exitPlayerThread_;

 public:
  MusicPlayer();
  ~MusicPlayer();

  // 加载音乐目录
  void loadMusicFolder(const string& folderPath);

  // 切换播放状态
  void toggleState();
  void togglePlay();

  // 切换播放模式
  void toggleMode();

  // 设置音量
  void setVolume(int volume);

  // 获取音量
  int getVolume() const;

  int* getMutVolume();

  // 获取当前播放器状态
  PlayerState getState() const;

  // 获取当前播放模式
  PlayMode getMode() const;

  // 设置播放模式
  void setMode(PlayMode playmode);

  // 获取当前播放进度（秒）
  int getCurrentTime() const;

  // 获取当前播放歌曲总时长（秒）
  int getTotalTime() const;

  // 获取当前播放歌曲名
  string getCurrentMusicName() const;

  // 获取当前播放歌曲在播放列表中的索引
  int getCurrentIndex() const;

  // 添加音乐到播放列表
  void addToPlaylist(const string& music);

  // 播放列表 index 处音乐
  void playMusicAtIndex(int index);

  // 播放下一首歌曲
  void playNext();

  // 播放上一首歌曲
  void playPrev();

  // 播放
  void play();

  // 结束
  void stop();

  // 暂停
  void pause();

  // 继续
  void resume();

  vector<Music> getPlaylist() const;

  bool isPlaying() const;
};