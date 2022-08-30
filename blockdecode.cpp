// Decode functions

#include <unordered_map>
#include <iostream>
#include <string>
#include <ctime>
#include <cassert>
#include "videofile.h"
#include "blockdecode.h"
#include "convert.h"

unsigned int timeUnit;

// Mapping from string to enum
std::unordered_map<std::string, BlockDecode::BlockType> decodeMap({
  {"ftyp", BlockDecode::type_ftyp},
  {"moov", BlockDecode::type_moov},
  {"mdat", BlockDecode::type_mdat},
  {"mvhd", BlockDecode::type_mvhd},
  {"trak", BlockDecode::type_trak},
  {"udta", BlockDecode::type_udta},
  {"tkhd", BlockDecode::type_tkhd},
  {"mdia", BlockDecode::type_mdia},
  {"mdhd", BlockDecode::type_mdhd},
  {"hdlr", BlockDecode::type_hdlr},
  {"minf", BlockDecode::type_minf},
  {"vmhd", BlockDecode::type_vmhd},
  {"smhd", BlockDecode::type_smhd},
  {"dinf", BlockDecode::type_dinf},
  {"stbl", BlockDecode::type_stbl},
  {"dref", BlockDecode::type_dref},
  {"stsd", BlockDecode::type_stsd},
  {"stts", BlockDecode::type_stts},
  {"stss", BlockDecode::type_stss},
  {"stsc", BlockDecode::type_stsc},
  {"stsz", BlockDecode::type_stsz},
  {"stco", BlockDecode::type_stco},
  
  // mdia{mdhd, hdlr, minf}, minf{vmhd, smhd, dinf, stbl}, dinf{dref}, stbl{stsd, stts, stss, stsc, stsz, stco/co64}
});

// Returns the enum mapping of the given string
BlockDecode::BlockType BlockDecode::getBlockMapping(std::string blockType)
{
  return decodeMap[blockType];
}

// ftyp decode
void BlockDecode::ftyp::decode(unsigned int blockSize)
{
  // Current position within the box
  unsigned int blockPos = 8;

  std::cout << "Brand: " << VideoFile::readString(4, blockPos) << std::endl;
  std::cout << "Brand Version: " << VideoFile::read32(blockPos) << std::endl;
  if(blockPos < blockSize){
    std::cout << "Compatible Brands: ";
    while(blockPos < blockSize){
      std::cout << VideoFile::readString(4, blockPos);
      if(blockPos < blockSize){
        std::cout << ", ";
      }
    }
  }
  std::cout << std::endl;
}

// moov decode
void BlockDecode::moov::decode(unsigned int blockBase, unsigned int blockSize)
{
  // Current position within the box
  unsigned int blockPos = 8;

  while(blockPos < blockSize - 1){
    Block blockInfo = VideoFile::getBlock();
    std::cout << "-Block Size: " << blockInfo.blockSize << " bytes, Block Type: " << blockInfo.blockType << std::endl;
    // Branch into different blocks
    switch (BlockDecode::getBlockMapping(blockInfo.blockType))
    {
      case type_mvhd:
        BlockDecode::mvhd::decode(blockInfo.blockSize);
        break;
      case type_trak:
        BlockDecode::trak::decode(blockBase + blockPos, blockInfo.blockSize);
        break;
      case type_udta:
        BlockDecode::udta::decode(blockInfo.blockSize);
        break;
      default:
        break;
    }

    blockPos += blockInfo.blockSize;
    VideoFile::setFilePos(blockBase + blockPos);
  }
}

