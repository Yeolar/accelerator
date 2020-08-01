mkdir -p _build && cd _build
cmake ..
make -j8
make DESTDIR=../_deps install
