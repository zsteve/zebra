#include "zopcode.h"
#include "../../zglobal/zglobal.h"
#include "../../zglobal/zglobaldefines.h"
#include "../../zmemory/zmemory/zmemory.h"

using namespace ZCpuOps;

ZOpcode::ZOpcode() : operandTypes(0){
    this->operandCount=ZOPCOUNT_0;
    this->opcodeName=UNDEFINED;
    this->opcodeType=ZOPTYPE_LONG;
};

ZOpcode::ZOpcode(ulong addr, ZMemory& zMem) : operandTypes(0){
    decodeOp(addr, zMem);
};

/**
 * decodes opcode
 */
ZOpcode::decodeOp(ulong addr, ZMemory& zMem){
    // first, we must look up the opcode number
};