// mvhd decode
void BlockDecode::mvhd::decode(unsigned int blockSize)
{
  // Current position within the box
  unsigned int blockPos = 8;

  // If version is 1, date and duration values are 8 bytes long, otherwise 4.

  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];

  std::cout << "--Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "--Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS

  // CREATED MODIFIED TIME
  std::cout << "--Creation Time: ";
  version ? Convert::convertDate(VideoFile::read64(blockPos)) 
          : Convert::convertDate(VideoFile::read32(blockPos));
  std::cout << std::endl;
  std::cout << "--Modification Time: ";
  version ? Convert::convertDate(VideoFile::read64(blockPos)) 
          : Convert::convertDate(VideoFile::read32(blockPos));
  std::cout << std::endl;
  // CREATED MODIFIED TIME

  // TIME SCALE
  timeUnit = VideoFile::read32(blockPos);
  std::cout << "--Time Scale: " << timeUnit << std::endl;
  // TIME SCALE

  // DURATION
  unsigned int duration = version ? VideoFile::read64(blockPos) : VideoFile::read32(blockPos);
  std::cout << "--Duration: " << duration << " (";
  Convert::convertDuration(duration, timeUnit);
  std::cout << ")" << std::endl;
  // DURATION

  // USER PLAYBACK SPEED
  std::cout << "--Preferred Rate: " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
  // USER PLAYBACK SPEED

  // USER VOLUME
  std::cout << "--Preferred Volume: " << Convert::convertFixedPoint(VideoFile::read16(blockPos), 8) << std::endl;
  // USER VOLUME

  // RESERVED 10 BYTES
  std::cout << "--Reserved: ";
  for(int i = 0; i < 4; i++){
    std::cout << VideoFile::read16(blockPos) << " ";
  }
  std::cout << VideoFile::read16(blockPos) << std::endl;
  // RESERVRED 10 BYTES

  // TRANSFORMATION MATRIX
  std::cout << "--Matrix Structure:" << std::endl;
  std::cout << "   " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
  std::cout << "   " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
  std::cout << "   " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
  // TRANSFORMATION MATRIX

  // QUICKTIME PREVIEW
  unsigned int previewStartTime = VideoFile::read32(blockPos);
  unsigned int previewDuration = VideoFile::read32(blockPos);
  std::cout << "--Preview Time: " << previewStartTime << " (";
  Convert::convertDuration(previewStartTime, timeUnit); 
  std::cout << ")" << std::endl;
  std::cout << "--Preview Duration: " << previewDuration << " (";
  Convert::convertDuration(previewDuration, timeUnit);
  std::cout << ")" << std::endl;
  // QUICKTIME PREVIEW

  // QUICKTIME STILL POSTER
  unsigned int stillPosterTime = VideoFile::read32(blockPos);
  std::cout << "--Poster Time: " << stillPosterTime << " (";
  Convert::convertDuration(stillPosterTime, timeUnit);
  std::cout << ")" << std::endl;
  // QUICKTIME STILL POSTER

  // QUICKTIME SELECTION TIME
  unsigned int selectionStartTime = VideoFile::read32(blockPos);
  unsigned int selectionDuration = VideoFile::read32(blockPos);
  std::cout << "--Selection Time: " << selectionStartTime << " (";
  Convert::convertDuration(selectionStartTime, timeUnit); 
  std::cout << ")" << std::endl;
  std::cout << "--Selection Duration: " << selectionDuration << " (";
  Convert::convertDuration(selectionDuration, timeUnit);
  std::cout << ")" << std::endl;
  // QUICKTIME SELECTION TIME

  // QUICKTIME CURRENT TIME
  unsigned int currentTime = VideoFile::read32(blockPos);
  std::cout << "--Current Time: " << currentTime << " (";
  Convert::convertDuration(currentTime, timeUnit);
  std::cout << ")" << std::endl;
  // QUICKTIME CURRENT TIME

  // NEXT TRACK ID
  std::cout << "--Next Track ID: " << VideoFile::read32(blockPos) << std::endl;
  // NEXT TRACK ID

}

