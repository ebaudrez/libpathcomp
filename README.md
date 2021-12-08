# NAME

Libpathcomp - A library for pathname composition

# SYNOPSIS

Suppose you have to work with pathnames that are built up from components, for
example,

    /opt/data/${instrument}/${instrument}_${year}${month}.dat

Libpathcomp is designed to make the construction of these pathnames as
convenient as possible, and offers a few functions for working with the
constructed pathnames. One of the ways to use Libpathcomp for pathnames as given
above, is to create a file called `my-config` with the following contents:

    [data]
    root      = /opt/data
    extension = .dat
    yymm      = lua { return string.format('%02d%02d', self.year % 100, self.month) }
    compose   = lua { return string.format('%s/%s_%s%s', self.instrument, \
                      self.instrument, self.yymm, self.extension) }

This file should be used together with a C program whose source could look like:

    #include <pathcomp.h>
    
    pathcomp_t *composer;
    
    pathcomp_add_config_from_file("my-config");
    composer = pathcomp_new("data");
    pathcomp_set(composer, "instrument", "N6");
    pathcomp_set_int(composer, "year", 1989);
    pathcomp_set_int(composer, "month", 3);
    char *path = pathcomp_yield(composer);
    printf("path: %s\n", path); /* prints "path: /opt/data/N6/N6_8903.dat" */
    free(path);
    patcomp_free(composer);
    pathcomp_cleanup();

Instead of, or in addition to, a C program, one could also use the stand-alone
utility `pathcomp`:

    $ pathcomp -f my-config -c data instrument=N6 year=1989 month=3
    /opt/data/N6/N6_8903.dat

Please have a look at the User Guide below for an overview of the features and
usage of Libpathcomp. You may also want to have a look at the Cookbook for
examples of usage.

# DESCRIPTION

Libpathcomp is a library for composing and working with pathnames. It is meant
to make it easy to build structured pathnames whose content depends on variables
that are taken from a configuration file, or that are supplied by the calling
function.

The example in the synopsis demonstrates how to construct pathnames for the data
files for a hypothetical scientific data product. Let us assume that the file
names for this product are constructed as follows:

    ${instrument}_${year}${month}.dat

The year is abbreviated to the last two digits. To keep the data collection
tidy, data files for the same instrument are grouped together in the same
directory, with the name of the instrument. Let us also assume that the data
collection is stored in a directory called `/opt/data` on your hard disk. An
example of a conforming pathname is `/opt/data/N6/N6_8903.dat`.

It is easy enough to construct conforming pathnames in C:

    #include <stdio.h>
    
    char path[100];
    const char *root = "/opt/data";
    const char *instrument = "N6";
    int year = 1989, month = 3;
    const char *extension = ".dat";
    snprintf(path, sizeof(path), "%s/%s/%s_%02d%02d%s", root, instrument, \
             instrument, year % 100, month, extension);

Although slightly shorter than the corresponding example with Libpathcomp, this
solution has a few drawbacks:

  * The values, and the pathname template, are hardcoded in the C program. This
    implies that the program must be recompiled whenever one of those change.
  * The size of the destination buffer is fixed. While 100 characters might be
    enough for the full pathname, this is by no means guaranteed. For very short
    pathnames, 100 bytes might be a waste of space.
  * The return value of `snprintf()` is not checked. In case of failure, one
    might need to retry `snprintf()` with a larger buffer.

It is definitely possible to improve the C code to address these issues, i.e.,
reading values from a configuration file, and using dynamic memory allocation
and reallocation. But by that time, the code has already grown a lot in size and
complexity.

I think this is the reason some C programs are actually called by shell scripts
or other interpreted languages, whose main purpose is to pass fully constructed
pathnames to the C program. This is because their string manipulation features,
especially string interpolation, lend themselves well to pathname construction:

    # example for the Bash shell
    instrument=N6
    year=1989
    month=`printf "%02d" 3`
    input="/opt/data/$instrument/${instrument}_${year:3:2}${month}.dat"
    ./my_program "$input"

