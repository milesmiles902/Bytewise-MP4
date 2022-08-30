// Structure to hold information about mp4 blocks

#ifndef BLOCK_H
#define BLOCK_H

#include <string>

// Object with block information
struct Block
{
  unsigned int blockSize;
  std::string blockType;
};

#endif