// trak decode
void BlockDecode::trak::decode(unsigned int blockBase, unsigned int blockSize)
{
  // Current position within the box
  unsigned int blockPos = 8;

  while(blockPos < blockSize - 1){
    Block blockInfo = VideoFile::getBlock();
    std::cout << "--Block Size: " << blockInfo.blockSize << " bytes, Block Type: " << blockInfo.blockType << std::endl;
    // Branch into different blocks
    switch (BlockDecode::getBlockMapping(blockInfo.blockType))
    {
      case type_tkhd:
        BlockDecode::tkhd::decode(blockInfo.blockSize);
        break;
      case type_mdia:
        BlockDecode::mdia::decode(blockBase + blockPos, blockInfo.blockSize);
        break;
      default:
        break;
    }

    blockPos += blockInfo.blockSize;
    VideoFile::setFilePos(blockBase + blockPos);
  }
}


//udta decode
void BlockDecode::udta::decode(unsigned int blockSize)
{ 
  // Current position within the box
  unsigned int blockPos = 8;
  
  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "--Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "--Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS

  // USER DATA LIST
  std::vector<char> userData = VideoFile::readBytes(3, blockPos);
  unsigned int user = userData[0];
  
  std::cout << "--User Data: " << user << std::endl;
  // USER DATA LIST
 

}

void BlockDecode::mdat::decode(unsigned int blockBase, unsigned int blockSize)
{
  // Current position within the box
  unsigned int blockPos = 8;
  int i = 1;
  if(blockPos < blockSize){
    while(blockPos < blockSize){
      VideoFile::read32(blockPos);
    }
  }
}

// tkhd decode
void BlockDecode::tkhd::decode(unsigned int blockSize)
{
  // Current position within the box
  unsigned int blockPos = 8;

  // If version is 1, date and duration values are 8 bytes long, otherwise 4.

  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "---Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "---Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS

  // CREATED MODIFIED TIME
  std::cout << "---Creation Time: ";
  version ? Convert::convertDate(VideoFile::read64(blockPos)) 
          : Convert::convertDate(VideoFile::read32(blockPos));
  std::cout << std::endl;
  std::cout << "---Modification Time: ";
  version ? Convert::convertDate(VideoFile::read64(blockPos)) 
          : Convert::convertDate(VideoFile::read32(blockPos));
  std::cout << std::endl;
  // CREATED MODIFIED TIME

  // TRACK ID
  std::cout << "---Track ID: " << VideoFile::read32(blockPos) << std::endl;
  // TRACK ID

  // RESERVED 4 BYTES
  std::cout << "---Reserved: " << VideoFile::read32(blockPos) << std::endl;
  // RESERVED 4 BYTES

  // DURATION
  unsigned int duration = version ? VideoFile::read64(blockPos) : VideoFile::read32(blockPos);
  std::cout << "---Duration: " << duration << " (";
  Convert::convertDuration(duration, timeUnit);
  std::cout << ")" << std::endl;
  // DURATION

  // RESERVED 8 BYTES
  std::cout << "---Reserved: ";
  std::cout << VideoFile::read64(blockPos) << std::endl;
  // RESERVED 8 BYTES

  // VIDEO LAYER
  std::cout << "---Layer: " << Convert::convertFixedPoint(VideoFile::read16(blockPos), 8) << std::endl;
  // VIDEO LAYER

  // ALTERNATE TRACK ID
  std::cout << "---Alternate Group: " << VideoFile::read16(blockPos) << std::endl;
  // ALTERNATE TRACK ID

  // TRACK VOLUME
  std::cout << "---Volume: " << Convert::convertFixedPoint(VideoFile::read16(blockPos), 8) << std::endl;
  // TRACK VOLUME

  // RESERVED 2 BYTES
  std::cout << "---Reserved: " << VideoFile::read16(blockPos) << std::endl;
  // RESERVED 2 BYTES

  // TRANSFORMATION MATRIX
  std::cout << "---Matrix Structure:" << std::endl;
  std::cout << "   " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
  std::cout << "   " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
  std::cout << "   " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << " " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
  // TRANSFORMATION MATRIX

  // VIDEO FRAME SIZE
  std::cout << "---Track Width: " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
  std::cout << "---Track Height: " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
  // VIDEO FRAME SIZE
 
}

