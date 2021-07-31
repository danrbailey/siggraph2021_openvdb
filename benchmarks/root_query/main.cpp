
#include <openvdb/openvdb.h>
#include <openvdb/util/CpuTimer.h>

#include "../parse.h"

using namespace openvdb;

void addOneTile(FloatTree& tree)
{
    tree.addTile(0, Coord(0, 0, 0), 1.0f, true);
}

void addEightTiles(FloatTree& tree)
{
    for (int i = -4096; i <= 0; i += 4096) {
        for (int j = -4096; j <= 0; j += 4096) {
            for (int k = -4096; k <= 0; k += 4096) {
                tree.addTile(0, Coord(i, j, k), 1.0f, true);
            }
        }
    }
}

void addSixtyFourTiles(FloatTree& tree)
{
    for (int i = -(4096*2); i <= 4096; i += 4096) {
        for (int j = -(4096*2); j <= 4096; j += 4096) {
            for (int k = -(4096*2); k <= 4096; k += 4096) {
                tree.addTile(0, Coord(i, j, k), 1.0f, true);
            }
        }
    }
}

void addCoalescedIJKs(FloatTree& tree, std::vector<Coord>& ijks, size_t count)
{
    ijks.clear();
    ijks.reserve(8*count);

    for (int tileIndex = 0; tileIndex < 8; tileIndex++) {
        Int32 i(0);
        Int32 j(0);
        Int32 k(0);
        if ((tileIndex & 1) == 1)   i -= 4096;
        if ((tileIndex & 2) == 1)   j -= 4096;
        if ((tileIndex & 4) == 1)   k -= 4096;
        for (int n = 0; n < count; n++) {
            ijks.emplace_back(i, j, k);
        }
    }
}

void addInterleavedIJKs(FloatTree& tree, std::vector<Coord>& ijks, size_t count)
{
    ijks.clear();
    ijks.reserve(8*count);

    for (int n = 0; n < count; n++) {
        for (int tileIndex = 0; tileIndex < 8; tileIndex++) {
            Int32 i(0);
            Int32 j(0);
            Int32 k(0);
            if ((tileIndex & 1) == 1)   i -= 4096;
            if ((tileIndex & 2) == 1)   j -= 4096;
            if ((tileIndex & 4) == 1)   k -= 4096;
            ijks.emplace_back(i, j, k);
        }
    }
}

void warmup(FloatTree& tree, const std::vector<Coord>& ijks)
{
    int total = 0;
    auto& root = tree.root();
    for (const auto& ijk : ijks) {
        total += root.getValueDepth(ijk);
    }
    if (total == 0)     std::cerr << std::endl; // prevent optimization
}

void rootQueryDirect(FloatTree& tree, const std::vector<Coord>& ijks, int iterations)
{
    util::CpuTimer timer;
    auto& root = tree.root();
    int total = 0;
    double time = 0.0f;

    for (int i = 0; i < iterations; i++) {

        timer.start();

        for (const auto& ijk : ijks) {
            total += root.getValueDepth(ijk);
        }

        time += timer.milliseconds();

        if (total == 0)     std::cerr << std::endl; // prevent optimization
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void rootQueryAccessor(FloatTree& tree, const std::vector<Coord>& ijks, int iterations)
{
    util::CpuTimer timer;
    auto& root = tree.root();
    int total = 0;
    double time = 0.0f;

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tree::ValueAccessor<FloatTree> valueAccessor(tree);

        for (const auto& ijk : ijks) {
            total += valueAccessor.getValueDepth(ijk);
        }

        time += timer.milliseconds();

        if (total == 0)     std::cerr << std::endl; // prevent optimization
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

int
main(int argc, char *argv[])
{
    openvdb::initialize();

    OptParse parser(argc, argv, /*vdbArg=*/false, /*cpusArg=*/false);
    int iterations = parser.iterations();

    std::vector<Coord> ijks;
    size_t count = 100 * 1000 * 1000;

    {
        std::cerr << "1 Tile Coalesced Root Query Direct ...";
        FloatTree tree;
        addOneTile(tree);
        addCoalescedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryDirect(tree, ijks, iterations);
    }

    {
        std::cerr << "1 Tile Coalesced Root Query Accessor ...";
        FloatTree tree;
        addOneTile(tree);
        addCoalescedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryAccessor(tree, ijks, iterations);
    }

    {
        std::cerr << "1 Tile Interleaved Root Query Direct ...";
        FloatTree tree;
        addOneTile(tree);
        addInterleavedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryDirect(tree, ijks, iterations);
    }

    {
        std::cerr << "1 Tile Interleaved Root Query Accessor ...";
        FloatTree tree;
        addOneTile(tree);
        addInterleavedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryAccessor(tree, ijks, iterations);
    }

    {
        std::cerr << "8 Tiles Coalesced Root Query Direct ...";
        FloatTree tree;
        addEightTiles(tree);
        addCoalescedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryDirect(tree, ijks, iterations);
    }

    {
        std::cerr << "8 Tiles Coalesced Root Query Accessor ...";
        FloatTree tree;
        addEightTiles(tree);
        addCoalescedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryAccessor(tree, ijks, iterations);
    }

    {
        std::cerr << "8 Tiles Interleaved Root Query Direct ...";
        FloatTree tree;
        addEightTiles(tree);
        addInterleavedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryDirect(tree, ijks, iterations);
    }

    {
        std::cerr << "8 Tiles Interleaved Root Query Accessor ...";
        FloatTree tree;
        addEightTiles(tree);
        addInterleavedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryAccessor(tree, ijks, iterations);
    }

    {
        std::cerr << "64 Tiles Coalesced Root Query Direct ...";
        FloatTree tree;
        addSixtyFourTiles(tree);
        addCoalescedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryDirect(tree, ijks, iterations);
    }

    {
        std::cerr << "64 Tiles Coalesced Root Query Accessor ...";
        FloatTree tree;
        addSixtyFourTiles(tree);
        addCoalescedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryAccessor(tree, ijks, iterations);
    }

    {
        std::cerr << "64 Tiles Interleaved Root Query Direct ...";
        FloatTree tree;
        addSixtyFourTiles(tree);
        addInterleavedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryDirect(tree, ijks, iterations);
    }

    {
        std::cerr << "64 Tiles Interleaved Root Query Accessor ...";
        FloatTree tree;
        addSixtyFourTiles(tree);
        addInterleavedIJKs(tree, ijks, count);
        warmup(tree, ijks);
        rootQueryAccessor(tree, ijks, iterations);
    }

    return 0;
}
