/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */



#ifndef FunctionSpace_h
#define FunctionSpace_h

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "atlas/mpl/HaloExchange.h"
#include "atlas/mpl/GatherScatter.h"
#include "atlas/mpl/Checksum.h"
#include "atlas/mesh/Metadata.h"

namespace atlas {

class Mesh;
class Field;
template <typename T> class FieldT;

/// @todo
// Horizontal nodes are always the slowest moving index
// Then variables
// Then levels are fastest moving index
class FunctionSpace : private eckit::NonCopyable {

public: // methods

	FunctionSpace(const std::string& name, const std::string& shape_func, const std::vector<int>& shape );

	virtual ~FunctionSpace();

	const std::string& name() const { return name_; }

	int index() const { return idx_; }

	Field& field( size_t ) const;
	Field& field(const std::string& name) const;

	template< typename DATA_TYPE>
	FieldT<DATA_TYPE>& field(const std::string& name) const;

	bool has_field(const std::string& name) const { return index_.count(name); }

	template< typename DATA_TYPE >
	FieldT<DATA_TYPE>& create_field(const std::string& name, size_t nb_vars);

	void remove_field(const std::string& name);

	// This is a Fortran view of the shape (i.e. reverse order)
	const std::vector<int>& shapef() const { return shape_; }
	const std::vector<int>& shape() const { return shape_; }
	int shape(const int i) const { ASSERT(i<shape_.size()); return shape_[i]; }
	void resize( const std::vector<int>& shape );


	void parallelise();
	void parallelise(const int proc[], const int remote_idx[], const int glb_idx[], int size);
	void parallelise(FunctionSpace& other_functionspace);

	template< typename DATA_TYPE >
	void halo_exchange( DATA_TYPE field_data[], int field_size )
	{
		int nb_vars = field_size/dof_;
		if( dof_*nb_vars != field_size )
		{
			std::cout << "ERROR in FunctionSpace::halo_exchange" << std::endl;
			std::cout << "field_size = " << field_size << std::endl;
			std::cout << "dof_ = " << dof_ << std::endl;
		}
		halo_exchange_->execute( field_data, nb_vars );
	}

	template< typename DATA_TYPE >
	void gather( const DATA_TYPE field_data[], int field_size, DATA_TYPE glbfield_data[], int glbfield_size )
	{
		int nb_vars = field_size/dof_;
		if( dof_*nb_vars != field_size ) std::cout << "ERROR in FunctionSpace::gather" << std::endl;
		if( glb_dof_*nb_vars != glbfield_size ) std::cout << "ERROR in FunctionSpace::gather" << std::endl;

		mpl::Field<DATA_TYPE const> loc_field(field_data,nb_vars);
		mpl::Field<DATA_TYPE      > glb_field(glbfield_data,nb_vars);

		gather_scatter_->gather( &loc_field, &glb_field, 1 );
	}

	mpl::HaloExchange& halo_exchange() const { return *halo_exchange_; }

	mpl::GatherScatter& gather_scatter() const { return *gather_scatter_; }

	mpl::Checksum& checksum() const { return *checksum_; }

	void set_index(int idx) { idx_ = idx; }


	const Metadata& metadata() const { return metadata_; }
	Metadata& metadata() { return metadata_; }

	const Mesh& mesh() const { ASSERT( mesh_ ); return *mesh_; }
	Mesh& mesh() { ASSERT( mesh_ ); return *mesh_; }
	void mesh( Mesh& m ) { mesh_ = &m; }

	int nb_fields() const { return fields_.size(); }

	int dof() const { return dof_; }

	int glb_dof() const { return glb_dof_; }

protected: // members

	int idx_;
	int dof_;
	int glb_dof_;
	std::string name_;
	std::vector<int> shapef_; // deprecated, use shape which is reverse order
	std::vector<int> shape_;
	std::map< std::string, size_t > index_;
	std::vector< Field* > fields_;
	mpl::HaloExchange::Ptr  halo_exchange_;
	mpl::GatherScatter::Ptr gather_scatter_;
	mpl::Checksum::Ptr      checksum_;
	Metadata metadata_;
	Mesh*    mesh_;  ///< not owned, but may be null
};

typedef mpl::HaloExchange HaloExchange_t;
typedef mpl::GatherScatter GatherScatter_t;
typedef mpl::Checksum Checksum_t;

// ------------------------------------------------------------------
// C wrapper interfaces to C++ routines

extern "C"
{
	FunctionSpace* atlas__FunctionSpace__new (char* name, char* shape_func, int shape[], int rank);
	void atlas__FunctionSpace__delete (FunctionSpace* This);
	int atlas__FunctionSpace__dof (FunctionSpace* This);
	int atlas__FunctionSpace__glb_dof (FunctionSpace* This);
	void atlas__FunctionSpace__create_field_int (FunctionSpace* This, char* name, int nb_vars);
	void atlas__FunctionSpace__create_field_float (FunctionSpace* This, char* name, int nb_vars);
	void atlas__FunctionSpace__create_field_double (FunctionSpace* This, char* name, int nb_vars);
	void atlas__FunctionSpace__remove_field (FunctionSpace* This, char* name);
	int atlas__FunctionSpace__has_field (FunctionSpace* This, char* name);
	const char* atlas__FunctionSpace__name (FunctionSpace* This);
	void atlas__FunctionSpace__shapef (FunctionSpace* This, int* &shape, int &rank);
	Field* atlas__FunctionSpace__field (FunctionSpace* This, char* name);
	void atlas__FunctionSpace__parallelise (FunctionSpace* This);
	void atlas__FunctionSpace__halo_exchange_int (FunctionSpace* This, int field_data[], int field_size);
	void atlas__FunctionSpace__halo_exchange_float (FunctionSpace* This, float field_data[], int field_size);
	void atlas__FunctionSpace__halo_exchange_double (FunctionSpace* This, double field_data[], int field_size);
	void atlas__FunctionSpace__gather_int (FunctionSpace* This, int field_data[], int field_size, int glbfield_data[], int glbfield_size);
	void atlas__FunctionSpace__gather_float (FunctionSpace* This, float field_data[], int field_size, float glbfield_data[], int glbfield_size);
	void atlas__FunctionSpace__gather_double (FunctionSpace* This, double field_data[], int field_size, double glbfield_data[], int glbfield_size);
	HaloExchange_t* atlas__FunctionSpace__halo_exchange (FunctionSpace* This);
	GatherScatter_t* atlas__FunctionSpace__gather (FunctionSpace* This);
	Checksum_t* atlas__FunctionSpace__checksum (FunctionSpace* This);

}
// ------------------------------------------------------------------

} // namespace atlas

#endif // functionspace_h