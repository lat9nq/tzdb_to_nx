#include "tzif.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <sys/types.h>

namespace Tzif {

static std::size_t SkipToVersion2(const u_int8_t *data) {
  char magic[5];
  const u_int8_t *p{data};

  std::memcpy(magic, data, 4);
  magic[4] = '\0';

  if (std::strcmp(magic, "TZif") != 0) {
    return 0;
  }

  do {
    p++;
  } while (std::strncmp(reinterpret_cast<const char *>(p), "TZif", 4) != 0);

  return p - data;
}

template <typename Type> constexpr static void SwapEndianess(Type *value) {
  u_int8_t *data = reinterpret_cast<u_int8_t *>(value);

  union {
    u_int8_t data[sizeof(Type)];
    Type value;
  } temp;

  for (int i = 0; i < sizeof(Type); i++) {
    int alt_index = sizeof(Type) - i - 1;
    temp.data[alt_index] = data[i];
  }

  *value = temp.value;
}

static void FlipHeader(Header &header) {
  SwapEndianess(&header.isutcnt);
  SwapEndianess(&header.isstdcnt);
  SwapEndianess(&header.leapcnt);
  SwapEndianess(&header.timecnt);
  SwapEndianess(&header.typecnt);
  SwapEndianess(&header.charcnt);
}

std::unique_ptr<DataImpl> ReadData(const u_int8_t *data, std::size_t size) {
  const u_int8_t *p = data + SkipToVersion2(data);

  Header header;
  std::memcpy(&header, p, sizeof(header));
  p += sizeof(header);

  FlipHeader(header);

  std::unique_ptr<DataImpl> impl = std::make_unique<DataImpl>();
  impl->header = header;

  impl->transition_times = std::make_unique<int64_t[]>(header.timecnt);
  impl->transition_types = std::make_unique<u_int8_t[]>(header.timecnt);
  impl->local_time_type_records =
      std::make_unique<TimeTypeRecord[]>(header.typecnt);
  impl->time_zone_designations = std::make_unique<int8_t[]>(header.charcnt);
  impl->standard_indicators = std::make_unique<u_int8_t[]>(header.isstdcnt);
  impl->ut_indicators = std::make_unique<u_int8_t[]>(header.isutcnt);

  std::memcpy(impl->transition_times.get(), p,
              header.timecnt * sizeof(int64_t));
  p += header.timecnt * sizeof(int64_t);

  std::memcpy(impl->transition_types.get(), p,
              header.timecnt * sizeof(u_int8_t));
  p += header.timecnt * sizeof(u_int8_t);

  std::memcpy(impl->local_time_type_records.get(), p,
              header.typecnt * sizeof(TimeTypeRecord));
  p += header.typecnt * sizeof(TimeTypeRecord);

  std::memcpy(impl->time_zone_designations.get(), p, header.charcnt);
  p += header.charcnt * sizeof(int8_t);

  std::memcpy(impl->standard_indicators.get(), p,
              header.isstdcnt * sizeof(u_int8_t));
  p += header.isstdcnt * sizeof(u_int8_t);

  std::memcpy(impl->ut_indicators.get(), p, header.isutcnt * sizeof(u_int8_t));
  p += header.isutcnt * sizeof(u_int8_t);

  const std::size_t footer_string_length = data + size - p - 2;
  p++;

  impl->footer.tz_string = std::make_unique<char[]>(footer_string_length);
  std::memcpy(impl->footer.tz_string.get(), p, footer_string_length);
  impl->footer.footer_string_length = footer_string_length;

  return impl;
}

static void PushToBuffer(std::vector<u_int8_t> &buffer, const void *data,
                         std::size_t size) {
  const u_int8_t *p{reinterpret_cast<const u_int8_t *>(data)};
  for (std::size_t i = 0; i < size; i++) {
    buffer.push_back(*p);
    p++;
  }
}

void DataImpl::ReformatNintendo(std::vector<u_int8_t> &buffer) const {
  buffer.clear();

  Header header_copy{header};
  header_copy.isstdcnt = 0;
  header_copy.isutcnt = 0;
  FlipHeader(header_copy);

  PushToBuffer(buffer, &header_copy, sizeof(Header));
  PushToBuffer(buffer, transition_times.get(),
               header.timecnt * sizeof(int64_t));
  PushToBuffer(buffer, transition_types.get(),
               header.timecnt * sizeof(u_int8_t));
  PushToBuffer(buffer, local_time_type_records.get(),
               header.typecnt * sizeof(TimeTypeRecord));
  PushToBuffer(buffer, time_zone_designations.get(),
               header.charcnt * sizeof(int8_t));
  // omit standard_indicators
  // omit ut_indicators
  PushToBuffer(buffer, &footer.nl_a, 1);
  PushToBuffer(buffer, footer.tz_string.get(), footer.footer_string_length);
  PushToBuffer(buffer, &footer.nl_b, 1);
}

} // namespace Tzif
