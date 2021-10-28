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

AC_DEFUN([X_AC_FLUX], [

  AC_ARG_VAR([FLUX_LIBDIR], [Directory containing FLUX libraries])
  AC_ARG_VAR([FLUX_INCLUDEDIR], [Directory containing FLUX header files])
  AC_ARG_VAR([FLUX_ENVDIR], [Directory containing FLUX configuration files])

  AC_ARG_WITH(
    [flux],
    AS_HELP_STRING(--with-flux=PATH,Specify path to flux installation),
    [],
    [with_flux=check])

  AS_IF([test x$with_flux != xno],[
    # various libs needed to call lsb_ functions
    #flux_extra_libs="-lbat -lflux -lrt -lnsl"
    flux_extra_libs="-lflux -lrt -lnsl"

    found_flux=no
    # Check for FLUX library in the default location.
    AS_IF([test x$with_flux = xyes -o x$with_flux = xcheck],[
      AC_SEARCH_LIBS([lsb_init], [bat], [found_flux=yes], [found_flux=no], [$flux_extra_libs])
    ])

    AS_IF([test x$found_flux = xno],[
      AC_CACHE_CHECK([for FLUX include directory],
                     [x_ac_cv_flux_includedir],
                     [AS_IF([test -z "$FLUX_INCLUDEDIR"],
                            [FLUX_INCLUDEDIR=`grep FLUX_INCLUDEDIR= $FLUX_ENVDIR/flux.conf 2>/dev/null| cut -d= -f2`])
                      AS_IF([test -f "$FLUX_INCLUDEDIR/flux/lsbatch.h"],
                            [x_ac_cv_flux_includedir="$FLUX_INCLUDEDIR"],
                            [x_ac_cv_flux_includedir=no])
                     ])
      AC_CACHE_CHECK([for FLUX library directory],
                     [x_ac_cv_flux_libdir],
                     [x_ac_cv_flux_libdir=no
                      AS_IF([test -d "$FLUX_LIBDIR"],[
                        LIBS="-L$FLUX_LIBDIR -lbat $flux_extra_libs $LIBS"
                        AC_LINK_IFELSE(
                          [AC_LANG_PROGRAM([lsb_init(NULL);])],
                          [x_ac_cv_flux_libdir=$FLUX_LIBDIR]
                        )
                        LIBS="$_x_ac_flux_libs_save"
                      ])
                     ])
      AS_IF([test x$x_ac_cv_flux_includedir != xno -a x$x_ac_cv_flux_libdir != xno],[
             found_flux=yes
             FLUX_CPPFLAGS="-I$FLUX_INCLUDEDIR"
             FLUX_LDFLAGS="-L$FLUX_LIBDIR"
             FLUX_LIBADD="-lbat $flux_extra_libs"
            ],[
             found_flux=no
             FLUX_CPPFLAGS=""
             FLUX_LDFLAGS=""
             FLUX_LIBADD=""
            ])
    ])

    AS_IF([test x$found_flux != xyes],[
      AS_IF([test x$with_flux = xyes],
        [AC_MSG_ERROR([FLUX not found!])],
        [AC_MSG_WARN([not building support for FLUX])]
      )
    ])
  ])

  AC_SUBST(FLUX_LIBADD)
  AC_SUBST(FLUX_CPPFLAGS)
  AC_SUBST(FLUX_LDFLAGS)

  AM_CONDITIONAL(WITH_FLUX, test "x$found_flux" = xyes)
])