The C code is a lot easier: the full pathname is in `argv[1]` when `main()`
kicks off. But now there are two programs: the C program, and the shell script.
Wouldn't it be easier to do everything from the C program, but with the
convenience of an interpreted language? This is where Libpathcomp steps in. It
has the following features:

  * It takes its information from a configuration file. No recompilation
    required when the configuration file changes.
  * Settings from the configuration file can be overridden at run time.
  * Constructed pathnames are allocated dynamically, and are automatically of
    the right size. Static or fixed-size buffers are not needed.
  * It supports functions written in [Lua], to construct arbitrarily complex
    pathnames.
  * It supports 'variations' of pathnames, like multiple possibilities for a
    file extension, multiple root directories (sort of like a search path), etc.
  * It has a few convenience functions to make it easy to work with the
    constructed pathnames, e.g., checking for existence, creating the containing
    directory, finding the first (or all) of a set of files.

In brief, you could view Libpathcomp as a glorified `sprintf()` implementation,
with a config file and a Lua parser, and extra features for working conveniently
with pathnames.

In addition to the C interface, there exists a standalone executable that brings
some of the functionality of the library to the command line.

# INTERFACE STABILITY

Libpathcomp is still under development and, in its current incarnation, hasn't
been tested much in real applications. You should consider the interface subject
to change.

# CONFIG FILE SYNTAX

The syntax of the configuration files is inspired by the syntax of Git's config
files (documented in the manual page of `git-config(1)`), and is similar to the
syntax of INI files. Configuration files contain sections, corresponding to
composer classes, and attributes for the composer classes. Each attribute must
belong to a section.

Sections are indicated by the class name in square brackets, and are followed by
attribute-value pairs, one per line. Attributes and their values are separated
by an equals sign. Whitespace surrounding the equals sign is stripped. Leading
and trailing whitespace on any line is also stripped.

Values can either be strings, or Lua functions (see below for the syntax).

Multiple values can be specified for an attribute, by listing multiple
`attribute = value` assignments for the same `attribute`. This is the primary
mechanism for creating alternatives.

Comments are introduced with either `;` or `#`, and run through the end of the
line. Comments are currently only recognized when they are the only thing on a
line (except for leading whitespace).

Long lines can be continued in traditional UNIX fashion, by writing a backslash
followed by a newline character.

## Example

    [colormap]
        ; multiple values are allowed
        root = /usr/share/myapp/colormaps
        root = /opt/data/colormaps
        extension = .txt
        ; self.extension will refer to the 'extension' attribute above
        ; 'basename' is not in the config file and needs to be set at run time
        compose = lua { return string.format("%s%s", self.basename, self.extension) }
    
    [output]
        ; relative path
        root = gen/data
        extension = .bin
        compose = lua { return self.basename .. self.extension }

## Special attributes

The following attributes are interpreted specially by Libpathcomp:

  * **compose**  
    This attribute will be evaluated by pathcomp_yield(). It need not be
    present.
  * **root**  
    This attribute will be prepended to the result of the evaluation of
    _compose_ in order to yield the full pathname returned by pathcomp_yield().
    It need not be an absolute path, and it need not be present.
  * **copy-from**  
    This attribute allows you to inherit attributes from another section; see
    below for more information. This attribute need not be present.

Other attributes have no special interpretation.

## Lua functions

It is possible to use Lua functions instead of strings for the value of
attributes. When such an attribute is evaluated, the Lua function is called, and
its value is returned. To specify a Lua function, format the value as follows:
write the word `lua`, followed by the function body enclosed in braces (`{` and
`}`):

    attribute = lua { <function body> }

The whitespace following the word `lua` is optional. If the attribute is not
formatted correctly (e.g., one of the braces is missing), the value will not be
recognized as a Lua function, but will be parsed as a string instead. The code
seen by the Lua parser is equivalent to

    local self = ...; <function body>

Notes:

  * It is mandatory to return a value from the function.
  * A Lua userdata value representing the composer object is passed in as the
    hidden `self` parameter (you do not need to declare it). You can refer to
    the other attributes in the Lua function by writing `self.attribute`.
  * A syntax or runtime error in the Lua function will not be detected until
    the attribute is evaluated.

The standard Lua libraries (`os`, `string`, ...) are available in Lua functions.

## Inheriting attributes

Libpathcomp allows a class to inherit attributes from another class. To do this,
declare an attribute with the special name _copy-from_ in your class. Its value
should be the name of the section you want to inherit from.

Please note that a class' attribute always _overrides_ an inherited attribute
with the same name. A class' attribute is never combined with an inherited
attribute of the same name to add alternatives to the values inherited from the
parent class.

# USER'S GUIDE

## Terminology