// mdia decode
void BlockDecode::mdia::decode(unsigned int blockBase, unsigned int blockSize)
{ 
  //Current position within the box
  unsigned int blockPos = 8;

  while(blockPos < blockSize - 1){
    Block blockInfo = VideoFile::getBlock();
    std::cout << "--Block Size: " << blockInfo.blockSize << " bytes, Block Type: " << blockInfo.blockType << std::endl;
  
    //Branch into different blocks
    switch (BlockDecode::getBlockMapping(blockInfo.blockType))
    {
    case type_mdhd: 
      BlockDecode::mdhd::decode(blockInfo.blockSize);
      break;
    case type_hdlr:
      BlockDecode::hdlr::decode(blockInfo.blockSize);
      break;
    case type_minf:
      BlockDecode::minf::decode(blockBase + blockPos, blockInfo.blockSize);
      break;
    default:
      break;
    }

  blockPos += blockInfo.blockSize;
  VideoFile::setFilePos(blockBase + blockPos);  
 }
}

// mdhd decode
void BlockDecode::mdhd::decode(unsigned int blockSize)
{  
  // Current position within the box
  unsigned int blockPos = 8;

  // If version is 1, date and duration values are 8 bytes long, otherwise 4.

  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "---Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "---Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS

  // CREATED MODIFIED TIME
  std::cout << "---Creation Time: ";
  version ? Convert::convertDate(VideoFile::read64(blockPos)) 
          : Convert::convertDate(VideoFile::read32(blockPos));
  std::cout << std::endl;
  std::cout << "---Modification Time: ";
  version ? Convert::convertDate(VideoFile::read64(blockPos)) 
          : Convert::convertDate(VideoFile::read32(blockPos));
  std::cout << std::endl;
  // CREATED MODIFIED TIME
 
  //Time Length
  std::cout << "---Time Length: ";
  version ? Convert::convertDate(VideoFile::read64(blockPos))
          : Convert::convertDate(VideoFile::read32(blockPos));
  std::cout << std::endl;
  //Time Length
  
  //Time Length
  std::cout << "---Time Length Modified: ";
  version ? Convert::convertDate(VideoFile::read64(blockPos))
          : Convert::convertDate(VideoFile::read32(blockPos));
  std::cout << std::endl;
  //Time Length
  
  //Quicktime Quality
  std::cout << "---Quicktime Quality: " << VideoFile::read32(blockPos) << std::endl;
  //Quicktime Quality
  
}

// hdlr decode
void BlockDecode::hdlr::decode(unsigned int blockSize)
{
  unsigned int blockPos = 8;

  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "---Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "---Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS  
  
  //Types 4-bytes
  std::cout << "---Types: " << VideoFile::readString(3, blockPos) << std::endl;
  // Types

  // Subtype/Media Type
  std::cout << "---Subtype/Media Type: " << VideoFile::readString(5, blockPos) << std::endl;
  // Subtype/Media Type

  // Manufacturer
  std::vector<char> manBuffer = VideoFile::readBytes(3, blockPos);
  unsigned int mantypes = manBuffer[0];
  std::cout << "---Manufacturer: " << mantypes << std::endl;
  // Manufacturer      

  // Reserved FLAGS 4-bytes
  std::cout << "---Reserved Flags Mask: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // Reserved Flags   

  // Reserved Flags Mask 4-bytes
  std::cout << "---Reserved Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // Reserved Flags Mask

  //Byte Padding 1-bytes
  std::vector<char> padBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int pad = padBuffer[0];
  std::cout << "---Padding: " << pad << std::endl;
  // Types     
  std::cout << "Still in error: " << blockPos << std::endl; 
  
}

