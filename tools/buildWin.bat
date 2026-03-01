mkdir build && cd build && cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/Qt/6.9.0/mingw_64 -DQT_DIR=C:/Qt/6.9.0/mingw_64/lib/cmake/Qt6 -DCMAKE_C_COMPILER=C:/Qt/Tools/mingw1310_64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw1310_64/bin/g++.exe -DCMAKE_BUILD_TYPE=Debug ..
cd build && cmake --build . --config Debug
