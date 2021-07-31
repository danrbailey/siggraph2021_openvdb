
#include <openvdb/openvdb.h>
#include <openvdb/util/CpuTimer.h>

#include "../asset.h"
#include "../parse.h"

using namespace openvdb;

void leafIterRange(FloatTree& tree, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tree::IteratorRange<FloatTree::LeafCIter> iterRange(tree.cbeginLeaf());

        total += iterRange.test() ? 1 : 0;

        time += timer.milliseconds();

        if (total == 0.0f)     std::cerr << std::endl; // prevent optimization
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void nodeIterRange(FloatTree& tree, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tree::IteratorRange<FloatTree::NodeCIter> iterRange(tree.cbeginNode());

        total += iterRange.test() ? 1 : 0;

        time += timer.milliseconds();

        if (total == 0.0f)     std::cerr << std::endl; // prevent optimization
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void valueIterRange(FloatTree& tree, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tree::IteratorRange<FloatTree::ValueOnCIter> iterRange(tree.cbeginValueOn());

        total += iterRange.test() ? 1 : 0;

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

    std::cerr << "Cloud Leaf Iterator Range ...";
    leafIterRange(tree, iterations);

    std::cerr << "Cloud Node Iterator Range ...";
    nodeIterRange(tree, iterations);

    std::cerr << "Cloud Value Iterator Range ...";
    valueIterRange(tree, iterations);

    return 0;
}
