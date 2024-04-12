mkdir -p lib
cd lib
wget https://github.com/jtv/libpqxx/archive/refs/tags/7.9.0.tar.gz
tar -xvf 7.9.0.tar.gz
mv libpqxx-7.9.0 libpqxx
cd ..

# wget https://raw.githubusercontent.com/simdjson/simdjson/master/singleheader/simdjson.h https://raw.githubusercontent.com/simdjson/simdjson/master/singleheader/simdjson.cpp Consider using this instead of the following

mkdir -p build
cd build
cmake ..
make -j 16