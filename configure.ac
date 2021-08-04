AC_PREREQ([2.69])
AC_INIT(open-multiboot, 0.1)
AM_INIT_AUTOMAKE

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([src/omb_common.h])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CC

#RFX_CHECK_FREETYPE
AC_CHECK_LIB([m], [acos])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h mntent.h stdlib.h string.h sys/ioctl.h sys/mount.h unistd.h])

PKG_CHECK_MODULES(LIBJSON, [ json-c >= 0.11 ])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_FUNC_GETMNTENT
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([getmntent gettimeofday memset mkdir munmap])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT