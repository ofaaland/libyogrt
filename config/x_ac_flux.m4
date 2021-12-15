##*****************************************************************************
#  SYNOPSIS:
#	X_AC_FLUX()
#
#  DESCRIPTION:
#	Check the usual suspects for a FLUX installation, using pkg-config
#	define FLUX_CORE_CFLAGS and FLUX_CORE_LIBS for use in Makefiles
##*****************************************************************************

AC_DEFUN([X_AC_FLUX], [
  AC_ARG_WITH(
	[flux],
	AS_HELP_STRING(--with-flux=PATH,Specify path to flux installation),
	[_x_ac_flux_dirs="$withval"
	 with_flux=yes],
	[with_flux=no])

	PKG_CHECK_MODULES([FLUX_CORE], [flux-core], [ ], [
		AS_IF([test x$with_flux = xyes],[
			AC_MSG_ERROR([unable to locate flux installation!])
		],[
			AC_MSG_WARN([unable to locate flux installation])
		])
	])
	AM_CONDITIONAL(WITH_FLUX, test "x$FLUX_CORE_LIBS" != x)
])
#
# vim: tabstop=4 shiftwidth=4 smartindent:
