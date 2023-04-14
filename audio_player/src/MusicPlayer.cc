#include "MusicPlayer.h"

MusicPlayer::MusicPlayer() {
  state_ = PlayerState::STOPPED;
  currentMusicIndex_ = 0;
  vlcInstance_ = nullptr;
  vlcMediaList_ = nullptr;
  vlcMediaPlayer_ = nullptr;
  vlcMediaListPlayer_ = nullptr;
  playMode_ = PlayMode::DEFAULT;
  volume_ = 100;
  exitPlayerThread_ = false;

  init();
}

MusicPlayer::~MusicPlayer() {
  // 停止播放器线程
  exitPlayerThread_ = true;
  playerCV_.notify_all();
  if (playerThread_.joinable()) {
    playerThread_.join();
  }

  // 释放 VLC 相关资源

  // 释放 VLC 媒体列表中的媒体
  if (vlcMediaList_ != nullptr) {
    libvlc_media_list_lock(vlcMediaList_);
    while (libvlc_media_list_count(vlcMediaList_) > 0) {
      // 每次都删除头元素
      libvlc_media_t* media = libvlc_media_list_item_at_index(vlcMediaList_, 0);
      libvlc_media_release(media);
      libvlc_media_list_remove_index(vlcMediaList_, 0);
    }
    libvlc_media_list_unlock(vlcMediaList_);
  }
  if (vlcMediaList_ != nullptr) {
    libvlc_media_list_release(vlcMediaList_);
  }

  if (vlcInstance_ != nullptr) {
    libvlc_release(vlcInstance_);
  }

  if (vlcMediaPlayer_ != nullptr) {
    libvlc_media_player_release(vlcMediaPlayer_);
  }

  if (vlcMediaListPlayer_ != nullptr) {
    libvlc_media_list_player_release(vlcMediaListPlayer_);
  }
}

void MusicPlayer::init() {
  // 初始化 VLC 播放器
  initVLC();

  // 启动播放器线程
  // playerThread_ = thread(&MusicPlayer::playerThreadFunc, this);
}

void MusicPlayer::initVLC() {
  // TODO: error code. no exception.
  // 创建 VLC 实例
  vlcInstance_ = libvlc_new(0, nullptr);
  if (vlcInstance_ == nullptr) {
    throw runtime_error("Failed to create VLC instance");
  }

  // 创建播放列表
  vlcMediaList_ = libvlc_media_list_new(vlcInstance_);
  if (vlcMediaList_ == nullptr) {
    throw runtime_error("Failed to create VLC media list");
  }

  // 创建媒体播放器
  vlcMediaListPlayer_ = libvlc_media_list_player_new(vlcInstance_);
  vlcMediaPlayer_ = libvlc_media_player_new(vlcInstance_);
  if (vlcMediaPlayer_ == nullptr) {
    throw runtime_error("Failed to create VLC media player");
  }

  // 设置媒体播放器的播放列表
  libvlc_media_list_player_set_media_player(vlcMediaListPlayer_,
                                            vlcMediaPlayer_);
  libvlc_media_list_player_set_media_list(vlcMediaListPlayer_, vlcMediaList_);

  // 设置音量
  libvlc_audio_set_volume(vlcMediaPlayer_, volume_);
}

int MusicPlayer::getCurrentIndex() const { return currentMusicIndex_; }

void MusicPlayer::clearPlaylist() {
  // 清空播放列表
  playlist_.clear();

  // 释放 VLC 媒体列表中的媒体
  if (vlcMediaList_ != nullptr) {
    libvlc_media_list_lock(vlcMediaList_);
    while (libvlc_media_list_count(vlcMediaList_) > 0) {
      // 每次都删除头元素
      libvlc_media_t* media = libvlc_media_list_item_at_index(vlcMediaList_, 0);
      libvlc_media_release(media);
      libvlc_media_list_remove_index(vlcMediaList_, 0);
    }
    libvlc_media_list_unlock(vlcMediaList_);
  }
}

