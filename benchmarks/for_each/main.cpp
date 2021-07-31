
#include <openvdb/openvdb.h>
#include <openvdb/util/CpuTimer.h>

#include <openvdb/tools/ValueTransformer.h>
#include <openvdb/tree/LeafManager.h>

#include <tbb/global_control.h>

#include "../asset.h"
#include "../parse.h"

using namespace openvdb;

struct DoubleOp
{
    template <typename T>
    bool operator()(T&, size_t = 0) const { return true; }

    bool operator()(FloatTree::LeafNodeType& leaf, size_t idx = 0) const
    {
        for (auto iter = leaf.beginValueOn(); iter; ++iter) {
            iter.setValue(iter.getValue() * 2);
        }
        return true;
    }
};

FloatTree copyTree(const FloatTree& refTree)
{
    FloatTree tree(refTree);

    // warm up

    float total = 0.0f;
    for (auto iter = tree.cbeginValueOn(); iter; ++iter) {
        total += iter.getValue();
    }
    if (total == 0.0f)     std::cerr << std::endl; // prevent optimization

    return tree;
}

void setValueSequentialLeaf(const FloatTree& refTree, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    FloatTree tree = copyTree(refTree);

    for (int i = 0; i < iterations; i++) {

        timer.start();

        for (auto leaf = tree.beginLeaf(); leaf; ++leaf) {
            for (auto iter = leaf->beginValueOn(); iter; ++iter) {
                iter.setValue(iter.getValue() * 2);
            }
        }

        time += timer.milliseconds();
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void setValueSequentialValue(const FloatTree& refTree, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    FloatTree tree = copyTree(refTree);

    for (int i = 0; i < iterations; i++) {

        timer.start();

        for (auto iter = tree.beginValueOn(); iter; ++iter) {
            iter.setValue(iter.getValue() * 2);
        }

        time += timer.milliseconds();
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void setValueForeachValue(const FloatTree& refTree, bool threaded, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    FloatTree tree = copyTree(refTree);

    auto op = [&](const auto& iter) {
        iter.setValue(iter.getValue() * 2);
    };

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tools::foreach(tree.beginValueOn(), op, threaded);

        time += timer.milliseconds();
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void setValueForeachLeaf(const FloatTree& refTree, bool threaded, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    FloatTree tree = copyTree(refTree);

    auto op = [&](const auto& leaf) {
        for (auto iter = leaf->beginValueOn(); iter; ++iter) {
            iter.setValue(iter.getValue() * 2);
        }
    };

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tools::foreach(tree.beginLeaf(), op, threaded);

        time += timer.milliseconds();
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void setValueForeachIterRange(const FloatTree& refTree, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    FloatTree tree = copyTree(refTree);

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tree::IteratorRange<FloatTree::ValueOnIter> iterRange(tree.beginValueOn());

        total += iterRange.test() ? 1 : 0;

        time += timer.milliseconds();

        if (total == 0.0f)     std::cerr << std::endl; // prevent optimization
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void setValueLeafManager(const FloatTree& refTree, bool threaded, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;
    float total = 0.0f;

    FloatTree tree = copyTree(refTree);

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tree::LeafManager<FloatTree> leafManager(tree);
        DoubleOp op;
        leafManager.foreach(op, threaded, /*grainSize=*/1);

        time += timer.milliseconds();
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void setValueNodeManager(const FloatTree& refTree, bool threaded, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;

    FloatTree tree = copyTree(refTree);

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tree::NodeManager<FloatTree> nodeManager(tree);
        DoubleOp op;
        nodeManager.foreachTopDown(op, threaded, /*grainSize=*/1);

        time += timer.milliseconds();
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}

void setValueDynamicNodeManager(const FloatTree& refTree, bool threaded, int iterations)
{
    util::CpuTimer timer;
    double time = 0.0f;

    FloatTree tree = copyTree(refTree);

    for (int i = 0; i < iterations; i++) {

        timer.start();

        tree::DynamicNodeManager<FloatTree> nodeManager(tree);
        DoubleOp op;
        nodeManager.foreachTopDown(op, threaded, /*grainSize=*/1);

        time += timer.milliseconds();
    }

    util::printTime(std::cerr, time/iterations, " completed in ", "\n", 4, 3, 1);
}


int
main(int argc, char *argv[])
{
    openvdb::initialize();

    OptParse parser(argc, argv, /*vdbArg=*/true, /*cpusArg=*/true);
    int iterations = parser.iterations();
    int cpus = parser.cpus();

    FloatTree tree = openVDBAsset(parser.vdb());

    std::cerr << "Cloud Set Value Sequential Value Iterator ...";
    setValueSequentialValue(tree, iterations);

    std::cerr << "Cloud Set Value Sequential Leaf Iterator ...";
    setValueSequentialLeaf(tree, iterations);

    std::cerr << "Cloud Set Value Foreach Value ...";
    setValueForeachValue(tree, false, iterations);

    for (int n = 1; n <= cpus; n *= 2) {
        std::cerr << "Cloud Set Value Foreach Value Thread" << n << " ...";
        tbb::global_control global_control(tbb::global_control::max_allowed_parallelism, n);
        setValueForeachValue(tree, true, iterations);
    }

    std::cerr << "Cloud Set Value Foreach Leaf ...";
    setValueForeachLeaf(tree, false, iterations);

    for (int n = 1; n <= cpus; n *= 2) {
        std::cerr << "Cloud Set Value Foreach Leaf Thread" << n << " ...";
        tbb::global_control global_control(tbb::global_control::max_allowed_parallelism, n);
        setValueForeachLeaf(tree, true, iterations);
    }

    std::cerr << "Cloud Set Value Foreach Iter Range ...";
    setValueForeachIterRange(tree, iterations);

    std::cerr << "Cloud Set Value LeafManager ...";
    setValueLeafManager(tree, false, iterations);

    for (int n = 1; n <= cpus; n *= 2) {
        std::cerr << "Cloud Set Value LeafManager Thread" << n << " ...";
        tbb::global_control global_control(tbb::global_control::max_allowed_parallelism, n);
        setValueLeafManager(tree, true, iterations);
    }

    std::cerr << "Cloud Set Value NodeManager ...";
    setValueNodeManager(tree, false, iterations);

    for (int n = 1; n <= cpus; n *= 2) {
        std::cerr << "Cloud Set Value NodeManager Thread" << n << " ...";
        tbb::global_control global_control(tbb::global_control::max_allowed_parallelism, n);
        setValueNodeManager(tree, true, iterations);
    }

    std::cerr << "Cloud Set Value DynamicNodeManager ...";
    setValueDynamicNodeManager(tree, false, iterations);

    for (int n = 1; n <= cpus; n *= 2) {
        std::cerr << "Cloud Set Value DynamicNodeManager Thread" << n << " ...";
        tbb::global_control global_control(tbb::global_control::max_allowed_parallelism, n);
        setValueDynamicNodeManager(tree, true, iterations);
    }

    return 0;
}
