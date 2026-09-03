#include <string>
#include <chrono>

class FileNumberSync {
    private:
        unsigned long long number;
        std::string filepath;
        std::chrono::time_point<std::chrono::steady_clock> last;

        unsigned long long fileValue();
        void writeCheck();

    public:
        FileNumberSync(std::string path);
        unsigned long long value();
        void increment();
        void decrement();
};