void MusicPlayer::addToPlaylist(const string& filepath) {
  // 创建媒体对象
  libvlc_media_t* media = libvlc_media_new_path(vlcInstance_, filepath.c_str());
  if (media == nullptr) {
    throw runtime_error("Failed to create " + filepath);
  }

  // 创建音乐对象并添加到播放列表中
  Music music;
  music.path = filepath;
  music.name = std::filesystem::path(filepath).filename().string();
  music.duration = libvlc_media_get_duration(media);
  playlist_.push_back(music);

  // 将媒体对象添加到 VLC 媒体列表中
  // libvlc_media_list_add_media(): The libvlc_media_list_lock should be held
  // upon entering this function.`
  libvlc_media_list_lock(vlcMediaList_);
  libvlc_media_list_add_media(vlcMediaList_, media);
  libvlc_media_list_unlock(vlcMediaList_);
}

void MusicPlayer::play() {
  // 如果播放列表为空，则不播放任何音乐
  if (playlist_.empty()) {
    return;
  }

  // 播放第一首音乐
  libvlc_media_list_player_play_item_at_index(vlcMediaListPlayer_, 0);
  currentMusicIndex_ = 0;
  state_ = PlayerState::PLAYING;
}

void MusicPlayer::pause() {
  libvlc_media_player_pause(vlcMediaPlayer_);
  state_ = PlayerState::PAUSED;
}

void MusicPlayer::resume() {
  libvlc_media_player_set_pause(vlcMediaPlayer_, 0);
  state_ = PlayerState::PLAYING;
}

void MusicPlayer::stop() {
  libvlc_media_player_stop(vlcMediaPlayer_);
  state_ = PlayerState::STOPPED;
}

void MusicPlayer::playNext() {
  // 如果播放列表为空，则不播放任何音乐
  if (playlist_.empty()) {
    return;
  }

  // 获取当前正在播放的音乐在播放列表中的索引
  int currentIndex = currentMusicIndex_;

  // 计算下一首要播放的音乐在播放列表中的索引
  int nextIndex = -1;
  switch (playMode_) {
    case RANDOM: {
      nextIndex = getRandomInt(playlist_.size());
      break;
    }
    case REPEAT: {
      nextIndex = currentIndex;
      break;
    }
    default: {
      nextIndex = (currentIndex + 1) % playlist_.size();
      break;
    }
  }

  // 播放下一首音乐
  libvlc_media_list_player_play_item_at_index(vlcMediaListPlayer_, nextIndex);
  currentMusicIndex_ = nextIndex;
  state_ = PlayerState::PLAYING;
}

void MusicPlayer::playPrev() {
  if (playlist_.empty()) {
    return;
  }

  // 获取当前正在播放的音乐在播放列表中的索引
  int currentIndex = currentMusicIndex_;

  // 计算下一首要播放的音乐在播放列表中的索引
  int prevIndex = -1;
  int n = playlist_.size();
  switch (playMode_) {
    // 除随机模式外，其他模式均切换成循环列表的下一首
    case RANDOM: {
      prevIndex = getRandomInt(n);
      break;
    }
    default: {
      prevIndex = (currentIndex + n - 1) % n;
      break;
    }
  }

  // 播放上一首音乐
  libvlc_media_list_player_play_item_at_index(vlcMediaListPlayer_, prevIndex);
  currentMusicIndex_ = prevIndex;
  state_ = PlayerState::PLAYING;
}

int MusicPlayer::getVolume() const { return volume_; }
int* MusicPlayer::getMutVolume() { return &volume_; }

void MusicPlayer::setVolume(int volume) {
  volume_ = volume;
  if (vlcMediaPlayer_ != nullptr) {
    libvlc_audio_set_volume(vlcMediaPlayer_, volume);
  }
}

vector<Music> MusicPlayer::getPlaylist() const { return playlist_; }

bool MusicPlayer::isPlaying() const {
  // or return state_ == PlayerState::PLAYING;
  return libvlc_media_player_is_playing(vlcMediaPlayer_);
}

