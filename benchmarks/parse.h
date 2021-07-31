#include <thread>

struct OptParse
{
    int argc;
    char** argv;
    const char* binary;
    bool vdbArg;
    bool cpusArg;

    OptParse(int argc_, char* argv_[], bool _vdbArg, bool _cpusArg):
        argc(argc_), argv(argv_), binary(argv[0]), vdbArg(_vdbArg), cpusArg(_cpusArg)
    {
        help();
    }

    void
    usage [[noreturn]] () const
    {
        std::ostringstream ostr;
        ostr << "Usage: " << binary << " [options]\n" <<
        "Options:\n" <<
        "   -iterations N   number of benchmark iterations to perform (defaults to 10)\n";
        if (vdbArg) {
            ostr << "   -vdb S          filepath to a VDB (defaults to \"wdas_cloud.vdb\"\n";
        }
        if (cpusArg) {
            ostr << "   -cpus N         max number of CPUs to perform multi-threaded benchmarks (defaults to " <<
                std::thread::hardware_concurrency() << ")\n";
        }
        "   -h, -help       print this usage message and exit\n";
        std::cerr << ostr.str();
        exit(0);
    }

    bool check(int idx, const std::string& name, int numArgs = 1) const
    {
        if (argv[idx] == name) {
            if (idx + numArgs >= argc) {
                std::cerr << "option " << name << " requires "
                    << numArgs << " argument" << (numArgs == 1 ? "" : "s");
                usage();
            }
            return true;
        }
        return false;
    }

    void help() const
    {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-h" || arg == "-help" || arg == "--help") {
                usage();
            }
        }
    }

    std::string vdb() const
    {
        std::string result = "wdas_cloud.vdb";
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg[0] == '-') {
                if (check(i, "-vdb")) {
                    ++i;
                    result = argv[i];
                }
            }
        }
        return result;
    }

    int iterations() const
    {
        int result = 10;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg[0] == '-') {
                if (check(i, "-iterations")) {
                    ++i;
                    result = std::max(1, atoi(argv[i]));
                }
            }
        }
        return result;
    }

    int cpus() const
    {
        int result = std::thread::hardware_concurrency();
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg[0] == '-') {
                if (check(i, "-cpus")) {
                    ++i;
                    result = std::max(1, atoi(argv[i]));
                }
            }
        }
        return result;
    }
};
