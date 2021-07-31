
#include <openvdb/openvdb.h>
#include <openvdb/util/CpuTimer.h>

#include "../asset.h"
#include "../parse.h"

using namespace openvdb;

void getValueSequentialLeaf(const FloatTree& tree, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    for (int i = 0; i < iterations; i++) {

        timer.start();

        for (auto leaf = tree.cbeginLeaf(); leaf; ++leaf) {
            for (auto iter = leaf->cbeginValueOn(); iter; ++iter) {
                total += iter.getValue();
            }
        }

        time += timer.milliseconds();

        if (total == 0.0f)     std::cerr << std::endl; // prevent optimization
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void getValueSequentialChild(const FloatTree& tree, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    for (int i = 0; i < iterations; i++) {

        timer.start();

        for (auto iter1 = tree.cbeginRootChildren(); iter1; ++iter1) {
            for (auto iter2 = iter1->cbeginChildOn(); iter2; ++iter2) {
                for (auto iter3 = iter2->cbeginChildOn(); iter3; ++iter3) {
                    for (auto iter4 = iter3->cbeginValueOn(); iter4; ++iter4) {
                        total += iter4.getValue();
                    }
                }
            }
        }

        time += timer.milliseconds();

        if (total == 0.0f)     std::cerr << std::endl; // prevent optimization
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void getValueSequentialValue(const FloatTree& tree, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    for (int i = 0; i < iterations; i++) {

        timer.start();

        for (auto iter = tree.cbeginValueOn(); iter; ++iter) {
            total += iter.getValue();
        }

        time += timer.milliseconds();

        if (total == 0.0f)     std::cerr << std::endl; // prevent optimization
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}


int
main(int argc, char *argv[])
{
    openvdb::initialize();

    OptParse parser(argc, argv, /*vdbArg=*/true, /*cpusArg=*/false);
    int iterations = parser.iterations();

    FloatTree tree = openVDBAsset(parser.vdb());

    std::cerr << "Cloud Get Value Sequential Leaf Iterator ...";
    getValueSequentialLeaf(tree, iterations);

    std::cerr << "Cloud Get Value Sequential Hierarchy Iterator ...";
    getValueSequentialChild(tree, iterations);

    std::cerr << "Cloud Get Value Sequential Voxel Iterator ...";
    getValueSequentialValue(tree, iterations);

    return 0;
}
