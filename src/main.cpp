#include "tzif.h"
#include <array>
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

constexpr std::size_t ten_megabytes{(1 << 20) * 10};

int main(int argc, char *argv[]) {
  int f{STDIN_FILENO};
  char *filename{};
  std::size_t filesize{ten_megabytes};

  if (argc > 1) {
    char *filename = argv[1];
    f = open(filename, O_RDONLY);

    struct stat statbuf;
    stat(filename, &statbuf);

    filesize = statbuf.st_size;
  }

  u_int8_t *buf = new u_int8_t[filesize];

  filesize = read(f, buf, filesize);
  close(f);

  const std::unique_ptr<Tzif::DataImpl> tzif_data =
      Tzif::ReadData(buf, filesize);

  std::vector<u_int8_t> output_buffer;
  tzif_data->ReformatNintendo(output_buffer);

  f = STDOUT_FILENO;
  if (argc > 2) {
    char *filename = argv[2];
    f = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
  }

  write(f, output_buffer.data(), output_buffer.size());

  close(f);

  return 0;
}
