# inc=""
# deps=""

# if [ ! -d "SDL" ] 
# then
#     git clone https://github.com/libsdl-org/SDL
#     pushd SDL
#     mkdir build
#     pushd build
#     cmake ../
#     cmake --build .
#     popd
#     popd
# fi
# deps="$deps `sdl2-config --cflags --libs`"
# clang main.c $deps -lX11  $inc -g -Wall -fno-exceptions -Wno-missing-braces -Wno-writable-strings

clang main.c -g -std=c99 -Wall -Wno-missing-braces -Wno-writable-strings -lX11 -lXfixes -lXext -lXcomposite -lm
