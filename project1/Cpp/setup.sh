mkdir -p lib
cd lib
wget https://github.com/jtv/libpqxx/archive/refs/tags/7.9.0.tar.gz
tar -xvf 7.9.0.tar.gz
mv libpqxx-7.9.0 libpqxx
cd ..

mkdir -p build
cd build
cmake ..
make