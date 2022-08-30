// Container to hold the mp4 after it is read into a stream

#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include <fstream>
#include <string>
#include <vector>
#include "block.h"

namespace VideoFile
{

  namespace{
    // Store the file in a stream
    std::fstream file;
    unsigned int fileSize;
  }

  // Set the position of the file stream
  void setFilePos(unsigned int pos);

  // Get fileSize
  unsigned int getFileSize(void);

  // Read bytes as ASCII string and update pos value
  std::string readString(unsigned int numOfBytes, unsigned int &pos);

  // Read bytes as 32-bit unsigned int and update pos value
  unsigned int read16(unsigned int &pos);

  // Read bytes as 32-bit unsigned int and update pos value
  unsigned int read32(unsigned int &pos);

  // Read bytes as 64-bit unsigned int and update pos value
  unsigned int read64(unsigned int &pos);

  // Generic function to read numOfBytes
  std::vector<char> readBytes(unsigned int numOfBytes, unsigned int &pos);

  // Read 8 bytes and returns information of the block
  Block getBlock(void);

  // Opening/Closing file
  void file_open(std::string fileName);
  void file_close(void);
  
};

#endif
