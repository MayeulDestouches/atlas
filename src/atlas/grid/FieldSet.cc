/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/grib/GribFieldSet.h"
#include "eckit/grib/GribField.h"
#include "eckit/grib/GribHandle.h"
#include "eckit/grib/GribAccessor.h"
#include "eckit/grib/GribParams.h"
#include "eckit/utils/Translator.h"

#include "atlas/mesh/Parameters.hpp"
#include "atlas/util/ArrayView.hpp"
#include "atlas/mesh/FunctionSpace.hpp"
#include "atlas/grid/FieldSet.h"

//------------------------------------------------------------------------------------------------------

using namespace eckit;
using namespace eckit::grib;

namespace atlas {
namespace grid {

//------------------------------------------------------------------------------------------------------

FieldHandle::FieldHandle( Grid::Ptr g, Data& d ) :
    grid_(g),
    data_(d)
{
    ASSERT( grid_ );
}

void FieldHandle::print(std::ostream& out) const
{
    FunctionSpace& nodes = data_.function_space();

    ArrayView<double,1> values( data_ );

//    ArrayView<double,2> coords( nodes.field("coordinates") );
    ArrayView<double,2> latlon( nodes.field("latlon") );

    ASSERT( values.extents()[0] == latlon.extents()[0] );

//    Log::info() << "values.extents()[0] " << values.extents()[0] << std::endl;

    for( size_t i = 0; i < latlon.extents()[0]; ++i )
        out << latlon(i,LAT) << " " << latlon(i,LON) << " " << values(i) << std::endl;
}

std::ostream& operator<<( std::ostream& os, const FieldHandle& f)
{
    f.print(os);
    return os;
}

//-----------------------------------------------------------------------------

static GribAccessor<std::string> grib_shortName("shortName");


FieldHandle::Ptr FieldSet::create_field( GribHandle& gh )
{
	if( !grid_ ) // first time create grid
    {
		GribParams* gp = GribParams::create(gh);
		ASSERT( gp );
		grid_.reset( Grid::create( *gp ) );
    }
	else // check grid is the same
    {
		if( grib_geography_hash(gh.raw()) != grid_->hash() )
            throw eckit::UserError("GRIB fields don't match grid within FieldSet", Here() );
    }

    Mesh& mesh = grid_->mesh();
    FunctionSpace&  nodes  = mesh.function_space( "nodes" );

    // get name for this field
    std::string sname = grib_shortName( gh.raw() ) + "_" + Translator<size_t,std::string>()( fields_.size() );

    // get values

    size_t nvalues = gh.getDataValuesSize();

    // create the field

    if( nodes.extents()[0] != nvalues )
        throw SeriousBug( "Size of field in GRIB does not match Grid", Here() );

    FieldHandle::Data& fdata = nodes.create_field<double>(sname,1);

    gh.getDataValues(fdata.data(),nvalues);

    FieldHandle::Ptr hf( new FieldHandle( grid_, fdata ) );

    hf->grib( gh.clone() );

//        {
//            std::ofstream of;
//            of.open( sname.c_str() );
//            of << *hf << std::endl;
//            of.close();
//        }

    return hf;
}

//-----------------------------------------------------------------------------

FieldSet::FieldSet( const eckit::PathName& fname )
{
    ASSERT( !grid_ );

    GribFieldSet gribfs(fname);

    if( gribfs.size() == 0 ) return;

    fields_.reserve( gribfs.size() );

    size_t check_nvalues = 0;

    for( size_t fidx = 0; fidx < gribfs.size(); ++fidx )
    {
        GribField* gf = const_cast<GribField*>( gribfs.get(fidx) );
        ASSERT( gf );

        GribHandle* gh = gf->getHandle();
        ASSERT( gh );

        fields_.push_back( create_field(*gh) );

        /* check all fields have same nvalues */
        if( !check_nvalues ) check_nvalues = fields_.back()->data().size();
        if( check_nvalues != fields_.back()->data().size() )
            throw eckit::UserError("GRIB file contains multiple fields with different sizes", Here() );

        gf->release(); // free this GribField
    }
}

FieldSet::FieldSet( const Buffer& buf )
{
    GribHandle gh( buf );
    fields_.push_back( create_field(gh) );
}

FieldSet::FieldSet(const Grid::Ptr grid, std::vector<std::string> nfields )
{
    ASSERT( grid );

    grid_.reset(grid);

    Mesh& mesh = grid_->mesh();
    FunctionSpace& nodes = mesh.function_space( "nodes" );
    fields_.reserve(nfields.size());
    for( size_t i = 0; i < nfields.size(); ++i )
    {
        FieldHandle::Data& fdata = nodes.create_field<double>(nfields[i],1);
        FieldHandle::Ptr hf( new FieldHandle( grid, fdata ) );
        fields_.push_back( hf );
    }
}

FieldSet::FieldSet(const FieldHandle::Vector& fields) :  fields_(fields)
{
    for( size_t i = 0; i < fields_.size(); ++i )
    {
        if( !grid_ )
            grid_.reset( &(fields_[i]->grid()) );     // first time create grid
        else                                          // then check is all the same
            if( fields_[i]->grid().hash() != grid_->hash() )
                throw eckit::UserError("list of fields don't match the same grid to build a FieldSet", Here() );

    }
}

std::vector<std::string> FieldSet::field_names() const
{
    std::vector<std::string> ret;
    ret.reserve(fields_.size());

    for( size_t i = 0; i < fields_.size(); ++i )
        ret.push_back( fields_[i]->data().name() );

    return ret;
}

void FieldHandle::grib(FieldHandle::Grib *g)
{
    grib_.reset(g);
}

FieldHandle::Grib& FieldHandle::grib() const
{
    return *grib_;
}

//-----------------------------------------------------------------------------

} // namespace grid
} // namespace eckit

