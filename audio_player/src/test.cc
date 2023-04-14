#include <iostream>
#include <string>

#include "MusicPlayer.h"

using namespace std;

int main(int argc, char** argv) {
  try {
    // 创建音乐播放器对象
    MusicPlayer player;

    // 添加音乐文件到播放列表中
    // player.addToPlaylist("./audio/1.mp3");
    // player.addToPlaylist("./audio/2.mp3");
    // player.addToPlaylist("./audio/3.mp3");
    string songsPath = "./";
    songsPath = argv[1];
    player.loadMusicFolder(songsPath);

    // 设置音量
    player.setVolume(80);

    // 播放音乐
    player.play();

    // 等待用户输入命令
    string command;
    while (cin >> command) {
      if (command == "next") {
        // 播放下一首音乐
        player.playNext();
      } else if (command == "prev") {
        // 播放上一首音乐
        player.playPrev();
      } else if (command == "pause") {
        // 暂停播放
        player.pause();
      } else if (command == "resume") {
        // 恢复播放
        player.resume();
      } else if (command == "stop") {
        // 停止播放
        player.stop();
      } else if (command == "volume") {
        // 调整音量
        int volume;
        cin >> volume;
        player.setVolume(volume);
      } else if (command == "playlist") {
        // 显示播放列表
        vector<Music> playlist = player.getPlaylist();
        for (const Music& music : playlist) {
          cout << music.name << endl;
        }
      } else if (command == "playing") {
        // 检查是否正在播放音乐
        bool isPlaying = player.isPlaying();
        if (isPlaying) {
          cout << "Music is playing." << endl;
        } else {
          cout << "Music is not playing." << endl;
        }
      } else if (command == "exit") {
        // 退出程序
        break;
      }
    }
  } catch (const exception& ex) {
    // 出现错误时输出错误信息
    cerr << "Error: " << ex.what() << endl;
    return 1;
  }

  return 0;
}