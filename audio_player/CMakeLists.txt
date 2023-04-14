cmake_minimum_required(VERSION 3.0.0)
project(video_player VERSION 0.1.0)

set(CMAKE_BUILD_TYPE Debug)


add_executable(player src/ui_main.cc src/MusicPlayer.cc)
target_link_libraries(player vlc ftxui-component ftxui-dom ftxui-screen)
