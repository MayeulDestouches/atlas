/*
 * (C) Copyright 1996-2014 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>
#include <algorithm>

#define BOOST_TEST_MODULE TestHaloExchange
#define BOOST_UNIT_TEST_FRAMEWORK_HEADER_ONLY
#include "ecbuild/boost_test_framework.h"

#include "atlas/mpl/MPL.hpp"
#include "atlas/atlas_config.h"
#include "atlas/util/Array.hpp"
#include "atlas/util/ArrayView.hpp"
#include "atlas/util/IndexView.hpp"
#include "atlas/mpl/HaloExchange.hpp"

/// POD: Type to test
typedef double POD;

namespace atlas {
namespace test {
template<typename T, size_t N>
std::vector<T> vec( const T (&list)[N] )
{
  return std::vector<T>(list,list+N);
}

}
}

using namespace atlas;
using namespace atlas::test;

struct MPIFixture {
    MPIFixture()  { MPL::init(); }
    ~MPIFixture() { MPL::finalize(); }
};

struct Fixture {
  Fixture()
  {
    int nnodes_c[] = {5, 6, 7}; nb_nodes = vec(nnodes_c);
    N = nb_nodes[MPL::rank()];
    switch( MPL::rank() )
    {
      case 0:
      {
        int part_c[] = {2,0,0,0,1};  part = vec(part_c);
        int ridx_c[] = {4,1,2,3,1};  ridx = vec(ridx_c);
        POD gidx_c[] = {0,1,2,3,0};  gidx = vec(gidx_c);
        break;
      }
      case 1:
      {
        int part_c[] = {0,1,1,1,2,2};  part = vec(part_c);
        int ridx_c[] = {3,1,2,3,2,3};  ridx = vec(ridx_c);
        POD gidx_c[] = {0,4,5,6,0,0};  gidx = vec(gidx_c);
        break;
      }
      case 2:
      {
        int part_c[] = {1,1,2,2,2,0,0};  part = vec(part_c);
        int ridx_c[] = {2,3,2,3,4,1,2};  ridx = vec(ridx_c);
        POD gidx_c[] = {0,0,7,8,9,0,0};  gidx = vec(gidx_c);
        break;
      }
    }
    halo_exchange.setup(part.data(),ridx.data(),0,N);
  }
  mpl::HaloExchange halo_exchange;
  std::vector<int> nb_nodes;
  std::vector<int> part;
  std::vector<int> ridx;
  std::vector<POD>   gidx;

  int N;
};


BOOST_GLOBAL_FIXTURE( MPIFixture )

BOOST_FIXTURE_TEST_CASE( test_rank0, Fixture )
{
  int strides[] = {1};
  int extents[] = {1};
  halo_exchange.execute(gidx.data(),strides,extents,1);

  switch( MPL::rank() )
  {
    case 0: { POD gidx_c[] = { 9, 1, 2, 3, 4};
      BOOST_CHECK_EQUAL_COLLECTIONS(gidx.begin(),gidx.end(), gidx_c,gidx_c+N); break; }
    case 1: { POD gidx_c[] = { 3, 4, 5, 6, 7, 8};
      BOOST_CHECK_EQUAL_COLLECTIONS(gidx.begin(),gidx.end(), gidx_c,gidx_c+N); break; }
    case 2: { POD gidx_c[] = { 5, 6, 7, 8, 9, 1, 2};
      BOOST_CHECK_EQUAL_COLLECTIONS(gidx.begin(),gidx.end(), gidx_c,gidx_c+N); break; }
  }
}

BOOST_FIXTURE_TEST_CASE( test_rank1, Fixture )
{
  Array<POD> arr(N,2);
  ArrayView<POD,2> arrv(arr);
  for( int j=0; j<N; ++j ) {
    arrv(j,0) = (part[j]!= MPL::rank() ? 0 : gidx[j]*10 );
    arrv(j,1) = (part[j]!= MPL::rank() ? 0 : gidx[j]*100);
  }

  int strides[] = {1};
  int extents[] = {2};
  halo_exchange.execute(arr.data(),strides,extents,1);

  switch( MPL::rank() )
  {
    case 0: { POD arr_c[] = { 90,900, 10,100, 20,200, 30,300, 40,400 };
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
    case 1: { POD arr_c[] = { 30,300, 40,400, 50,500, 60,600, 70,700, 80,800};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
    case 2: { POD arr_c[] = { 50,500, 60,600, 70,700, 80,800, 90,900, 10,100, 20,200};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
  }
}

BOOST_FIXTURE_TEST_CASE( test_rank1_strided_v1, Fixture )
{
  Array<POD> arr(N,2);
  ArrayView<POD,2> arrv(arr);
  for( int j=0; j<N; ++j ) {
    arrv(j,0) = (part[j]!= MPL::rank() ? 0 : gidx[j]*10 );
    arrv(j,1) = (part[j]!= MPL::rank() ? 0 : gidx[j]*100);
  }

  int strides[] = {2};
  int extents[] = {1};
  halo_exchange.execute(&arrv(0,0),strides,extents,1);

  switch( MPL::rank() )
  {
    case 0: { POD arr_c[] = { 90,0, 10,100, 20,200, 30,300, 40,0 };
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
    case 1: { POD arr_c[] = { 30,0, 40,400, 50,500, 60,600, 70,0, 80,0};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
    case 2: { POD arr_c[] = { 50,0, 60,0, 70,700, 80,800, 90,900, 10,0, 20,0};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
  }
}

BOOST_FIXTURE_TEST_CASE( test_rank1_strided_v2, Fixture )
{
  Array<POD> arr(N,2);
  ArrayView<POD,2> arrv(arr);
  for( int j=0; j<N; ++j ) {
    arrv(j,0) = (part[j]!= MPL::rank() ? 0 : gidx[j]*10 );
    arrv(j,1) = (part[j]!= MPL::rank() ? 0 : gidx[j]*100);
  }

  int strides[] = {2};
  int extents[] = {1};
  halo_exchange.execute(&arrv(0,1),strides,extents,1);

  switch( MPL::rank() )
  {
    case 0: { POD arr_c[] = { 0,900, 10,100, 20,200, 30,300, 0,400 };
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
    case 1: { POD arr_c[] = { 0,300, 40,400, 50,500, 60,600, 0,700, 0,800};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
    case 2: { POD arr_c[] = { 0,500, 0,600, 70,700, 80,800, 90,900, 0,100, 0,200};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
  }
}

BOOST_FIXTURE_TEST_CASE( test_rank2, Fixture )
{
  Array<POD> arr(N,3,2);
  ArrayView<POD,3> arrv(arr);
  for( int p=0; p<N; ++p )
  {
    for( int i=0; i<3; ++i )
    {
      arrv(p,i,0) = (part[p]!= MPL::rank() ? 0 : -gidx[p]*std::pow(10,i) );
      arrv(p,i,1) = (part[p]!= MPL::rank() ? 0 :  gidx[p]*std::pow(10,i) );
    }
  }

  int strides[] = {2,1};
  int extents[] = {3,2};
  halo_exchange.execute(arr.data(),strides,extents,2);

  switch( MPL::rank() )
  {
    case 0:
    {
      int arr_c[] = { -9,9, -90,90, -900,900,
                      -1,1, -10,10, -100,100,
                      -2,2, -20,20, -200,200,
                      -3,3, -30,30, -300,300,
                      -4,4, -40,40, -400,400};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 1:
    {
      int arr_c[] = { -3,3, -30,30, -300,300,
                      -4,4, -40,40, -400,400,
                      -5,5, -50,50, -500,500,
                      -6,6, -60,60, -600,600,
                      -7,7, -70,70, -700,700,
                      -8,8, -80,80, -800,800};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 2:
    {
      int arr_c[] = { -5,5, -50,50, -500,500,
                      -6,6, -60,60, -600,600,
                      -7,7, -70,70, -700,700,
                      -8,8, -80,80, -800,800,
                      -9,9, -90,90, -900,900,
                      -1,1, -10,10, -100,100,
                      -2,2, -20,20, -200,200};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
  }
}

BOOST_FIXTURE_TEST_CASE( test_rank2_l1, Fixture )
{
  Array<POD> arr(N,3,2);
  ArrayView<POD,3> arrv(arr);
  for( int p=0; p<N; ++p )
  {
    for( int i=0; i<3; ++i )
    {
      arrv(p,i,0) = (part[p]!= MPL::rank() ? 0 : -gidx[p]*std::pow(10,i) );
      arrv(p,i,1) = (part[p]!= MPL::rank() ? 0 :  gidx[p]*std::pow(10,i) );
    }
  }

  int strides[] = {6,1};
  int extents[] = {1,2};
  halo_exchange.execute(arr.data(),strides,extents,2);

  switch( MPL::rank() )
  {
    case 0:
    {
      POD arr_c[] = { -9,9,   0, 0,    0,  0,  // halo
                      -1,1, -10,10, -100,100,  // core
                      -2,2, -20,20, -200,200,  // core
                      -3,3, -30,30, -300,300,  // core
                      -4,4,   0, 0,    0,  0}; // halo
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 1:
    {
      POD arr_c[] = { -3,3,   0, 0,    0,  0,  // halo
                      -4,4, -40,40, -400,400,  // core
                      -5,5, -50,50, -500,500,  // core
                      -6,6, -60,60, -600,600,  // core
                      -7,7,   0, 0,    0,  0,  // halo
                      -8,8,   0, 0,    0,  0}; // halo
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 2:
    {
      POD arr_c[] = { -5,5,   0, 0,    0,  0,  // halo
                      -6,6,   0, 0,    0,  0,  // halo
                      -7,7, -70,70, -700,700,  // core
                      -8,8, -80,80, -800,800,  // core
                      -9,9, -90,90, -900,900,  // core
                      -1,1,   0, 0,    0,  0,  // halo
                      -2,2,   0, 0,    0,  0}; // halo
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
  }
}

BOOST_FIXTURE_TEST_CASE( test_rank2_l2_v2, Fixture )
{
  // Test rank 2 halo-exchange
  Array<POD> arr(N,3,2);
  ArrayView<POD,3> arrv(arr);
  for( int p=0; p<N; ++p )
  {
    for( int i=0; i<3; ++i )
    {
      arrv(p,i,0) = (part[p]!= MPL::rank() ? 0 : -gidx[p]*std::pow(10,i) );
      arrv(p,i,1) = (part[p]!= MPL::rank() ? 0 :  gidx[p]*std::pow(10,i) );
    }
  }

  int strides[] = {6,2};
  int extents[] = {1,1};
  halo_exchange.execute(&arrv(0,1,1),strides,extents,2);

  switch( MPL::rank() )
  {
    case 0:
    {
      POD arr_c[] = {  0,0,   0,90,    0,  0,  // halo
                      -1,1, -10,10, -100,100,  // core
                      -2,2, -20,20, -200,200,  // core
                      -3,3, -30,30, -300,300,  // core
                       0,0,   0,40,    0,  0}; // halo
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 1:
    {
      POD arr_c[] = {  0,0,   0,30,    0,  0,  // halo
                      -4,4, -40,40, -400,400,  // core
                      -5,5, -50,50, -500,500,  // core
                      -6,6, -60,60, -600,600,  // core
                       0,0,   0,70,    0,  0,  // halo
                       0,0,   0,80,    0,  0}; // halo
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 2:
    {
      POD arr_c[] = {  0,0,   0,50,    0,  0,  // halo
                       0,0,   0,60,    0,  0,  // halo
                      -7,7, -70,70, -700,700,  // core
                      -8,8, -80,80, -800,800,  // core
                      -9,9, -90,90, -900,900,  // core
                       0,0,   0,10,    0,  0,  // halo
                       0,0,   0,20,    0,  0}; // halo
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
  }
}

BOOST_FIXTURE_TEST_CASE( test_rank2_v2, Fixture )
{
  Array<POD> arr(N,3,2);
  ArrayView<POD,3> arrv(arr);
  for( int p=0; p<N; ++p )
  {
    for( int i=0; i<3; ++i )
    {
      arrv(p,i,0) = (part[p]!= MPL::rank() ? 0 : -gidx[p]*std::pow(10,i) );
      arrv(p,i,1) = (part[p]!= MPL::rank() ? 0 :  gidx[p]*std::pow(10,i) );
    }
  }

  int strides[] = {2,2};
  int extents[] = {3,1};
  halo_exchange.execute(&arrv(0,0,1),strides,extents,2);

  switch( MPL::rank() )
  {
    case 0:
    {
      POD arr_c[] = {  0,9,   0,90,    0,900,  // halo
                      -1,1, -10,10, -100,100,  // core
                      -2,2, -20,20, -200,200,  // core
                      -3,3, -30,30, -300,300,  // core
                       0,4,   0,40,    0,400}; // halo
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 1:
    {
      POD arr_c[] = {  0,3,   0,30,    0,300,  // halo
                      -4,4, -40,40, -400,400,  // core
                      -5,5, -50,50, -500,500,  // core
                      -6,6, -60,60, -600,600,  // core
                       0,7,   0,70,    0,700,  // halo
                       0,8,   0,80,    0,800}; // halo
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 2:
    {
      POD arr_c[] = {  0,5,   0,50,    0,500,  // halo
                       0,6,   0,60,    0,600,  // halo
                      -7,7, -70,70, -700,700,  // core
                      -8,8, -80,80, -800,800,  // core
                      -9,9, -90,90, -900,900,  // core
                       0,1,   0,10,    0,100,  // halo
                       0,2,   0,20,    0,200}; // halo
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
  }
}

BOOST_FIXTURE_TEST_CASE( test_rank0_ArrayView, Fixture )
{
  int strides[] = {1};
  int extents[] = {N};
  ArrayView<POD,1> arrv(gidx.data(),strides,extents);

  halo_exchange.execute(arrv);

  switch( MPL::rank() )
  {
    case 0: { POD gidx_c[] = { 9, 1, 2, 3, 4};
      BOOST_CHECK_EQUAL_COLLECTIONS(gidx.begin(),gidx.end(), gidx_c,gidx_c+N); break; }
    case 1: { POD gidx_c[] = { 3, 4, 5, 6, 7, 8};
      BOOST_CHECK_EQUAL_COLLECTIONS(gidx.begin(),gidx.end(), gidx_c,gidx_c+N); break; }
    case 2: { POD gidx_c[] = { 5, 6, 7, 8, 9, 1, 2};
      BOOST_CHECK_EQUAL_COLLECTIONS(gidx.begin(),gidx.end(), gidx_c,gidx_c+N); break; }
  }
}


BOOST_FIXTURE_TEST_CASE( test_rank1_ArrayView, Fixture )
{
  Array<POD> arr(N,2);
  ArrayView<POD,2> arrv(arr);
  for( int j=0; j<N; ++j ) {
    arrv(j,0) = (part[j]!= MPL::rank() ? 0 : gidx[j]*10 );
    arrv(j,1) = (part[j]!= MPL::rank() ? 0 : gidx[j]*100);
  }

  int strides[] = {1};
  BOOST_CHECK_EQUAL_COLLECTIONS( arrv.strides()+1,arrv.strides()+2, strides,strides+1);

  halo_exchange.execute(arrv);

  switch( MPL::rank() )
  {
    case 0: { POD arr_c[] = { 90,900, 10,100, 20,200, 30,300, 40,400 };
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
    case 1: { POD arr_c[] = { 30,300, 40,400, 50,500, 60,600, 70,700, 80,800};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
    case 2: { POD arr_c[] = { 50,500, 60,600, 70,700, 80,800, 90,900, 10,100, 20,200};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+2*N, arr_c,arr_c+2*N); break; }
  }
}

BOOST_FIXTURE_TEST_CASE( test_rank2_ArrayView, Fixture )
{
  Array<POD> arr(N,3,2);
  ArrayView<POD,3> arrv(arr);
  for( int p=0; p<N; ++p )
  {
    for( int i=0; i<3; ++i )
    {
      arrv(p,i,0) = (part[p]!= MPL::rank() ? 0 : -gidx[p]*std::pow(10,i) );
      arrv(p,i,1) = (part[p]!= MPL::rank() ? 0 :  gidx[p]*std::pow(10,i) );
    }
  }

  int strides[] = {2,1};
  BOOST_CHECK_EQUAL_COLLECTIONS( arrv.strides()+1,arrv.strides()+3, strides,strides+2);

  halo_exchange.execute(arrv);

  switch( MPL::rank() )
  {
    case 0:
    {
      POD arr_c[] = { -9,9, -90,90, -900,900,
                      -1,1, -10,10, -100,100,
                      -2,2, -20,20, -200,200,
                      -3,3, -30,30, -300,300,
                      -4,4, -40,40, -400,400};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 1:
    {
      POD arr_c[] = { -3,3, -30,30, -300,300,
                      -4,4, -40,40, -400,400,
                      -5,5, -50,50, -500,500,
                      -6,6, -60,60, -600,600,
                      -7,7, -70,70, -700,700,
                      -8,8, -80,80, -800,800};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
    case 2:
    {
      POD arr_c[] = { -5,5, -50,50, -500,500,
                      -6,6, -60,60, -600,600,
                      -7,7, -70,70, -700,700,
                      -8,8, -80,80, -800,800,
                      -9,9, -90,90, -900,900,
                      -1,1, -10,10, -100,100,
                      -2,2, -20,20, -200,200};
      BOOST_CHECK_EQUAL_COLLECTIONS(arr.data(),arr.data()+6*N, arr_c,arr_c+6*N);
      break;
    }
  }
}