// minf decode
void BlockDecode::minf::decode(unsigned int blockBase, unsigned int blockSize)
{ 
  // Current position within the box
  unsigned int blockPos = 8;

  while(blockPos < blockSize - 1){
    Block blockInfo = VideoFile::getBlock();
    std::cout << "---Block Size: " << blockInfo.blockSize << " bytes, Block Type: " << blockInfo.blockType << std::endl;

    switch (BlockDecode::getBlockMapping(blockInfo.blockType))
    {
    case type_vmhd:
      BlockDecode::vmhd::decode(blockInfo.blockSize);
      break;
    case type_smhd:
      BlockDecode::smhd::decode(blockInfo.blockSize);
      break;
    case type_dinf:
      BlockDecode::dinf::decode(blockBase + blockPos, blockInfo.blockSize);
      break;
    case type_stbl:
      BlockDecode::stbl::decode(blockBase + blockPos, blockInfo.blockSize);
      break;
    default:
      break;
  }

    blockPos += blockInfo.blockSize;
    VideoFile::setFilePos(blockBase + blockPos);
  }
}

// vmhd decode
void BlockDecode::vmhd::decode(unsigned int blockSize)
{ 
  // Current position within the box
  unsigned int blockPos = 8;

  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "----Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "----Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS

  // Graphics Mode
  std::cout << "----Graphics Mode: ";
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(2, blockPos)){
  // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c$
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // Graphics Mode                                 

  // OpColor - 6 bytes = 3 * short unsigned RGB color values
  std::cout << "----Graphics color Mode (R): " << VideoFile::read16(blockPos) << std::endl;
  std::cout << "----Graphics color Mode (G): " << VideoFile::read16(blockPos) << std::endl;
  std::cout << "----Graphics color Mode (B): " << VideoFile::read16(blockPos) << std::endl;
  // OpColor
  
}

void BlockDecode::smhd::decode(unsigned int blockSize)
{
  unsigned int blockPos = 8;
  
  // Version - 1 byte = hex version
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "----Version: " << version << std::endl;
  // Version
 
  // Flags - 3 bytes = hex flags
    std::cout << "----Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;                                 
  // Flags

  // Audio Balance - 2 bytes = short fixed point value
  std::cout << "----Audio Balance: " << VideoFile::read32(blockPos) << std::endl; 
  // Audio Balance

  // Reserved - 2 bytes = short value set to zero
  std::cout << "----Reserved: " << VideoFile::read16(blockPos) << std::endl;
  // Reserved
  
}

// dinf decode
void BlockDecode::dinf::decode(unsigned int blockBase, unsigned int blockSize)
{ 
 // Current position within the box
 unsigned int blockPos = 8;
 
 // Branch into different blocks
 while(blockPos < blockSize -1){
   Block blockInfo = VideoFile::getBlock();
   std::cout << "----Block Size: " << blockInfo.blockSize << " bytes, Block Type: " << blockInfo.blockType << std::endl; 
   switch (BlockDecode::getBlockMapping(blockInfo.blockType)){
     case type_dref:
       BlockDecode::dref::decode(blockInfo.blockSize);
       break;
     default:
       break;
    }

   blockPos += blockInfo.blockSize;
   VideoFile::setFilePos(blockBase + blockPos);
  }
}

// stbl decode
void BlockDecode::stbl::decode(unsigned int blockBase, unsigned int blockSize)
{   
  //Current position within the box
  unsigned int blockPos = 8;
  
  while(blockPos < blockSize -1){
    Block blockInfo = VideoFile::getBlock();
    std::cout << "----Block Size:: " << blockInfo.blockSize << " bytes, Block Type: " << blockInfo.blockType << std::endl;
  // Branch into different blocks
  switch (BlockDecode::getBlockMapping(blockInfo.blockType))
  {
  case type_stsd:
    BlockDecode::stsd::decode(blockInfo.blockSize);
    break;
  case type_stts:
    BlockDecode:stts::decode(blockInfo.blockSize);
    break;
  case type_stss:
    BlockDecode::stss::decode(blockInfo.blockSize);
    break;
  case type_stsz:
    BlockDecode::stsz::decode(blockInfo.blockSize);
    break;
  case type_stsc:
    BlockDecode::stsc::decode(blockInfo.blockSize);
    break;
  case type_stco:
    BlockDecode::stco::decode(blockInfo.blockSize);
    break;
  default:
    break;
  }

  blockPos += blockInfo.blockSize;
  VideoFile::setFilePos(blockBase + blockPos);
 }
}

