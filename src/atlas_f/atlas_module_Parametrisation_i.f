! (C) Copyright 2013-2014 ECMWF.

!------------------------------------------------------------------------------
TYPE, extends(object_type) :: atlas_Parametrisation

! Purpose :
! -------
!   *Parametrisation* : Container of Parametrisation, parameters or attributes
!       The Parametrisation are seted as key, value pairs

! Methods :
! -------
!   set : set a new property with given key and value
!   set : Modify a property with given key and value
!   get : Return a property value for given key

! Author :
! ------
!   20-Nov-2013 Willem Deconinck     *ECMWF*

!------------------------------------------------------------------------------
contains
  procedure, private :: set_logical => Parametrisation__set_logical
  procedure, private :: set_int32 => Parametrisation__set_int32
  procedure, private :: set_real32 => Parametrisation__set_real32
  procedure, private :: set_real64 => Parametrisation__set_real64
  procedure, private :: set_string => Parametrisation__set_string
  procedure, private :: set_array_int32 => Parametrisation__set_array_int32
  procedure, private :: set_array_int64 => Parametrisation__set_array_int64
  procedure, private :: set_array_real32 => Parametrisation__set_array_real32
  procedure, private :: set_array_real64 => Parametrisation__set_array_real64
  procedure :: has => Parametrisation__has
  generic :: set => set_logical, set_int32, set_real32, set_real64, set_string, set_array_int32, &
    set_array_int64, set_array_real32, set_array_real64
  procedure :: get_int32 => Parametrisation__get_int32
  procedure :: get_logical => Parametrisation__get_logical
  procedure :: get_real32 => Parametrisation__get_real32
  procedure :: get_real64 => Parametrisation__get_real64
  procedure :: get_string => Parametrisation__get_string
  procedure :: get_array_int32 => Parametrisation__get_array_int32
  procedure :: get_array_int64 => Parametrisation__get_array_int64
  procedure :: get_array_real32 => Parametrisation__get_array_real32
  procedure :: get_array_real64 => Parametrisation__get_array_real64
  generic :: get => get_int32, get_logical, get_real32, get_real64, get_string, get_array_int32, &
    get_array_int64, get_array_real32, get_array_real64

END TYPE atlas_Parametrisation

!------------------------------------------------------------------------------

interface atlas_Parametrisation
  module procedure atlas_Parametrisation__ctor
end interface

!------------------------------------------------------------------------------