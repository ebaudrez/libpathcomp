#include <pathcomp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv)
{
    pathcomp_t *c;
    char *s = NULL;

    pathcomp_add_config_from_file("main.conf");
    assert(c = pathcomp_new("my_class"));
    while (s = pathcomp_find(c)) {
        printf("found: %s\n", s);
        free(s);
    }
    pathcomp_free(c);
    pathcomp_cleanup();
    return EXIT_SUCCESS;
}

/* vim:set softtabstop=4 shiftwidth=4 expandtab: */
