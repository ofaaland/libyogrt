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

# --with-flux=no	no test for flux, HAVE_LIBFLUX undefined
# --with-flux=check	look in default location
# --with-flux		look in default location; error on fail
# --with-flux=yes	look in default location; error on fail
# --with-flux=<path>	look under <path>

AC_DEFUN([X_AC_FLUX], [
  AC_ARG_WITH(
    [flux],
    AS_HELP_STRING(--with-flux=PATH,Specify path to flux installation),
    [],
    [with_flux=check])

  AS_IF([test x$with_flux != xno],[
    flux_extra_libs=""
    found_flux=no

    # Check for FLUX library in the default location.
    AS_IF([test x$with_flux = xyes -o x$with_flux = xcheck],[
      AC_SEARCH_LIBS([flux_open], [flux-core], [found_flux=yes], [found_flux=no], [$flux_extra_libs])
    ])

    AS_IF([test x$found_flux = xno -a x$with_flux != xyes -a x$with_flux != xcheck ],[
      AC_CACHE_CHECK([for FLUX include directory],
                     [x_ac_cv_flux_includedir],
                     [FLUX_INCLUDEDIR="$with_flux/include/"
                      AS_IF([test -f "$FLUX_INCLUDEDIR/flux/core.h"],
                            [x_ac_cv_flux_includedir="$FLUX_INCLUDEDIR"],
                            [x_ac_cv_flux_includedir=no])
                     ])
      AC_CACHE_CHECK([for FLUX library directory],
                     [x_ac_cv_flux_libdir],
                     [x_ac_cv_flux_libdir=no
                      _x_ac_flux_libs_save=$LIBS
                      FLUX_LIBDIR="$with_flux/lib64/"
                      AS_IF([test -d "$FLUX_LIBDIR"],[
                        LIBS="-L$FLUX_LIBDIR -lflux-core $flux_extra_libs $LIBS"
                        CFLAGS="-I $x_ac_cv_flux_includedir"
                        AC_LINK_IFELSE(
                          [AC_LANG_PROGRAM([flux_open(NULL,0);])],
                          [x_ac_cv_flux_libdir=$FLUX_LIBDIR]
                        )
                      ])
                      LIBS="$_x_ac_flux_libs_save"
                     ])
      AS_IF([test x$x_ac_cv_flux_includedir != xno -a x$x_ac_cv_flux_libdir != xno],[
             found_flux=yes
             FLUX_CPPFLAGS="-I$FLUX_INCLUDEDIR"
             FLUX_LDFLAGS="-L$FLUX_LIBDIR"
            ],[
             found_flux=no
             FLUX_CPPFLAGS=""
             FLUX_LDFLAGS=""
            ])
    ])

    AS_IF([test x$found_flux != xyes],[
      AS_IF([test x$with_flux = xyes],
        [AC_MSG_ERROR([FLUX not found!])],
        [AC_MSG_WARN([not building support for FLUX])])
    ], [
        FLUX_LIBADD="-lflux-core $flux_extra_libs"
        AC_SUBST(FLUX_LIBADD)
        AC_SUBST(FLUX_CPPFLAGS)
        AC_SUBST(FLUX_LDFLAGS)
        AC_DEFINE([HAVE_LIBFLUX], 1, [Define to 1 if you have the `flux-core' library (-lflux-core).])
    ])
  ])


  AM_CONDITIONAL(WITH_FLUX, test "x$found_flux" = xyes)
])
