#include "atlas/library/Library.h"
#include "atlas/grid/Grid.h"
#include "atlas/runtime/Log.h"

using atlas::Log;
using atlas::grid::StructuredGrid;

int main(int argc, char *argv[])
{
    atlas::Library::instance().initialise(argc, argv);

    StructuredGrid grid( "O32" );
    
    Log::info() << "nx first  = " << grid.nx().front() << std::endl;
    Log::info() << "ny        = " << grid.ny()         << std::endl;
    Log::info() << "npts      = " << grid.npts()       << std::endl;
    Log::info() << "xy        = " << grid.xy(0,0)      << std::endl;
    Log::info() << "lonlat    = " << grid.lonlat(0,0) << std::endl;

    atlas::Library::instance().finalise();
    return 0;
}



