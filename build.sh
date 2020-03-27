rm -rf build/*
cd build && cmake .. && make -j4 && sudo make install && pkill dde-dock && dde-dock
