#!/bin/sh
touch aclocal.m4
find -name Makefile.in -print0 | xargs -0 touch
touch configure
touch include/config.h.in
