#include <chrono>  // for operator""s, chrono_literals
#include <fstream>
#include <iostream>
#include <map>
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for operator+, to_string
#include <thread>

#include "MusicPlayer.h"
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, gauge, text, Element, operator|, vbox, border
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/color.hpp"   // for ftxui
#include "ftxui/screen/screen.hpp"  // for Screen

using namespace ftxui;

// Define a special style for some menu entry.
MenuEntryOption Colored(ftxui::Color c) {
  MenuEntryOption option;
  option.transform = [c](EntryState state) {
    state.label = (state.active ? "> " : "  ") + state.label;
    Element e = text(state.label) | color(c);
    if (state.focused) e = e | inverted;
    if (state.active) e = e | bold;
    return e;
  };
  return option;
}

RadioboxOption SetMode(MusicPlayer& player, PlayMode mode) {
  RadioboxOption option;
  option.on_change = [&player, &mode]() { player.setMode(mode); };
  return option;
}

Component makePlaylist(MusicPlayer& player) {
  int selected = player.getCurrentIndex();

  vector<Music> songs = player.getPlaylist();
  vector<Component> children_components;
  for (const Music& song : songs) {
    string song_info = song.name;
    children_components.push_back(MenuEntry(song_info));
  }
  Component menu = Container::Vertical(children_components, &selected);

  Component renderer = Renderer(menu, [&] {
    return vbox({
               hbox(text("selected = "), text(std::to_string(selected) +
                                              player.getCurrentMusicName())),
               separator(),
               menu->Render() | frame | size(HEIGHT, LESS_THAN, 10),
           }) |
           border;
  });

  return renderer;
}

