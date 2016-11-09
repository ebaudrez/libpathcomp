#include <pathcomp.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    pathcomp_t *composer;
    char *path;

    pathcomp_add_config_from_file("your-config-file");
    composer = pathcomp_new("your-name-here");
    pathcomp_set(composer, "slot", "20130209_124500");
    path = pathcomp_yield(composer);
    printf("%s\n", path);
    free(path);
    pathcomp_free(composer);
    pathcomp_cleanup();
    return 0;
}

/* vim:set softtabstop=4 shiftwidth=4 expandtab: */