In Libpathcomp terminology, the object returned by pathcomp_new() is a _composer
object_. pathcomp_new() expects an argument, the _class name_ for this composer.
A composer object is like a dictionary of key-value pairs, the keys being called
_attributes_. If sections with the same name as the class are found in the
configuration file, the object's attributes are initialized from those sections.
If no section with the same name can be found, the composer object starts out
without attributes.

Libpathcomp introduces a concept which is useful to handle variations in
pathnames. When an attribute has multiple values, it is said to have
_alternatives_. Every attribute has the notion of a _current alternative_: when
an attribute is evaluated, the value corresponding to its current alternative is
returned. Alternatives are stored in an ordered list, and we say the attribute
is _rewound_ when the current alternative is set to the first
alternative in the list. Several attributes may possess several attributes, and
the composer object always corresponds to one possible _combination of
alternatives_. Libpathcomp provides a means to step through all combinations,
through an iterator interface. Obviously, when a composer object has no
attributes with multiple values, there is only one possible combination of
alternatives: the object represents only one possible pathname.

## Data structures

The only publicly visible data structure for the time being is the `pathcomp_t`
structure, which is an abstract data type. It can only be manipulated through
the public interface of Libpathcomp.

## Set-up and shutdown

    pathcomp_add_config_from_file("my_filename");
    /* ... */
    pathcomp_cleanup();

As indicated before, Libpathcomp allows one to store data for composer classes
and their attributes in a configuration file. Although not required, this allows
reuse of the pathname templates and their attributes. Libpathcomp starts out
with an empty configuration. In order to add to this configuration, you can use
pathcomp_add_config_from_file() and pathcomp_add_config_from_string(). The
former reads configuration from a file on disk, and the latter from a
null-terminated string passed in as an argument.
pathcomp_add_config_from_string() is mostly intended for testing.

Libpathcomp maintains global state for the parsed configuration file, and for
the Lua interpreter. To deallocate the global state, you need to call
pathcomp_cleanup() after you are done using all composer objects.

## Creating and destroying composer objects

    pathcomp_t *composer;
    composer = pathcomp_new("my_class");
    /* ... */
    pathcomp_free(composer);

Composer objects are created using pathcomp_new(). This function takes as an
argument the class of the composer object. The class name is then compared to
the name of the sections in the parsed configuration to find matching sections.
If none are found, the composer object starts out without attributes. When a
matching section is found, its attributes are added to the composer object as if
pathcomp_add() had been called with the name and value of the attribute as it
appears in the configuration. This is repeated for every matching section.
Multiple assignments to the same attribute will create alternatives. Please note
that an assignment to an attribute inherited from a parent class will _override_
the inherited value, instead of adding an alternative.

Note that it is not required to pass to pathcomp_new() a class name that matches
a section name in the configuration, nor is it required to have any
configuration parsed before calling pathcomp_new(). An empty composer object
(i.e., without attributes) may be created in this way.

The composer object must be deallocated by calling pathcomp_free().

## Cloning composer objects

    pathcomp_t *clone;
    clone = pathcomp_clone(composer);
    /* ... */
    pathcomp_free(clone);

It is also possible to create a composer object from another one.
pathcomp_clone() creates a clone which starts out with identical attributes and
state as the original, but which is otherwise independent. Changes to the clone
do not affect the original, and neither do changes to the original affect the
clone after the cloning. The clone must be deallocated with pathcomp_free(),
just like the original.

## Setting and adding attribute values

    pathcomp_set(composer, "year", "2004");
    // alternatively, the year can be supplied as an int
    pathcomp_set_int(composer, "year", 2004);
    for (int month = 1; month <= 12; month++)
        pathcomp_add_int(composer, "month", month);
    pathcomp_set(composer, "extension", ".txt");
    pathcomp_add(composer, "extension", ".dat");
    pathcomp_add(composer, "extension", ".nc");
    pathcomp_add(composer, "root", "/opt/data");

Most of the time, you will need to set attributes at run time. After all, it
doesn't make much sense to use complex pathname templates if all of the
components of the pathname can be determined before the program is run. It is
likely the pathname templates in the configuration file will refer to
attributes that are _not_ set at configuration time, like the `instrument`,
`year`, and `month` attributes in the synopsis.

