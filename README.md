This is a set of benchmarks for use with OpenVDB as presented in the [Siggraph 2021 OpenVDB Course](https://s2021.siggraph.org/presentation/?id=gensub_281&sess=sess155).

### Quick Start

1) Build and install the core OpenVDB library

2) Build the benchmarks, making sure to set the CMAKE_MODULE_PATH to point to the location of the installed OpenVDB cmake config files (FindOpenVDB.cmake is an example).

```
mkdir build
cd build
cmake \
	-DTBB_INCLUDEDIR=/usr/include \
	-DTBB_LIBRARYDIR=/usr/lib \
	-DBLOSC_INCLUDEDIR=/usr/include \
	-DBLOSC_LIBRARYDIR=/usr/lib \
	-DCMAKE_MODULE_PATH=/usr/lib/cmake/OpenVDB \
	..
make -j32
```

3) Run each benchmark

```
./benchmarks/for_each -vdb /tmp/wdas_cloud.vdb -iterations 10 -cpus 32
```

This benchmark takes optional arguments of the path to a VDB to use for the test, a set of repeat iterations to perform (more iterations means more accurate results) and how many cpus to use which defaults to the number of logical cores if not set.

Call ./benchmarks/for_each -help for a complete list of options.
