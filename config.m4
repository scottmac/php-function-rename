dnl
dnl $Id: config.m4 282645 2009-06-23 13:09:34Z johannes $
dnl

PHP_ARG_ENABLE(rename, whether to enable function rename support,
[  --enable-rename          Enable function rename support])

if test "$PHP_RENAME" != "no"; then
	rename_sources="php_rename.c "
	PHP_NEW_EXTENSION(rename, $rename_sources, $ext_shared)
fi
