##*****************************************************************************
## $Id: x_ac_flux.m4 8192 2006-05-25 00:15:05Z morrone $
##*****************************************************************************
#  SYNOPSIS:
#    X_AC_FLUX()
#
#  DESCRIPTION:
#    Check the usual suspects for a FLUX installation,
#    updating CPPFLAGS and LDFLAGS as necessary.
#
#  WARNINGS:
#    This macro must be placed after AC_PROG_CC and before AC_PROG_LIBTOOL.
##*****************************************************************************

##
## I don't understand some of what the other ac checks support.
## This one will look for flux headers and libraries and define
##   FLUX_CPPFLAGS
##   FLUX_LDFLAGS
##   FLUX_LIBADD
##
## configure will allow --with-flux and take the following arguments
##    no     - do not look for flux, do not define above macros
##    check  - look for flux in default locations, OK if not found
##    yes    - look for flux in default locations, failure if not found

#
# libjansson is used to parse the json flux uses to describe allocations
#

AC_DEFUN([X_AC_FLUX], [

  AC_ARG_WITH(
    [flux],
    AS_HELP_STRING(--with-flux=check/yes/no),
    [],
    [with_flux=check])

  args_ok="no"
  AS_IF([test x$with_flux = xno],[args_ok="yes"],
        [test x$with_flux = xyes],[args_ok="yes"],
        [test x$with_flux = xcheck],[args_ok="yes"]
  )
  AS_IF([test x$args_ok = xno],[
    AC_MSG_ERROR([--with-flux argument must be yes, no, or check])
  ])

  AS_IF([test x$with_flux != xno],[
    AC_SEARCH_LIBS([flux_open], [flux-core], [found_flux=yes], [found_flux=no])

    if test "$found_flux" = yes; then
      FLUX_CPPFLAGS=""
      FLUX_LDFLAGS=""
      FLUX_LIBADD="-lflux-core -ljansson"

      AC_SUBST(FLUX_LIBADD)
      AC_SUBST(FLUX_CPPFLAGS)
      AC_SUBST(FLUX_LDFLAGS)
    else
      if test "$with_flux" = yes; then
        AC_MSG_ERROR([unable to locate flux installation. Failing build.])
      else
        AC_MSG_WARN([unable to locate flux installation])
      fi
    fi
 
  ])


  AM_CONDITIONAL(WITH_FLUX, test "x$found_flux" = xyes)
])
