#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdint>
#include <netdb.h>

std::vector<uint8_t> blockRead(int fd, size_t readSize);
ssize_t blockWrite (int fd, std::vector<uint8_t> data);

// Reads a type from vector of binary data
template <typename T>
T extract(const std::vector<unsigned char> &v, int pos)
{
  T value;
  memcpy(&value, &v[pos], sizeof(T));
  return value;
}

// Writes a type to vector of binary data
template <typename T>
std::vector<uint8_t> pack(T data)
{
     std::vector<std::uint8_t> rawData( (std::uint8_t*)&data, (std::uint8_t*)&(data) + sizeof(T));

     return rawData;
}



#endif