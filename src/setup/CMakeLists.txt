add_library(setup STATIC
            accessibility.cpp     accessibility.hpp
            compatibility.cpp     compatibility.hpp
            display.cpp           display.hpp
            joystick.cpp          joystick.hpp
            keyboard.cpp          keyboard.hpp
            mainmenu.cpp
            mode.cpp              mode.hpp
            mouse.cpp             mouse.hpp
            multiplayer.cpp       multiplayer.hpp
            sound.cpp             sound.hpp
            execute.cpp           execute.hpp
            txt_joyaxis.cpp       txt_joyaxis.hpp
            txt_joybinput.cpp     txt_joybinput.hpp
            txt_keyinput.cpp      txt_keyinput.hpp
            txt_mouseinput.cpp    txt_mouseinput.hpp)

target_include_directories(setup PRIVATE "../" "${CMAKE_CURRENT_BINARY_DIR}/../../")
target_link_libraries(setup textscreen SDL2::SDL2)
if(ENABLE_SDL2_MIXER)
    target_link_libraries(setup SDL2_mixer::SDL2_mixer)
endif()
