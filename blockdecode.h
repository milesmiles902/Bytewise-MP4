// Decode functions

#ifndef BLOCK_DECODE_H
#define BLOCK_DECODE_H

#include <string>

namespace BlockDecode
{
  enum BlockType
  {
    type_ftyp = 1,
    type_moov,
    type_mvhd,
    type_trak,
    type_udta,
    type_mdat,
    type_tkhd,
    type_mdia,
    type_mdhd,
    type_hdlr,
    type_minf,
    type_vmhd,
    type_smhd,
    type_dinf,
    type_stbl,
    type_dref,
    type_stsd,
    type_stts,
    type_stss,
    type_stsc,
    type_stsz,
    type_stco,
  };

  // Returns the enum mapping of the given string
  BlockType getBlockMapping(std::string blockType);

  // Functions for ftyp block
  namespace ftyp
  {
    void decode(unsigned int blockSize);
  }

  // Functions for moov block
  namespace moov
  {
    void decode(unsigned int blockBase, unsigned int blockSize);
  }

  // Functions for mvhd block
  namespace mvhd
  {
    void decode(unsigned int blockSize);
  }

  // Functions for trak block
  namespace trak
  {
    void decode(unsigned int blockBase, unsigned int blockSize);
  }

  // Functions for udta block
  namespace udta
  {
    void decode(unsigned int blockSize);
  }

  // Functions for mdat block
  namespace mdat
  {
    void decode(unsigned int blockBase, unsigned int blockSize);
  }
  
  // Functions for tkhd block
  namespace tkhd
  {
    void decode(unsigned int blockSize);
  }

  // Functions for mdia block
  namespace mdia
  {
    void decode(unsigned int blockBase, unsigned int blockSize);
  }

  // Functions for mdhd block
  namespace mdhd
  {
    void decode(unsigned int blockSize);
  }  

  // Functions for smhd block
  namespace smda
  {
    void decode(unsigned int blockSize);
  }

  namespace hdlr
  {
    void decode(unsigned int blockSize);
  }

  namespace minf
  {
    void decode(unsigned int blockBase, unsigned int blockSize);
  } 

  namespace vmhd
  {
    void decode(unsigned int blockSize);
  }

  namespace smhd
  {
    void decode(unsigned int blockSize);
  }
  
  namespace dinf
  { 
    void decode(unsigned int blockBase, unsigned int blockSize);
  }
  
  namespace stbl
  {
    void decode(unsigned int blockBase, unsigned int blockSize);
  }

  namespace dref
  {
    void decode(unsigned int blockSize);
  }
  
  namespace stsd
  {
    void decode(unsigned int blockSize);
  }
  
  namespace stts
  {
    void decode(unsigned int blockSize);
  }

  namespace stss
  {
    void decode(unsigned int blockSize);
  }

  namespace stsc
  {
    void decode(unsigned int blockSize);
  }
  
  namespace stsz
  {
    void decode(unsigned int blockSize);
  }
  
  namespace stco
  {
    void decode(unsigned int blockSize);
  }
}

#endif
