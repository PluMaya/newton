cd ~/newton
mkdir build
cd build
cmake .. -DBOOST_ROOT=$HOME/boost_install
make -j