! (C) Copyright 2013-2014 ECMWF.

subroutine do_something_with_T1(v1)
    type(T1) :: v1
    v1%private%object = 1
end subroutine
