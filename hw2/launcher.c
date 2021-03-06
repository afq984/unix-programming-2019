#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const char *default_sopath = "./sandbox.so";
const char *default_basedir = ".";

void setpath(const char *errhint, const char *in, char *out) {
    if (!realpath(in, out)) {
        fprintf(stderr, "cannot resolve %s `%s`: %s\n", errhint, in,
                strerror(errno));
        exit(1);
    }
}

int main(int argc, char **argv) {
    int opt;
    char sopath[PATH_MAX];
    char basedir[PATH_MAX];
    int sopath_set = 0;
    int basedir_set = 0;
    while (-1 != (opt = getopt(argc, argv, "p:d:"))) {
        switch (opt) {
        case 'p':
            setpath("-p", optarg, sopath);
            sopath_set = 1;
            break;
        case 'd':
            setpath("-d", optarg, basedir);
            basedir_set = 1;
            break;
        default:
            fprintf(
                stderr,
                "usage: %s [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n"
                "       -p: set the path to sandbox.so, default = %s\n"
                "       -d: restrict directory, default = %s\n"
                "       --: seperate the arguments for sandbox and for the "
                "executed command\n",
                argv[0], default_sopath, default_basedir);
            return 1;
        }
    }
    if (optind == argc) {
        fprintf(stderr, "no command given.\n");
        return 1;
    }
    if (!sopath_set) {
        setpath("default_sopath", default_sopath, sopath);
    }
    if (!basedir_set) {
        setpath("default_basedir", default_basedir, basedir);
    }
    setenv("LD_PRELOAD", sopath, 1);
    setenv("SANDBOX_BASEDIR", basedir, 1);
    execvp(argv[optind], argv + optind);
    perror(argv[optind]);
}
