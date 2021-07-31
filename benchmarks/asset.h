
#include <openvdb/openvdb.h>

using namespace openvdb;

FloatTree openVDBAsset(const std::string& filepath)
{
    // open the VDB and extract the first grid

    io::File file(filepath);
    file.open();
    auto grids = file.getGrids();
    file.close();
    auto gridBase = (*grids)[0];
    auto grid = GridBase::grid<FloatGrid>(gridBase);

    // create a new tree and voxelize all active tiles

    FloatTree tree(grid->tree());
    tree.voxelizeActiveTiles();

    // warm up

    float total = 0.0f;
    for (auto iter = tree.cbeginValueOn(); iter; ++iter) {
        total += iter.getValue();
    }
    if (total == 0.0f)     std::cerr << std::endl; // prevent optimization

    return tree;
}
