
#include <openvdb/openvdb.h>
#include <openvdb/util/CpuTimer.h>

#include "../asset.h"
#include "../parse.h"

using namespace openvdb;

void addSequentialVoxelIJKs(const FloatTree& tree, std::vector<Coord>& ijks)
{
    ijks.clear();

    for (auto leaf = tree.cbeginLeaf(); leaf; ++leaf) {
        for (auto iter = leaf->cbeginValueOn(); iter; ++iter) {
            ijks.emplace_back(iter.getCoord());
        }
    }
}

void addInterleavedVoxelIJKs(const FloatTree& tree, std::vector<Coord>& ijks)
{
    ijks.clear();

    auto& root = tree.root();

    std::vector<std::vector<Coord>> ijksRootChildren;
    ijksRootChildren.resize(root.childCount());

    size_t i = 0;
    for (auto iter1 = tree.cbeginRootChildren(); iter1; ++iter1) {
        auto& ijksRootChild = ijksRootChildren[i++];
        for (auto iter2 = iter1->cbeginChildOn(); iter2; ++iter2) {
            for (auto iter3 = iter2->cbeginChildOn(); iter3; ++iter3) {
                for (auto iter4 = iter3->cbeginValueOn(); iter4; ++iter4) {
                    ijksRootChild.push_back(iter3.getCoord());
                }
            }
        }
    }

    size_t maxSize = 0;
    for (const auto& ijksRootChild : ijksRootChildren) {
        maxSize = std::max(maxSize, ijksRootChild.size());
    }

    for (i = 0; i < maxSize; i++) {
        for (const auto& ijksRootChild : ijksRootChildren) {
            if (i < ijksRootChild.size()) {
                ijks.push_back(ijksRootChild[i]);
            }
        }
    }
}

void getValueDirect(const FloatTree& tree, const std::vector<Coord>& ijks, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0;

    const auto& root = tree.root();

    for (int i = 0; i < iterations; i++) {

        timer.start();

        for (const auto& ijk : ijks) {
            total += root.getValue(ijk);
        }

        time += timer.milliseconds();

        if (total == 0.0f)     std::cerr << std::endl; // prevent optimization
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void getValueAccessor(const FloatTree& tree, const std::vector<Coord>& ijks, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0;

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tree::ValueAccessor<const FloatTree> valueAccessor(tree);

        for (const auto& ijk : ijks) {
            total += valueAccessor.getValue(ijk);
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

    std::vector<Coord> ijks;

    std::cerr << "Cloud Get Value Sequential Direct ...";
    addSequentialVoxelIJKs(tree, ijks);
    getValueDirect(tree, ijks, iterations);

    std::cerr << "Cloud Get Value Sequential Accessor ...";
    addSequentialVoxelIJKs(tree, ijks);
    getValueAccessor(tree, ijks, iterations);

    std::cerr << "Cloud Get Value Interleaved Direct ...";
    addInterleavedVoxelIJKs(tree, ijks);
    getValueDirect(tree, ijks, iterations);

    std::cerr << "Cloud Get Value Interleaved Accessor ...";
    addInterleavedVoxelIJKs(tree, ijks);
    getValueAccessor(tree, ijks, iterations);

    return 0;
}
