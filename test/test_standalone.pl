#!/usr/bin/env perl
#
# Copyright (C) 2015 Edward Baudrez <edward.baudrez@gmail.com>
# This file is part of Libpathcomp.
#
# Libpathcomp is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Libpathcomp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with Libpathcomp; if not, see <http://www.gnu.org/licenses/>.
#
use strict;
use warnings;
use Test::More;
use File::Path 'rmtree';
use File::Copy 'copy';

my $exe = '../src/pathcomp';
my $srcdir = $ENV{srcdir} || '.';
my $prefix = "$exe -c test.archive";

if (! -d "$srcdir/lib/archive") {
    plan skip_all => "lib/archive not available in $srcdir";
}

if ($srcdir ne '.') {
    copy("$srcdir/.pathcomprc", ".pathcomprc") or die "copy failed: $!";
}

my @returns;

perform_test(
    command => [ $prefix, "root=$srcdir/lib/archive", qw(instrument=G1 imager=SEV2 product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V006) ],
    returns => [ "$srcdir/lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz" ],
);

perform_test(
    command => [ $prefix, '-e', "root=$srcdir/lib/archive", qw(instrument=G1 imager=SEV2 product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V006) ],
    returns => [ "$srcdir/lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz" ],
    test_exists => 1,
);

perform_test(
    command => [ $prefix, '-e', "root=$srcdir/lib/archive", qw(instrument=G2 imager=SEV1 product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003) ],
    returns => [ "$srcdir/lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0502/G2_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz" ],
    test_exists => 1,
);

# June 2007 doesn't exist in G2/SEV1
perform_test(
    command => [ $prefix, "root=$srcdir/lib/archive", qw(instrument=G2 imager=SEV1 product=SOL_TH resolution=HR level=20 slot=20070601084500 version=V003) ],
    returns => [ "$srcdir/lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0601/G2_SEV1_L20_HR_SOL_TH_20070601_084500_V003.hdf.gz" ],
    test_not_exists => 1,
);

perform_test(
    command => [ $prefix, '-e', "root=$srcdir/lib/archive", qw(instrument=G2 imager=SEV1 product=SOL_TH resolution=HR level=20 slot=20070601084500 version=V003) ],
    returns => [],
);

# test -a
perform_test(
    command => [ $prefix, '-a', "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ "$srcdir/lib/archive/G1/SEV1/G1_SEV1_L20_HR_SOL_TH/2007/0502/G1_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz",
                 "$srcdir/lib/archive/G1/SEV1/G1_SEV1_L20_HR_SOL_TH/2007/0502/G1_SEV1_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz",
                 "$srcdir/lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz",
                 "$srcdir/lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz",
                 "$srcdir/lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0502/G2_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz",
                 "$srcdir/lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0502/G2_SEV1_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz",
                 "$srcdir/lib/archive/G2/SEV2/G2_SEV2_L20_HR_SOL_TH/2007/0502/G2_SEV2_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz",
                 "$srcdir/lib/archive/G2/SEV2/G2_SEV2_L20_HR_SOL_TH/2007/0502/G2_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz" ],
);

perform_test(
    command => [ $prefix, "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ "$srcdir/lib/archive/G1/SEV1/G1_SEV1_L20_HR_SOL_TH/2007/0502/G1_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz" ],
);

perform_test(
    command => [ $prefix, '-ae', "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ "$srcdir/lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz",
                 "$srcdir/lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0502/G2_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz" ],
    test_exists => 1,
);

@returns = perform_test(
    command => [ $prefix, '-e', "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    test_exists => 1,
);
cmp_ok @returns, '==', 1;
ok($returns[0] eq "$srcdir/lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz" ||
   $returns[0] eq "$srcdir/lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0502/G2_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz");
ok -e $returns[0];

# test -m
rmtree 'lib/scratch';
ok ! -d 'lib/scratch', 'directory does not exist';
ok ! -d 'lib/scratch/abc', 'directory does not exist';
ok ! -d 'lib/scratch/abc/def', 'directory does not exist';
perform_test(
    command => [ $exe, '-c', 'test.mkdir', '-m', qw(root=lib/scratch one=abc two=def three=ghi) ],
    returns => [ 'lib/scratch/abc/def/ghi' ],
    test_not_exists => 1,
);
ok -d 'lib/scratch', 'directory has been created';
ok -d 'lib/scratch/abc', 'directory has been created';
ok -d 'lib/scratch/abc/def', 'directory has been created';
rmtree 'lib/scratch';

# test -f
perform_test(
    command => [ $prefix, "-f", "$srcdir/.pathcomprc", "root=$srcdir/lib/archive", qw(instrument=G1 imager=SEV2 product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V006) ],
    returns => [ "$srcdir/lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz" ],
);

# test -x
perform_test(
    command => [ $prefix, '-a', qw(-x instrument), "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ 'G1',
                 'G1',
                 'G1',
                 'G1',
                 'G2',
                 'G2',
                 'G2',
                 'G2' ],
);

perform_test(
    command => [ $prefix, '-a', qw(-x imager), "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ 'SEV1',
                 'SEV1',
                 'SEV2',
                 'SEV2',
                 'SEV1',
                 'SEV1',
                 'SEV2',
                 'SEV2' ],
);

perform_test(
    command => [ $prefix, qw(-x instrument), "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ 'G1' ],
);

perform_test(
    command => [ $prefix, qw(-x version), "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ 'V003' ],
);

perform_test(
    command => [ $prefix, '-ae', qw(-x instrument), "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ 'G1',
                 'G2' ],
);

perform_test(
    command => [ $prefix, '-ae', qw(-x imager), "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ 'SEV2',
                 'SEV1' ],
);

perform_test(
    command => [ $prefix, '-ae', qw(-x version), "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ 'V006',
                 'V003' ],
);

@returns = perform_test(
    command => [ $prefix, '-e', qw(-x version), "root=$srcdir/lib/archive", qw(instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    test_exists => 1,
);
cmp_ok @returns, '==', 1;
ok($returns[0] eq 'V006' || $returns[0] eq 'V003');

perform_test(
    command => [ $prefix, '-e', qw(-x extension), "root=$srcdir/lib/archive", qw(instrument=G2 imager=SEV1),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006),
                 qw(extension=.hdf extension+=.hdf.gz) ],
    returns => [ '.hdf.gz' ], # GERB (used to be) gzip-compressed
);

perform_test(
    command => [ $prefix, '-e', qw(-x extension), "root=$srcdir/lib/archive", qw(instrument=GL imager=SEV1),
                 qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006),
                 qw(extension=.hdf extension+=.hdf.gz) ],
    returns => [ '.hdf' ], # GERB-like not gzip-compressed, at least not externally ;-)
);

if ($srcdir ne '.') {
    unlink ".pathcomprc" or die "cannot unlink .pathcomprc: $!";
}

done_testing;

sub perform_test
{
    my %o = @_;
    #my $level = Test::More->builder->level;
    #Test::More->builder->level($level + 1);
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    my @cmd = @{ $o{command} };
    note "running @cmd";
    chomp (my @got = `@cmd`);
    #note "got $_" for @got;
    if ($o{returns}) {
        my @expected = @{ $o{returns} };
        is_deeply( [sort @got], [sort @expected] );
        if ($o{test_exists}) {
            for my $f (@expected) {
                ok -e $f, "$f exists";
            }
        }
        if ($o{test_not_exists}) {
            for my $f (@expected) {
                ok ! -e $f, "$f does not exist";
            }
        }
    }
    return @got;
}
