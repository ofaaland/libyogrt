##*****************************************************************************
## $Id: x_ac_flux.m4 8192 2006-05-25 00:15:05Z morrone $
##*****************************************************************************
#  SYNOPSIS:
#    X_AC_FLUX()
#
#  DESCRIPTION:
#    Check the usual suspects for an flux installation,
#    updating CPPFLAGS and LDFLAGS as necessary.
#
#  WARNINGS:
#    This macro must be placed after AC_PROG_CC and before AC_PROG_LIBTOOL.
##*****************************************************************************

AC_DEFUN([X_AC_FLUX], [

  # Check for FLUX header file in the default location.
  #AC_CHECK_HEADERS([flux/flux.h])

  _x_ac_flux_dirs="/usr"
  _x_ac_flux_libs="lib64 lib"

  AC_ARG_WITH(
    [flux],
    AS_HELP_STRING(--with-flux=PATH,Specify path to flux installation),
    [_x_ac_flux_dirs="$withval"
     with_flux=yes],
    [with_flux=no])

  _backup_libs="$LIBS"
  if test "$with_flux" = no; then
    # Check for FLUX library in the default location.
    AC_CHECK_LIB([flux-core], [flux_open])
  fi
  LIBS="$_backup_libs"

  if test "$ac_cv_lib_flux_flux_open" != yes; then
    AC_CACHE_CHECK(
      [for flux installation],
      [x_ac_cv_flux_dir],
      [
        incname="core.h"
        libname="flux-core"

        for d in $_x_ac_flux_dirs; do
          test -d "$d" || continue
          test -d "$d/include" || continue
          test -d "$d/include/flux" || continue
          test -f "$d/include/flux/$incname" || continue
          for bit in $_x_ac_flux_libs; do
            test -d "$d/$bit" || continue

            _x_ac_flux_libs_save="$LIBS"
            LIBS="-L$d/$bit -lflux-core $LIBS"
            AC_LINK_IFELSE(
              [AC_LANG_PROGRAM([],[flux_open(NULL,0);])],
              [AS_VAR_SET([x_ac_cv_flux_dir], [$d])
               AS_VAR_SET([x_ac_cv_flux_libdir], [$d/$bit])]
            )
            LIBS="$_x_ac_flux_libs_save"
            test -n "$x_ac_cv_flux_dir" && break
          done
          test -n "$x_ac_cv_flux_dir" && break
        done
    ])
  fi

  if test "$with_flux" = no \
     && test "$ac_cv_lib_flux_flux_open" = yes; then
    FLUX_CPPFLAGS=""
    FLUX_LDFLAGS=""
    FLUX_LIBADD="-lflux"
  elif test -n "$x_ac_cv_flux_dir"; then
    FLUX_CPPFLAGS="-I$x_ac_cv_flux_dir/include"
    FLUX_LDFLAGS="-L$x_ac_cv_flux_libdir -lpthread -lcrypto"
    FLUX_LIBADD="-lflux"
  else
    if test "$with_flux" = yes; then
      AC_MSG_ERROR([flux is not in specified location!])
    else
      AC_MSG_WARN([unable to locate flux installation])
    fi
  fi

  AC_SUBST(FLUX_CPPFLAGS)
  AC_SUBST(FLUX_LDFLAGS)
  AC_SUBST(FLUX_LIBADD)

  if test -n "$x_ac_cv_flux_dir" || test "$ac_cv_lib_flux_flux_open" = yes; then
    AS_VAR_SET([flux_available], [yes])
  else
    AS_VAR_SET([flux_available], [no])
  fi

  AM_CONDITIONAL([WITH_FLUX], [test "$flux_available" = yes])
])
