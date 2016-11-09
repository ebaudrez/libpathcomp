#include <pathcomp.h>
#include <iostream>
#include <cstdlib>
#include <cassert>

// by no means an idiomatic example, but the purpose is to test compiling and
// linking against the pathcomp library

int main(int argc, char **argv)
{
    pathcomp_t *c;
    char *s = NULL;

    pathcomp_add_config_from_file("main.conf");
    assert(c = pathcomp_new("my_class"));
    while (s = pathcomp_find(c)) {
        std::cout << "found: " << s << std::endl;
        free(s);
    }
    pathcomp_free(c);
    pathcomp_cleanup();
    return EXIT_SUCCESS;
}

// vim:set softtabstop=4 shiftwidth=4 expandtab:
