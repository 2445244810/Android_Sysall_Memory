#include <iostream>
#include <dirent.h>
#include <cctype>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <fstream>

using namespace std;

uint64_t get_pid(const char *package_name);

uint64_t read_memory(uint64_t pid, uint64_t address);

uint64_t write_memory(uint64_t pid, uint64_t address, uint64_t value);


auto 软件包名 = "com.ShuiSha.FPS2";
auto 子弹地址 = 0x749EC96CBF00;

int main() {
    const uint64_t pid = get_pid(软件包名);
    cout << "应用 PID: " << pid << endl;

    // 读取内存
    const uint64_t read_memory_address = read_memory(pid, 子弹地址);
    cout << "读取结果: " << read_memory_address << endl;

    // 写入内存
    const uint64_t write_memory_address = write_memory(pid, 子弹地址, 100);
    cout << "读取结果: " << write_memory_address << endl;

    return 0;
}

auto get_pid(const char *package_name) -> uint64_t {
    DIR *dir = opendir("/proc");
    if (!dir) {
        return 0;
    }

    dirent *entry;
    uint64_t pid = 0;

    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            char path[256];
            snprintf(path, sizeof(path), "/proc/%s/cmdline", entry->d_name);

            if (ifstream file(path); file.is_open()) {
                string process_name;
                getline(file, process_name, '\0');

                if (!process_name.empty() && process_name == package_name) {
                    pid = stoul(entry->d_name);
                    break;
                }
            }
        }
    }
    closedir(dir);
    return pid;
}

auto read_memory(const uint64_t pid, const uint64_t address) -> uint64_t {
    iovec local[1];
    iovec remote[1];
    uint64_t buffer = 0;

    local[0].iov_base = &buffer;
    local[0].iov_len = sizeof(buffer);

    remote[0].iov_base = reinterpret_cast<void *>(address);
    remote[0].iov_len = sizeof(buffer);

    syscall(
        __NR_process_vm_readv,
        pid,
        local,
        1,
        remote,
        1,
        0
    );

    return buffer;
}

auto write_memory(const uint64_t pid, const uint64_t address, const uint64_t value) -> uint64_t {
    iovec local[1];
    iovec remote[1];
    uint64_t buffer = value;

    local[0].iov_base = &buffer;
    local[0].iov_len = sizeof(buffer);

    remote[0].iov_base = reinterpret_cast<void *>(address);
    remote[0].iov_len = sizeof(buffer);

    syscall(
        __NR_process_vm_writev,
        pid,
        local,
        1,
        remote,
        1,
        0
    );
    return value;
}


