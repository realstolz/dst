#!/usr/bin/perl
# -*- cperl -*-

# Copyright (c) 2007 MySQL AB
# Use is subject to license terms.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

use strict;

use My::File::Path;

use Test::Simple tests => 8;
use File::Temp qw / tempdir /;
my $dir = tempdir( CLEANUP => 1 );
my $testdir="$dir/test";

my $subdir= "$testdir/test1/test2/test3";

#
# 1. Create and remove a directory structure
#
mkpath($subdir);
ok( -d $subdir, "Check '$subdir' is created");

rmtree($testdir);
ok( ! -d $testdir, "Check '$testdir' is gone");

#
# 2. Create and remove a directory structure
# where one directory is chmod to 0000
#
mkpath($subdir);
ok( -d $subdir, "Check '$subdir' is created");

ok( chmod(0000, $subdir) == 1 , "Check one dir was chmoded");

rmtree($testdir);
ok( ! -d $testdir, "Check '$testdir' is gone");

#
# 3. Create and remove a directory structure
# where one file is chmod to 0000
#
mkpath($subdir);
ok( -d $subdir, "Check '$subdir' is created");

my $testfile= "$subdir/test.file";
open(F, ">", $testfile) or die;
print F "hello\n";
close(F);

ok( chmod(0000, $testfile) == 1 , "Check one file was chmoded");

rmtree($testdir);
ok( ! -d $testdir, "Check '$testdir' is gone");

 