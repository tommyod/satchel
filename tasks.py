import invoke

invoke.run(

    "c++ -O3 -shared -std=gnu++11 -I ./pybind11/include "
    "`python3-config --cflags --ldflags --libs` pybind11_wrapper.cpp -o example.so -fPIC"

)