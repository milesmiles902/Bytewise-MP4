#include <array>
#include "videofile.h"
#include "convert.h"
#include <iostream>

// Set the position of the file stream
void VideoFile::setFilePos(unsigned int pos)
{
  file.seekg(pos);
  return;
}

// Get fileSize
unsigned int VideoFile::getFileSize(void)
{
  return fileSize;
}

// Read bytes as ASCII string and update pos value
std::string VideoFile::readString(unsigned int numOfBytes, unsigned int &pos)
{
  // Dynamically allocate array size
  char *arr = new char(numOfBytes);

  file.read(arr, numOfBytes);
  pos += numOfBytes;
  std::string res = std::string(arr, numOfBytes);
  delete arr;
  return res;
}

// Read bytes as 16-bit unsigned int and update pos value
unsigned int VideoFile::read16(unsigned int &pos)
{
  std::array<char, 2> arr;
  file.read(arr.data(), arr.size());
  pos += 2;
  
  return Convert::convert16(arr);

}

// Read bytes as 32-bit unsigned int and update pos value
unsigned int VideoFile::read32(unsigned int &pos)
{
  std::array<char, 4> arr;
  file.read(arr.data(), arr.size());

  pos += 4;

  return Convert::convert32(arr);
}

// Read bytes as 64-bit unsigned int and update pos value
unsigned int VideoFile::read64(unsigned int &pos)
{
  std::array<char, 8> arr;
  file.read(arr.data(), arr.size());
  pos += 8;

  return Convert::convert64(arr);
}

// Generic function to read numOfBytes
std::vector<char> VideoFile::readBytes(unsigned int numOfBytes, unsigned int &pos)
{
  // Dynamically allocate array size
  char *arr = new char(numOfBytes);
  file.read(arr, numOfBytes);
  pos += numOfBytes;
  std::vector<char> res(arr, arr + numOfBytes * sizeof(char));
  delete arr;
  return res;
}

// Read 8 bytes and returns information of the block
Block VideoFile::getBlock(void)
{

  Block blockInfo;

  // Read the block size
  std::array<char, 4> blockSizeBuffer;
  file.read(blockSizeBuffer.data(), blockSizeBuffer.size());

  blockInfo.blockSize = Convert::convert32(blockSizeBuffer);
  
  // Read the block type
  std::array<char, 4> blockTypeBuffer;
  file.read(blockTypeBuffer.data(), blockTypeBuffer.size());

  blockInfo.blockType = std::string(blockTypeBuffer.data(), blockTypeBuffer.size());

  return blockInfo;
}

// Opening file
void VideoFile::file_open(std::string fileName)
{
  file.open(fileName, std::ios::in | std::ios::binary);
  // Set fileSize
  file.seekg(0, std::ios::end);
  fileSize = file.tellg();
  
  file.seekg(0, std::ios::beg);
}

// Closing file
void VideoFile::file_close(void)
{
  file.close();
}
