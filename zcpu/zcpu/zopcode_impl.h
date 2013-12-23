#ifndef ZOPCODE_IMPL_H
#define ZOPCODE_IMPL_H

#include "zopcode.h"
#include "zcpu.h"

namespace ZOpcodeImpl{
	void registerJumpFlag(int* f);
	void registerJumpOffset(int* o);
	void registerZCpuObj(ZCpu* o);
	int JE(ZOpcode& zOp);
	int JL(ZOpcode& zOp);
	int JG(ZOpcode& zOp);
	int DEC_CHK(ZOpcode& zOp);
}

#endif
