#include "tzif.h"
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

constexpr std::size_t ten_megabytes{(1 << 20) * 10};

int main(int argc, char *argv[]) {
  int f{STDIN_FILENO};
  const char *filename{"(stdin)"};
  std::size_t filesize{ten_megabytes};

  if (argc > 1) {
    filename = argv[1];
    f = open(filename, O_RDONLY);

    if (f == -1) {
      const int err = errno;
      std::fprintf(stderr, "%s: %s\n", filename, std::strerror(err));
      return -1;
    }

    struct stat statbuf;
    fstat(f, &statbuf);

    filesize = statbuf.st_size;
  } else {
    struct pollfd fds {
      f, POLLIN, 0,
    };

    const int result = poll(&fds, 1, 0);
    if (result == 0) {
      std::fprintf(stderr, "%s: No input\n", filename);
      return -1;
    }
  }

  u_int8_t *buf = new u_int8_t[filesize];

  filesize = read(f, buf, filesize);
  if (filesize == static_cast<std::size_t>(-1)) {
    const int err = errno;
    std::fprintf(stderr, "%s: %s\n", filename, std::strerror(err));
    return -1;
  }
  int result = close(f);
  if (result == -1) {
    const int err = errno;
    std::fprintf(stderr, "%s: %s\n", filename, std::strerror(err));
    return -1;
  }

  if (filesize < 4) {
    std::fprintf(stderr, "%s: Too small\n", filename);
    return -1;
  }
  if (std::strncmp(reinterpret_cast<const char *>(buf), "TZif", 4) != 0) {
    std::fprintf(stderr, "%s: Bad magic number\n", filename);
    return -1;
  }

  const std::unique_ptr<Tzif::Data> tzif_data = Tzif::ReadData(buf, filesize);
  if (tzif_data == nullptr) {
    std::fprintf(stderr, "%s: Error occured while reading data\n", filename);
    return -1;
  }

  std::vector<u_int8_t> output_buffer;
  tzif_data->ReformatNintendo(output_buffer);

  filename = "(stdout)";
  f = STDOUT_FILENO;
  if (argc > 2) {
    filename = argv[2];
    f = open(filename, O_WRONLY | O_CREAT | O_TRUNC);

    if (f == -1) {
      const int err = errno;
      std::fprintf(stderr, "%s: %s\n", filename, std::strerror(err));
      return -1;
    }
  }

  result = write(f, output_buffer.data(), output_buffer.size());
  if (result == -1) {
    const int err = errno;
    std::fprintf(stderr, "%s: %s\n", filename, std::strerror(err));
    return -1;
  }

  result = close(f);
  if (result == -1) {
    const int err = errno;
    std::fprintf(stderr, "%s: %s\n", filename, std::strerror(err));
    return -1;
  }

  return 0;
}