Attributes are set with pathcomp_set() and pathcomp_add(). The difference
between the two is that pathcomp_set() will consider an attribute to hold only
one value, and set that value to the one you supply. pathcomp_add(), on the
other hand, will consider the attribute to be an ordered list of alternatives
(turning it into a list if it isn't one already), and _add_ the value you supply
to the values the attribute already holds. The alternative is appended to the
end of the list. If the attribute doesn't exist yet, pathcomp_set() and
pathcomp_add() are equivalent.

In other words, pathcomp_set() is for single-valued attributes, pathcomp_add()
for multi-valued attributes.

Although Libpathcomp is primarily concerned with the composition of text
strings, often you'll need to set the value of an attribute to an integer. In
particular, if the value of an attribute is obtained from a loop, the loop
variable is likely an integer. pathcomp_set_int() and pathcomp_add_int() are the
equivalent of pathcomp_set() and pathcomp_add(), except that they accept and
store a value of type _int_ instead of a string. When the attribute is
evaluated, the integer is converted to a string using the "%d" conversion
specification (see printf(3)).

Please note the following:

  * An attribute needn't exist before you call pathcomp_set() or pathcomp_add();
    you can use them to create new attributes.
  * Attributes always _evaluate_ as text strings, reflecting their use as
    components of pathnames. However, since version 0.3, they can be supplied as
    integers, which is particularly useful inside loops.
  * Composer objects are completely open. That means you may set or add any
    attribute, or create new ones, at run time. There is no protection against
    overwriting an existing attribute.
  * It is possible pass a Lua function as the value of an attribute at run time,
    using the same syntax as in the configuration file:
    ```
    pathcomp_set(composer, "the_answer", "lua { return 42 }");
    ```

## Evaluating attributes

    char *val;
    val = pathcomp_eval(composer, "year"); /* returns "2004" */
    /* ... */
    free(val);

To evaluate an attribute, use pathcomp_eval(). If the attribute is a single
string value, that value is returned. Otherwise, if it's a Lua function, this
function is called, and the result is returned. Otherwise, if it's an integer
value, the integer is converted to a string, and the result is returned.
Otherwise, if it's a multi-valued attribute, the result corresponding to the
current alternative is returned. (More on the 'current alternative' later.)

Note that pathcomp_eval() always returns a string, even for attributes that look
like numbers. The string returned by pathcomp_eval() is allocated dynamically,
and must be freed by the caller, by calling `free()`. If the attribute can't be
found, or can't be evaluated without raising an error, NULL is returned.

There is also a function pathcomp_eval_nocopy(), which is mostly intended for
testing. For more information about it, you should read the sources.

## Working with alternatives

    pathcomp_add_int(composer, "version", 1);
    pathcomp_add_int(composer, "version", 2);
    pathcomp_add_int(composer, "version", 3);
    while (!pathcomp_done(composer)) {
        char *val;
        /* returns "1", "2", "3" in successive iterations */
        val = pathcomp_eval(composer, "version");
        /* ... */
        free(val);
        pathcomp_next();
    }
    pathcomp_rewind(composer);

It is not always sufficient to specify a single value for an attribute. For
example, what if the data files in the example in the synopsis could be
uncompressed or compressed? This would require multiple extensions. Or what if
data files could be scattered over multiple locations on the disk? This would
require multiple roots. For this reason, Libpathcomp supports alternatives.

Alternatives are added by means of pathcomp_add(), and are stored in an ordered
list. Attributes will be 'upgraded' from single-valued to multi-valued if
required. If multiple attributes have alternatives, the composer class
represents one combination of alternatives. Libpathcomp provides a means to step
through all possible combinations in a systematic fashion, via an interface
modelled like an iterator.

Every attribute has the notion of the 'current alternative'. For single-value
attributes, this is trivial. For multi-valued attributes, a hidden pointer is
kept that identifies the current alternative in the ordered list of
alternatives.

The composer object always represents one possible combination of alternatives
at any one time. By calling pathcomp_next(), the next combination is selected in
the following way:

  1. The composer object steps through all attributes in sequence. The order in
     which the attributes are selected is unspecified.
  2. The selected attribute is updated: its 'current alternative' is advanced to
     the next alternative. If there is one, pathcomp_next() terminates and
     returns 1. Otherwise, execution continues with step 3.
  3. There is no 'next alternative' for the selected attribute. The 'current
     alternative' is made to point to the beginning of the list. The attribute
     is said to have been 'rewound'. The **next attribute** is selected, and
     execution resumes from step 2. If there aren't any attributes left,
     execution continues with step 4.
  4. All attributes have been rewound; there are no combinations left.
     pathcomp_next() returns 0.

At this point, all possibilities have been exhausted, and calling
pathcomp_next() will not do anything. pathcomp_done() will return 1. If the
composer object is to be rewound, in order to step through all combinations
again, pathcomp_rewind() must be called.

pathcomp_done() returns a boolean that indicates whether all combinations have
been visited.

The example code shown above is admittedly somewhat contrived. But in
combination with pathcomp_yield() (described below), alternatives can be very
useful!

### Interaction with pathcomp_set() and pathcomp_add()

You should be aware that pathcomp_set() and pathcomp_add() do not automatically
rewind all alternatives like they used to do in an early version. This may
surprise you if invoke them while stepping through all combinations using
pathcomp_next() (or another function that relies on it, such as
pathcomp_find()). Remember that combinations are tried in an unspecified order;
there is no guarantee that a matching combination of alternatives comes later in
the list of possible combinations.

For example, imagine the situation where you have successfully located a data
file with the following attributes:

    instrument = G2
    extension  = .hdf
    extension  = .hdf.gz

Further suppose "G2" files are gzip-compressed. After calling pathcomp_find(),
the _extension_ attribute will point to the `.hdf.gz` value. Suppose you now
want to locate a data file for an instrument called "GL", whose data files are
_not_ compressed:

    pathcomp_set(composer, "instrument", "GL")

pathcomp_find() will now fail even if a matching file exists, because the
alternative `.hdf` comes before `.hdf.gz` in the list of alternatives.

The solution is the following: if you have invoked pathcomp_set() or
pathcomp_add() after starting an iteration with pathcomp_next(), call
pathcomp_rewind() to reset the state of the iterator. If you haven't started an
iteration, there is no need to invoke pathcomp_rewind(). Hence, in the common
case of setting or adding attributes at run time right after composer object
creation, rewinding the attributes isn't necessary.

## Working with pathnames

The main purpose of Libpathcomp is to make it easy to work with pathnames, and
the last group of functions we review are meant to do that.

### Composing pathnames

    char *path;
    pathcomp_set(composer, "root", "/opt/data");
    pathcomp_set(composer, "compose", "N6/N6_8903.txt");
    path = pathcomp_yield(composer); /* returns "/opt/data/N6/N6_8903.txt" */
    /* ... */
    free(path);

pathcomp_yield() returns the pathname that the composer object represents. If
the composer object contains alternatives, pathcomp_yield() returns the pathname
that is represented by the current combination of alternatives.

pathcomp_yield() evaluates the attributes `root` and `compose` and joins them
with an intervening directory separator. Note that the directory separator is
hardcoded to a slash (`/`). If the `root` attribute cannot be evaluated, the
value of the `compose` attribute is returned. If the `compose` attribute cannot
be evaluated, the `root` attribute is returned. If neither can be evaluated,
NULL is returned.

The string returned by pathcomp_yield() is allocated dynamically, and must be
freed by the caller, by calling `free()`.

### Finding files and directories

    char *path;
    pathcomp_set(composer, "extension", ".dat");
    pathcomp_add(composer, "extension", ".dat.gz");
    path = pathcomp_find(composer);
    if (!path) { /* no existing pathname found */ }
    else {
        /* first match is in "path" */
        free(path);
    }

It may be useful to find out which combinations among the possible combinations
of alternatives match existing pathnames. In the example shown above, if a file
could exist with extension `.dat` or with extension `.dat.gz` (but not both),
pathcomp_find() will return the pathname that matches the existing file.

pathcomp_find() is built on top of pathcomp_yield() and pathcomp_next(). It will
call both repeatedly until it finds an existing pathname. pathcomp_find() can be
used to find the first match among the possible combinations, as shown in the
example above. Since the order in which alternatives are visited is unspecified,
it is unspecified which match will be returned first. However, alternatives for
a single attribute are stored in an ordered list, so you can be sure that
alternatives that come earlier in the list of alternatives for an attribute are
tried earlier.

It's also possible to retrieve all matches, by calling pathcomp_find() in a
loop:

    for (;;) {
        char *path = pathcomp_find(composer);
        if (!path) break;
        /* matched an existing path */
        free(path);
    }

Or, if you prefer a `while` loop:

    char *path;
    while ((path = pathcomp_find(composer))) {
        /* matched an existing path */
        free(path);
    }

This works because the composer object remembers the last combination tried, and
a new invocation of pathcomp_find() will resume searching from the _next_
combination.

pathcomp_find() leaves the composer object in the state corresponding to the
file just matched. That means you can evaluate the attributes to find out
details of the file just matched, without needing to parse the pathname! For the
example shown above,

    char *extension;
    /* returns '.dat' or '.dat.gz' depending on which alternative matched */
    extension = pathcomp_eval(composer, "extension");

If no matching pathname can be found, pathcomp_find() returns NULL. Otherwise,
pathcomp_find() returns a dynamically allocated string, which must be freed by
the caller by calling `free()`. Note that pathcomp_find() will perform _stat(2)_
calls on the filesystem.

### Creating directories recursively

    pathcomp_set(composer, "root", "/opt/data");
    pathcomp_set(composer, "compose", "N6/N6_8903.log");
    /* full path is "/opt/data/N6/N6_8903.log" */
    if (pathcomp_mkdir(composer) == 0) {
        /* directory "/opt/data/N6" was created */
    }
    else {
        /* errno contains error number */
        perror("mkdir");
        /* or: */
        fprintf(stderr, "error creating directories: %s\n", strerror(error));
    }

With pathnames that represent potentially complex directory hierarchies comes
the need to create directories recursively. If, as in the example, you want to
create a new file named "/opt/data/N6/N6_8903.log", you have to ensure that all
directory components leading up to the final "N6_8903.log" have been created
beforehand. In other words, you cannot create a new file
"/opt/data/N6/N6_8903.log" if "/opt/data/N6" hasn't been created yet.

pathcomp_mkdir() can be used to recursively create the directories that the
composer object represents, as if you ran `mkdir -p` on the directory portion of
the pathname returned by pathcomp_yield(). In contrast to a normal _mkdir(2)_
call, it is not an error if any or all of the parent directories already exist.
If the composer object contains alternatives, the directory portion of the
pathname corresponding to the current combination of alternatives is created
recursively.

pathcomp_mkdir() will recursively create the directories up to the final
directory separator in the pathname. Note that pathcomp_mkdir() recognizes only
a slash (`/`) as the directory separator. Therefore, if the pathname ends in a
slash, the full pathname is assumed to represent a directory:

    ; suppose daily averages are stored in a directory like "N6/N6_8903/" --
    ; a composer object with this attribute then represents that directory;
    ; note how the pathname ends in a slash
    compose = lua { return string.format('%s/%s_%s/', self.instrument, \
                    self.instrument, self.yymm) }

In this example, pathcomp_mkdir() will create the directory
`/opt/data/N6/N6_8903`.

If, on the other hand, the pathname does not end in a slash, the full pathname
is assumed to represent a file:

    ; a composer object with this attribute represents a file;
    ; note how the pathname DOES NOT end in a slash
    compose = lua { return string.format('%s/%s_%s%s', self.instrument, \
                    self.instrument, self.yymm, self.extension) }

In this example, pathcomp_mkdir() will create the directory `/opt/data/N6`.

Note that, internally, pathcomp_mkdir() calls pathcomp_yield(), so the value of
the `root` attribute is always used as the leading part of the directories to be
created, if this attribute exists.

pathcomp_mkdir() returns 0 if the directory was made successfully, and -1
otherwise. In the latter case, `errno` is set to the error number of the
underlying system call.

# COOKBOOK

## Find first matching pathname

    ; config file
    [class]
        version = V003
        version = V002
        version = V001
        compose = lua { return string.format('data_%s.txt', self.version) }

    /* C */
    char *path = pathcomp_find(composer);

This is a situation in which you want to locate the latest version of a
particular file. By default, pathcomp_find() returns the first matching
(existing) pathname. The trick is to rank the alternatives in the order you want
them to be tried; you can always rely on the alternatives being tried in the
order they are given.

In the example shown, a version `V003` would be tried first. If `data_V003.txt`
exists, pathcomp_find() returns that string. Otherwise,
if `data_V002.txt` exists, it is returned. Otherwise, if `data_V001.txt` exists,
it is returned. Otherwise, NULL is returned.

## Determine attributes dynamically

    ; config file
    [class]
        extension = hdf.gz
        extension = hdf
        compose   = lua { return string.format('data.%s', self.extension) }

    /* C */
    char *path = pathcomp_find(composer);
    char *extension = pathcomp_eval(composer, "extension");

A variation of finding the first matching filename, is locating a filename whose
attributes are not known in advance. The example shows how you would go about
locating a file which might be gzip-compressed. pathcomp_find() will try the
compressed file `data.hdf.gz` first, and return its pathname if it exists.
Otherwise, it will return `data.hdf`. Either way, you can inspect the
`extension` attribute to figure out which extension matched.

## Find all matching pathnames

    ; config file
    [class]
        root    = data/G1/SEV2
        root    = data/G1/SEV3
        root    = data/G2/SEV1
        root    = data/G3/SEV3
        root    = data/G4/SEV4
        compose = PROCESSED

    /* C */
    char *path;
    while ((path = pathcomp_find(composer))) {
        char *root = pathcomp_eval(composer, "root");
        printf("directory %s has been processed\n", root);
        free(path);
        free(root);
    }

This example shows how to locate all matching (existing) pathnames using
pathcomp_find(). Assume you have a number of directories, and the presence of a
file called `PROCESSED` in any directory indicates whether this directory has
been processed already.

Called for the first time, pathcomp_find() returns the first match, but it
remembers the last successfully matched combination, and resumes searching for
other matching combinations if you call it again. This way, you can use it in a
loop to search for all existing pathnames, as shown in the example.

Since pathcomp_find() leaves the composer object in the state corresponding to
the pathname that matched last, you can use pathcomp_eval() to examine the
attributes of the composer. This allows you to obtain parts of the pathname
without the need to parse it!

Mind the extra parentheses in the condition of the `while`. They are there to
silence a `gcc` warning, and are generally useful to alert the reader that the
single `=` for assignment is intended. (You may want to use a `for` loop if you
dislike this style.)

## Cache directory

    ; config file
    [class]
        root    = cache
        root    = /opt/data
        compose = my_file.txt

    /* C */
    char *path;
    path = pathcomp_find(composer);

This is another variation on finding the first matching pathname. The idea is
that files may reside in two locations. One location would be a large, but
slower, storage area, and the other location would be a faster storage area, but
much smaller in size. Some, but not all, of the files present in the first
storage are duplicated in the second (which is a kind of cache).

As far as the contents of any given input file are concerned, we don't care
whether this file comes from the first or second location (the cache). But since
the cache offers much faster access, we ought to prefer it for any file that can
be found there. Therefore, we give two alternatives for the `root` attribute,
ordering the cache directory first.

## Creating a file in multiple directories

    ; config file
    [class]
        root    = data/G1/SEV2
        root    = data/G1/SEV3
        root    = data/G2/SEV1
        root    = data/G3/SEV3
        root    = data/G4/SEV4
        compose = PROCESSED

    /* C */
    #include <stdio.h>
    #include <errno.h>
    #include <string.h>
    #include <stdlib.h>
    
    char *path;
    FILE *fp;
    for (pathcomp_rewind(composer); !pathcomp_done(composer); pathcomp_next(composer)) {
        path = pathcomp_yield(composer);
        if (pathcomp_mkdir(composer) == -1) {
            fprintf(stderr, "Error creating directories for path %s: %s\n",
                path, strerror(errno));
            exit(2);
        }
        fp = fopen(path, "w");
        fprintf(fp, "done.\n");
        fclose(fp);
        free(path);
    }

You can also use pathcomp_yield() and pathcomp_mkdir() in a loop. However,
unlike pathcomp_find(), these functions do not automatically advance to the next
combination. You need to iterate manually using pathcomp_next(), and check for
the end of the iteration with pathcomp_done().

This example shows how you would create a file called `PROCESSED` in multiple
directories, when it is not sure that all directories exist. pathcomp_mkdir()
will create them, recursively, and return successfully if they were created
successfully or if they existed already. pathcomp_yield() is then used to obtain
the full pathname of the witness file `PROCESSED`. The pathname is then used in
a call to `fopen()` to create a small file with dummy content. Note that you
could not have created a new file using `fopen()` unless the parent directory
existed, which is why you need pathcomp_mkdir() to ensure the parent directory
exists.

## User home directory

    ; config file
    [class]
        homedir = lua { return os.getenv('HOME') }

All standard Lua libraries are available inside Lua functions. The `os` library
provides a function `getenv` which allows retrieving variables from the
environment, as shown in the example above for the user's home directory. Of
course, any environment variable can be retrieved this way.

## Shell wildcard characters and globbing

    ; config file
    [class]
        instrument = *
        imager     = SEV?
        prefix     = lua { return string.format('%s_%s_L20_HR_SOL_TH', \
                           self.instrument, self.imager) }
        compose    = lua { return string.format('%s/%s/%s/README', \
                           self.instrument, self.imager, self.prefix) }

    /* C */
    #include <glob.h>
    glob_t buf;
    char *pattern;
    char **el;
    pattern = pathcomp_yield(composer);
    /* match "*/SEV?/*_SEV?_L20_HR_SOL_TH/README" */
    if (glob(pattern, 0, NULL, &buf) != 0) { /* error */ }
    for (el = buf.gl_pathv; *el; el++) {
        /* (*el) points to a matched pathname */
    }
    globfree(&buf);
    free(pattern);

Libpathcomp does not have facilities for shell-style globbing (yet). But you can
still take advantage of its ability to create structured pathnames if you
combine shell-style wildcards with a call to _glob(3)_ or _wordexp(3)_. See
above for an example where two components of the pathname are specified with
shell wildcard characters. Since the instrument and imager occur more than once
in the full pathname, Libpathcomp repeats the wildcard characters as needed.

## Using inherited attributes to avoid code duplication

    ; config file
    [gerb]
        instrument = G1
        imager     = SEV2
        level      = 20
        prefix     = lua { return string.format('%s_%s_L%s_%s_%s', self.instrument, self.imager, self.level, self.resolution, self.product) }
        compose    = lua { return string.format('%s/%s/%s/README', self.instrument, self.imager, self.prefix) }

    [gerb.barg]
        copy-from  = gerb
        resolution = BARG
        product    = SOL_M15_R50

    [gerb.hr]
        copy-from  = gerb
        resolution = HR
        product    = SOL_TH

    /* C */
    pathcomp_t *barg, *hr;
    barg = pathcomp_new("gerb.barg");
    path = pathcomp_yield(barg); /* returns "G1/SEV2/G1_SEV2_L20_BARG_SOL_M15_R50/README" */
    free(path);
    hr = pathcomp_new("gerb.hr");
    path = pathcomp_yield(hr); /* returns "G1/SEV2/G1_SEV2_L20_HR_SOL_TH/README" */
    free(path);

This example shows how you could construct two very similar pathnames for two
different data products (actually, their README file) without code duplication.
By writing a common ancestor section (which is not actually used in the C code),
and having the two sections `gerb.barg` and `gerb.hr` inherit their attributes
from it, only the differences between the data products need to be specified.

## Locating the log file associated to a data file

    ; config file
    [hdf]
        root       = /home/myuser/cache
        root       = /opt/data
        root       = /mnt/nfs/data
        instrument = N6
        extension  = .hdf
        compose    = ...

    /* C */
    pathcomp_t *hdf, *log;
    char *path;
    hdf = pathcomp_new("hdf");
    path = pathcomp_find(hdf);
    free(path);
    /* hdf composer now points to first matching (existing) .hdf file */
    log = pathcomp_clone(hdf);
    pathcomp_set(log, extension, ".log");
    /* log composer now points to associated .log file */
    pathcomp_free(hdf);
    pathcomp_free(log);

Suppose you have a data product that has a log file for every data file, and
suppose further that the log file has the same basename as the data file, with
extension `.log` instead of `.hdf`. This example shows how you would locate the
log file for a data file. First, the data file is located with pathcomp_find().
A clone of the composer object is created. The clone starts out with identical
state, so it initially refers to the data file. By calling pathcomp_set(), we
can change the extension to `.log`. Remember that pathcomp_set() doesn't rewind
alternatives, so the `root` attribute continues to point to the same directory.
The `log` composer now points to the log file associated to the data file.

# BUGS

None known. Please test Libpathcomp, and get back to me with bug reports and
feedback.

# AUTHOR

Edward Baudrez

# COPYRIGHT AND LICENSE

Copyright (C) 2015, 2016 Edward Baudrez <edward.baudrez@gmail.com>

Libpathcomp is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Libpathcomp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Libpathcomp; if not, see <http://www.gnu.org/licenses/>.

[Lua]: http://www.lua.org

<!--- vim:set textwidth=80 softtabstop=4 shiftwidth=4 expandtab: -->