// dref decode
void BlockDecode::dref::decode(unsigned int blockSize)
{ 
  // Current position within the box
  unsigned int blockPos = 8;
 
  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "-----Version: " << version << std::endl;
  // VERSION 

  //Flags - 3 bytes = hex flags
  std::cout << "-----Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // Flags

  // RESERVED 4 BYTES
  std::cout << "-----Reference: " << VideoFile::read32(blockPos) << std::endl;
  // RESERVED 4 BYTES
 
  // Reserved Flags Mask 4-bytes
  std::cout << "---Reserved Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(7, blockPos)){
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // Reserved Flags Mask

  // VERSION
  std::vector<char> versBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int vers = versBuffer[0];
  std::cout << "-----Version: " << vers << std::endl;
  // VERSION 

  //Flags - 3 bytes = hex flags
  std::cout << "-----Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // Flags  
 
  // URL C String
  std::cout << "-----URL C String: " << VideoFile::readString(1, blockPos) << std::endl;
  // URL C String

}

// stsd decode
void BlockDecode::stsd::decode(unsigned int blockSize)
{ 

  // Current position within the box
  unsigned int blockPos = 8;
  
  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "-----Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "-----Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS
  
  //Descriptions 4-bytes
  std::cout << "-----Description: " << VideoFile::read32(blockPos) << std::endl;
  //Description 
 
  //Length 4-bytes
  std::cout << "-----Length: " << VideoFile::read32(blockPos) << std::endl;
  //Length

  //Visual or Audio Format 4-bytes
  std::string label = VideoFile::readString(4, blockPos);
  if(label == "avc1"){
    std::cout << "-----Audio Format: " << label << std::endl;
    
    //Reserved 6-bytes
    std::vector<char> resBuffer = VideoFile::readBytes(5, blockPos);
    unsigned int reserved = resBuffer[0];
    std::cout << "-----Reserved: " << reserved << std::endl;
    //Reserved

    //Reserved 2-bytes
    std::cout << "-----Data Reference: " << VideoFile::read16(blockPos) << std::endl;
    //Reserved

    //Quicktime Audio Encoding Version - 2-bytes
    std::cout << "-----Video Encoding Version: " << VideoFile::read16(blockPos) << std::endl;
    //Quicktime Audio Encoding Version

    //Video Encoding Revision - 2 bytes
    std::cout << "-----Video Encoding Revision: ";
    std::cout << std::hex;
    for(auto c : VideoFile::readBytes(1, blockPos)){
      std::cout << +c;
    }
    std::cout << std::endl;
    std::cout << std::dec;
    //Video Encoding Revision

    //Quicktime Video Encoding Revision Level - 4-bytes
    std::cout << "-----Video Encoding Vendor: ";
    // Change output to hex
    std::cout << std::hex;
    for(auto c : VideoFile::readBytes(3, blockPos)){
      std::cout << +c;
    }
    std::cout << std::endl;
    std::cout << std::dec;                                     
    //Quicktime Video Encoding Revision Level

    //Video Temporal Quality - 4-bytes
    std::cout << "-----Video Temporal Quality " << VideoFile::read32(blockPos) << std::endl;
    //Video Temporal Quality

    //Video Spatial Quality - 4-bytes
    std::cout << "-----Video Spatial Quality: " << VideoFile::read32(blockPos) << std::endl;
    //Video Spatial Quality

    //Video Frame Pixel Size - 4-bytes
    std::cout << "-----Video Frame Pixel Size: " << VideoFile::read32(blockPos) << std::endl;
    //Video Frame Pixel Size

    //Audio Sample Rate - 2-bytes
    std::cout << "-----Audio Sample Rate: " << VideoFile::read32(blockPos) << std::endl;
    //Audio Sample Rate
    
    // VIDEO FRAME SIZE
    std::cout << "---Track Width: " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
    std::cout << "---Track Height: " << Convert::convertFixedPoint(VideoFile::read32(blockPos), 16) << std::endl;
    // VIDEO FRAME SIZE       
  }
  //Visual Format

  std::cout << blockPos << std::endl;  
}