int main(int argc, const char* argv[]) {
  MusicPlayer player;
  if (argc < 2) {
    std::cout << "usage: ./player music_folder_path\n";
    return 0;
  }
  string songsPath = "../MusicB";
  songsPath = argv[1];
  player.loadMusicFolder(songsPath);

  /* -------------------------- 控制歌曲面板 ----------------------------- */
  // 1. 播放，暂停，终止按钮；
  // 2. 音量调节 slider
  // 3. 播放模式切换

  // 播放，暂停，终止按钮；
  string current_song_name = "";
  Component buttons = Container::Horizontal({
      Button("▶",
             [&player, &current_song_name] {
               //  player.toggleState();
               player.togglePlay();
               current_song_name = player.getCurrentMusicName();
             }),
      Button("■",
             [&player, &current_song_name] {
               //  player.toggleState();
               player.stop();
               current_song_name = "";
             }),
      Button("▶|",
             [&player, &current_song_name] {
               //  player.toggleState();
               player.playNext();
               current_song_name = player.getCurrentMusicName();
             }),
  });

  // 音量调节
  int value = player.getVolume();
  SliderOption<int> slider_option;
  slider_option.value = &value;
  slider_option.max = 100;
  slider_option.min = 0;
  slider_option.increment = 5;
  slider_option.direction = Direction::Up;
  // auto volume_slider = Slider("Volume", &value, 0, 100, 1);
  auto volume_slider = Slider(slider_option);
  //  size(WIDTH, EQUAL, 25) |
  //  size(HEIGHT, EQUAL, 1);  // 音量条
  auto volume_renderer = Renderer(volume_slider, [&] {
    player.setVolume(value);
    return vbox({
               volume_slider->Render(),
               text("v"),
           }) |
           border;
  });
  // 播放模式
  int mode_selected = 0;
  vector<string> modes = {"🔀", "🔂", "🔁", "🔚"};
  vector<PlayMode> modes_map = {PlayMode::RANDOM, PlayMode::REPEAT,
                                PlayMode::LOOP, PlayMode::DEFAULT};
  auto mode_radio_box = Radiobox(&modes, &mode_selected);
  auto mode_renderer = Renderer(mode_radio_box, [&] {
    player.setMode(modes_map[mode_selected]);
    return mode_radio_box->Render() | flex;
  });

  auto song_control_panel = Container::Horizontal({
      buttons,          // 播放暂停终止 按钮
      volume_renderer,  // 音量条
      mode_renderer,    // 播放模式选择
  });

  Component song_control_panel_renderer = Renderer(song_control_panel, [&] {
    // string name =
    //     player.isPlaying() ? player.getCurrentMusicName() : "无正在播放歌曲";
    return hbox({
               border(
                   frame(text(current_song_name.empty() ? "无正在播放歌曲"
                                                        : current_song_name))),
               //  text("value = " + std::to_string(value)),
               //  separator(),
               //  gauge(value * 0.01f),
               //  separator(),
               song_control_panel->Render(),
               //  buttons->Render(), volume_slider->Render() | flex,
               //  mode_radio_box->Render(),
           }) |
           border;
  });
  /* ------------------------------------------------------------------------ */

  /* ------------------------------- 播放列表 ------------------------------- */
  int selected = player.getCurrentIndex();

  vector<Music> songs = player.getPlaylist();
  vector<string> info_list;
  for (const Music& song : songs) {
    string song_info = song.name;
    info_list.push_back(song_info);
  }

  MenuOption menu_option = MenuOption::Vertical();
  menu_option.on_enter = [&player, &selected, &current_song_name]() {
    player.playMusicAtIndex(selected);
    current_song_name = player.getCurrentMusicName();
  };

  menu_option.entries = Colored(Color::Cyan);
  Component menu = Menu(&info_list, &selected, menu_option);
  // menu |= size(HEIGHT, LESS_THAN, 10);
  // menu |= size(WIDTH, LESS_THAN, 50);
  Component menu_renderer = Renderer(menu, [&] {
    return vbox({
               hbox(text("selected = "), text(songs[selected].name)) |
                   size(WIDTH, LESS_THAN, 50),
               separator(),
               menu->Render() | vscroll_indicator | frame | border |
                   size(HEIGHT, EQUAL, 20) | size(WIDTH, EQUAL, 50),
           }) |
           border;
  });
  /* ------------------------------------------------------------------- */

  /* -------------------------- 音浪歌曲进度条 ------------------------------*/
  // 音浪和进度条一齐渲染
  using namespace std::chrono_literals;
  auto convert_time_func = [](int duration) {
    string minutes = to_string(duration / 60);
    string seconds = to_string(duration - duration / 60 * 60);

    string res = (minutes.size() == 2 ? minutes : ("0" + minutes)) + ":" +
                 (seconds.size() == 2 ? seconds : ("0" + seconds));
    return res;
  };

  // std::vector<Element> pitch_bars(10);

  int duration = player.getTotalTime();  // 单位 s
  string finish = convert_time_func(duration);
  string start = "0:00";

  // `current_time` be updated in refresh_ui loop.
  int current_time = player.getCurrentTime();
  // float percentage = current_time * 1.0f / duration;
  // Element progress_element = hbox({
  //     text(start),
  //     gauge(current_time * 1.0f / duration) | flex,
  //     text(convert_time_func(current_time) + "/" + finish),
  // });

  vector<Element> current_pitch_bars(10);

  Component wave_and_progress = Renderer([&] {
    current_time = player.getCurrentTime();
    finish = convert_time_func(player.getTotalTime());
    duration = player.getTotalTime();

    // 随机生成 current_pitch_bars 模拟某时刻音浪效果
    const int PITCHHEIGHT = 15;
    srand(time(NULL));
    for (int i = 0; i < 10; ++i) {
      float pitch_height = player.isPlaying() ? rand() * 1.0f / RAND_MAX : 0;
      auto gauge_up = hbox({
                          gaugeUp(pitch_height) | border | color(Color::Blue),
                      }) |
                      flex;
      current_pitch_bars[i] = vbox({
          // separator(),
          gauge_up | size(WIDTH, EQUAL, 5) | size(HEIGHT, EQUAL, PITCHHEIGHT),
      });  // 单个 bar 长条形往上增长
    };
    const int BAR_WIDTH = 50;
    return vbox({
        hbox(current_pitch_bars) | size(WIDTH, EQUAL, BAR_WIDTH) |
            border,  // 音浪
        filler(),
        hbox({
            text(start),
            gauge(current_time * 1.0f / duration) | color(Color::BlueLight),
            text(convert_time_func(current_time) + "/" + finish),
        }) | size(WIDTH, EQUAL, BAR_WIDTH),  // 播放进度条,
    });
  });

  // song_panel: 歌曲控制面板 + 音浪_进度条
  Component song_panel = Renderer(song_control_panel_renderer, [&] {
    return vbox({
        wave_and_progress->Render(),
        song_control_panel_renderer->Render(),
    });
  });
  /* --------------------------------------------------------------------*/

  Component global_container = Container::Horizontal({
      menu_renderer,
      song_panel,
  });

  Component global_renderer = Renderer(global_container, [&] {
    return vbox({
        text("Music Player") | bold | hcenter | border,
        hbox({
            menu_renderer->Render(),
            song_panel->Render(),
        }),
    });
  });

  auto screen = ScreenInteractive::FitComponent();

  std::atomic<bool> refresh_ui_continue = true;
  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.001s);

      screen.Post([&] {
        // current_time = player.getCurrentTime();
        // duration = player.getTotalTime();
      });

      // After updating the state, request a new frame to be drawn. This is done
      // by simulating a new "custom" event to be handled.
      screen.Post(Event::Custom);
    }
  });

  screen.Loop(global_renderer);
  refresh_ui_continue = false;
  refresh_ui.join();

  return 0;
}