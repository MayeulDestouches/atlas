! (C) Copyright 2013-2014 ECMWF.

!------------------------------------------------------------------------------
TYPE, extends(object_type) :: atlas_GatherScatter

! Purpose :
! -------
!   *Gather* :

! Methods :
! -------
!   setup : Setup using arrays detailing proc, glb_idx, remote_idx, max_glb_idx
!   execute : Do the gather

! Author :
! ------
!   17-Dec-2013 Willem Deconinck     *ECMWF*

!------------------------------------------------------------------------------
contains
  procedure :: glb_dof => GatherScatter__glb_dof
  procedure, private :: GatherScatter__setup32
  procedure, private :: GatherScatter__setup64
  procedure, private :: GatherScatter__gather_int32_r1_r1
  procedure, private :: GatherScatter__gather_int32_r2_r2
  procedure, private :: GatherScatter__gather_int32_r3_r3
  procedure, private :: GatherScatter__gather_int64_r1_r1
  procedure, private :: GatherScatter__gather_int64_r2_r2
  procedure, private :: GatherScatter__gather_int64_r3_r3
  procedure, private :: GatherScatter__gather_real32_r1_r1
  procedure, private :: GatherScatter__gather_real32_r2_r2
  procedure, private :: GatherScatter__gather_real32_r3_r3
  procedure, private :: GatherScatter__gather_real64_r1_r1
  procedure, private :: GatherScatter__gather_real64_r2_r2
  procedure, private :: GatherScatter__gather_real64_r3_r3
  procedure, private :: GatherScatter__scatter_int32_r1_r1
  procedure, private :: GatherScatter__scatter_int32_r2_r2
  procedure, private :: GatherScatter__scatter_int64_r1_r1
  procedure, private :: GatherScatter__scatter_int64_r2_r2
  procedure, private :: GatherScatter__scatter_real32_r1_r1
  procedure, private :: GatherScatter__scatter_real32_r2_r2
  procedure, private :: GatherScatter__scatter_real64_r1_r1
  procedure, private :: GatherScatter__scatter_real64_r2_r2
  procedure, private :: GatherScatter__scatter_real64_r3_r3
  generic :: setup => &
      & GatherScatter__setup32, &
      & GatherScatter__setup64
  generic :: gather => &
      & GatherScatter__gather_int32_r1_r1, &
      & GatherScatter__gather_int32_r2_r2, &
      & GatherScatter__gather_int32_r3_r3, &
      & GatherScatter__gather_int64_r1_r1, &
      & GatherScatter__gather_int64_r2_r2, &
      & GatherScatter__gather_int64_r3_r3, &
      & GatherScatter__gather_real32_r1_r1, &
      & GatherScatter__gather_real32_r2_r2, &
      & GatherScatter__gather_real32_r3_r3, &
      & GatherScatter__gather_real64_r1_r1, &
      & GatherScatter__gather_real64_r2_r2, &
      & GatherScatter__gather_real64_r3_r3
  generic :: scatter => &
      & GatherScatter__scatter_int32_r1_r1, &
      & GatherScatter__scatter_int32_r2_r2, &
      & GatherScatter__scatter_int64_r1_r1, &
      & GatherScatter__scatter_int64_r2_r2, &
      & GatherScatter__scatter_real32_r1_r1, &
      & GatherScatter__scatter_real32_r2_r2, &
      & GatherScatter__scatter_real64_r1_r1, &
      & GatherScatter__scatter_real64_r2_r2, &
      & GatherScatter__scatter_real64_r3_r3

END TYPE atlas_GatherScatter
!------------------------------------------------------------------------------

interface atlas_GatherScatter
  module procedure atlas_GatherScatter__ctor
end interface

!------------------------------------------------------------------------------