// stts decode
void BlockDecode::stts::decode(unsigned int blockSize)
{

  //Current position within the box
  unsigned int blockPos = 8;

  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "-----Version: " << version << std::endl;
  // VERSION
 
  // FLAGS
  std::cout << "-----Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
  // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS

  //Number of Blocks - 4-bytes
  std::cout << "-----Number of times: " << VideoFile::read32(blockPos) << std::endl;
  //Number of Blocks
 
  //Times per frame - 4-bytes
  std::cout << "-----Time per frame: " << VideoFile::read64(blockPos) << std::endl;
  //Times per frame
  std::cout << "-----Time per frame: " << VideoFile::read64(blockPos) << std::endl;
  
}

// stss decode
void BlockDecode::stss::decode(unsigned int blockSize)
{ 
  // Current position within the box
  unsigned int blockPos = 8;
  
  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "-----Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "-----Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS
  
  //Number of Key Frames - 4-bytes
  std::cout << "-----Number of Key Frames: " << VideoFile::read32(blockPos) << std::endl;
  //Number of Key Frames

  //Intraframe Location - 4-bytes
  std::cout << "-----Number of Intraframes: " << VideoFile::read32(blockPos) << std::endl;
  //Intraframe Location

}

// stsc decode
void BlockDecode::stsc::decode(unsigned int blockSize)
{ 

  // Current position within the box
  unsigned int blockPos = 8;
 
  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "-----Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "-----Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS
 
  //Frames per block - 8-bytes
  std::cout << "-----Frames per block: " << VideoFile::read64(blockPos) << std::endl;
  //Frames per block

  //Samples Description ID - 4-bytes
  std::cout << "-----Sample Description ID: " << VideoFile::read32(blockPos) << std::endl;      
  //Samples Description ID

}

// stsz decode
void BlockDecode::stsz::decode(unsigned int blockSize)
{ 

  // Current position within the box
  unsigned int blockPos = 8;
 
  // VERSION
  std::vector<char> versionBuffer = VideoFile::readBytes(1, blockPos);
  unsigned int version = versionBuffer[0];
  std::cout << "-----Version: " << version << std::endl;
  // VERSION

  // FLAGS
  std::cout << "-----Flags: ";
  // Change output to hex
  std::cout << std::hex;
  for(auto c : VideoFile::readBytes(3, blockPos)){
    // https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c$
    if(c < 0x10){
      std::cout << 0;
    }
    std::cout << +c;
  }
  std::cout << std::endl;
  std::cout << std::dec;
  // FLAGS

  //Block Byte Size for All - 4-bytes
  std::cout << "-----Block Byte Size for All: " << VideoFile::read32(blockPos) << std::endl;
  //Block Byte Size for All
 
  //Number of Block Size - 4-bytes
  std::cout << "-----Number of Block Size: " << VideoFile::read32(blockPos) << std::endl;
  //Number of Block Size 
 
  //Block Byte Size - 4-bytes
  std::vector<char> sampleBuffer = VideoFile::readBytes(3 , blockPos);
  unsigned int sample = sampleBuffer[0];
  std::cout << "-----Block Byte Size: " << sample << std::endl;
  //Block Byte Size

}

//stco decode
void BlockDecode::stco::decode(unsigned int blockSize)
{
  // Current position within the box
  unsigned int blockPos = 8;

  //Number of Offsets - 4-bytes
  std::cout << "-----Number of Offsets: " << VideoFile::read32(blockPos) << std::endl;
  //Number of Offsets
  
  //Block Offsets - 4-bytes
  std::vector<char> blockBuffer = VideoFile::readBytes(3, blockPos);
  unsigned int block = blockBuffer[0];
  std::cout << "-----Block Buffer: " << block << std::endl;
  //Block Offsets
}
