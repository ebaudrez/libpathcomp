#!/usr/bin/env perl
use strict;
use warnings;
use Test::More;
use File::Path 'rmtree';

my $exe = '../src/pathcomp';
my $prefix = "$exe -c test.archive";

perform_test(
    command => [ $prefix, qw(root=lib/archive instrument=G1 imager=SEV2 product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V006) ],
    returns => [ 'lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz' ],
);

perform_test(
    command => [ $prefix, '-e', qw(root=lib/archive instrument=G1 imager=SEV2 product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V006) ],
    returns => [ 'lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz' ],
    test_exists => 1,
);

perform_test(
    command => [ $prefix, '-e', qw(root=lib/archive instrument=G2 imager=SEV1 product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003) ],
    returns => [ 'lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0502/G2_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz' ],
    test_exists => 1,
);

# June 2007 doesn't exist in G2/SEV1
perform_test(
    command => [ $prefix, qw(root=lib/archive instrument=G2 imager=SEV1 product=SOL_TH resolution=HR level=20 slot=20070601084500 version=V003) ],
    returns => [ 'lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0601/G2_SEV1_L20_HR_SOL_TH_20070601_084500_V003.hdf.gz' ],
    test_not_exists => 1,
);

perform_test(
    command => [ $prefix, '-e', qw(root=lib/archive instrument=G2 imager=SEV1 product=SOL_TH resolution=HR level=20 slot=20070601084500 version=V003) ],
    returns => [],
);

# test -a
perform_test(
    command => [ $prefix, '-a', qw(root=lib/archive instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
        qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ 'lib/archive/G1/SEV1/G1_SEV1_L20_HR_SOL_TH/2007/0502/G1_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz',
                 'lib/archive/G1/SEV1/G1_SEV1_L20_HR_SOL_TH/2007/0502/G1_SEV1_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz',
                 'lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz',
                 'lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz',
                 'lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0502/G2_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz',
                 'lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0502/G2_SEV1_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz',
                 'lib/archive/G2/SEV2/G2_SEV2_L20_HR_SOL_TH/2007/0502/G2_SEV2_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz',
                 'lib/archive/G2/SEV2/G2_SEV2_L20_HR_SOL_TH/2007/0502/G2_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz' ],
);

perform_test(
    command => [ $prefix, '-ae', qw(root=lib/archive instrument=G1 instrument+=G2 imager=SEV1 imager+=SEV2 ),
        qw(product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V003 version+=V006) ],
    returns => [ 'lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz',
                 'lib/archive/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/2007/0502/G2_SEV1_L20_HR_SOL_TH_20070502_084500_V003.hdf.gz' ],
    test_exists => 1,
);

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
    command => [ $prefix, qw(-f .pathcomprc root=lib/archive instrument=G1 imager=SEV2 product=SOL_TH resolution=HR level=20 slot=20070502084500 version=V006) ],
    returns => [ 'lib/archive/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/2007/0502/G1_SEV2_L20_HR_SOL_TH_20070502_084500_V006.hdf.gz' ],
);

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
