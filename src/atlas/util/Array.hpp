/*
 * (C) Copyright 1996-2014 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */



#ifndef atlas_Array_hpp
#define atlas_Array_hpp

#include <vector>
#include "ArrayUtil.hpp"

//------------------------------------------------------------------------------------------------------

namespace atlas {

template< typename DATA_TYPE >
class Array {
public:
  Array() {}
  Array(int size) { resize( make_extents(size) ); }
  Array(int size1, int size2) { resize( make_extents(size1,size2) ); }
  Array(int size1, int size2, int size3) { resize( make_extents(size1,size2,size3) ); }
  Array(int size1, int size2, int size3, int size4) { resize( make_extents(size1,size2,size3,size4) ); }
  Array(const ArrayExtents& extents) { resize(extents); }
  
  void resize(const ArrayExtents& extents)
  {
    extents_= extents;
    strides_.resize(extents_.size());
    strides_[extents_.size()-1] = 1;
    for( int n=extents_.size()-2; n>=0; --n )
    {
      strides_[n] = strides_[n+1]*extents_[n+1];
    }
    std::size_t size = strides_[0]*extents_[0];
    data_.resize(size);
    rank_ = extents_.size();
  }
  const DATA_TYPE& operator[](int i) const { return *(data()+i); }
  DATA_TYPE&       operator[](int i)       { return *(data()+i); }
  std::size_t size() const { return data_.size(); }
  DATA_TYPE*       data() { return data_.data(); }
  const DATA_TYPE* data() const { return data_.data(); }
  int rank() const { return rank_; }
  int stride(int i) const { return strides_[i]; }
  int extent(int i) const { return extents_[i]; }
  const ArrayStrides& strides() const { return strides_; }
  const ArrayExtents& extents() const { return extents_; }
  void operator=(const DATA_TYPE& scalar) { for(int n=0; n<size(); ++n) data_[n]=scalar; }
  template< class InputIt >
  void assign( InputIt first, InputIt last ) { data_.assign(first,last); }
private:
  int rank_;
  ArrayExtents extents_;
  ArrayStrides strides_;
  std::vector<DATA_TYPE> data_;
};

//------------------------------------------------------------------------------------------------------

} // namespace atlas

#endif