void MusicPlayer::playMusicAtIndex(int index) {
  if (index >= 0 && index < playlist_.size()) {
    libvlc_media_list_player_stop(vlcMediaListPlayer_);
    libvlc_media_list_player_play_item_at_index(vlcMediaListPlayer_, index);
    currentMusicIndex_ = index;
    state_ = PlayerState::PLAYING;
  }
}

// 播放器线程函数
void MusicPlayer::playerThreadFunc() {
  while (true) {
    unique_lock<mutex> lock(playerMutex_);

    // 等待通知
    playerCV_.wait(lock);

    // 根据当前播放状态执行相应的操作
    if (state_ == PLAYING) {
      // 等待直到歌曲播放完毕或者被暂停/停止
      while (state_ == PLAYING) {
        libvlc_state_t vlcState =
            libvlc_media_player_get_state(vlcMediaPlayer_);
        if (vlcState == libvlc_Ended) {
          playNext();
        }
        playerCV_.wait_for(lock, chrono::milliseconds(100));
      }
    } else if (state_ == PAUSED) {
      // 等待直到被唤醒
      while (state_ == PAUSED) {
        playerCV_.wait(lock);
      }
    } else if (state_ == STOPPED) {
      // 停止播放并清空播放列表
      libvlc_media_player_stop(vlcMediaPlayer_);
      clearPlaylist();
    }
  }
}

void MusicPlayer::loadMusicFolder(const string& folderPath) {
  // 清空播放列表
  clearPlaylist();

  // 遍历目录下的音乐文件
  auto is_music = [](const auto& file) -> bool {
    auto ext = file.path().extension();
    return file.is_regular_file() &&
           (ext == ".mp3" || ext == ".wav" || ext == ".flac" || ext == ".aac" ||
            ext == ".ogg");
  };
  for (const auto& file : filesystem::directory_iterator(folderPath)) {
    if (is_music(file)) {
      // 添加音乐到播放列表
      addToPlaylist(file.path().string());
    }
  }
}

void MusicPlayer::toggleState() {
  switch (state_) {
    case STOPPED: {
      if (!playlist_.empty()) {
        playMusicAtIndex(0);
      }
      break;
    }
    case PLAYING: {
      stop();
      break;
    }
    case PAUSED: {
      play();
    }
  }
}

void MusicPlayer::togglePlay() {
  // unique_lock<mutex> lock(playerMutex_);

  if (state_ == STOPPED) {
    // 播放第一首歌曲
    if (!playlist_.empty()) {
      // cout << "------------ stopped -> playing\n";
      playMusicAtIndex(0);
      // state_ = PLAYING;
      // playerCV_.notify_all();
    }
  } else if (state_ == PLAYING) {
    // state_ = PAUSED;
    pause();
    // cout << "------------ playing -> pause\n";
    // playerCV_.notify_all();
  } else if (state_ == PAUSED) {
    // state_ = PLAYING;
    resume();
    // cout << "------------ pause -> resume\n";
    // playerCV_.notify_all();
  }
}

PlayMode MusicPlayer::getMode() const { return playMode_; }

void MusicPlayer::toggleMode() {
  playMode_ = static_cast<PlayMode>((playMode_ + 1) % (PlayMode::DEFAULT + 1));
}
void MusicPlayer::setMode(PlayMode playmode) { playMode_ = playmode; }

PlayerState MusicPlayer::getState() const { return state_; }

int MusicPlayer::getCurrentTime() const {
  if (vlcMediaPlayer_ != nullptr) {
    return libvlc_media_player_get_time(vlcMediaPlayer_) / 1000;
  }
  return 0;
}

int MusicPlayer::getTotalTime() const {
  if (vlcMediaPlayer_ != nullptr) {
    return libvlc_media_player_get_length(vlcMediaPlayer_) / 1000;
  }
  return 0;
}

string MusicPlayer::getCurrentMusicName() const {
  if (currentMusicIndex_ >= 0 && currentMusicIndex_ < playlist_.size()) {
    return playlist_[currentMusicIndex_].name;
  }
  return "";
}

int MusicPlayer::getRandomInt(size_t n) const {
  srand(time(NULL));  // 设置随机数种子
  return rand() % n;  // 生成随机数
}