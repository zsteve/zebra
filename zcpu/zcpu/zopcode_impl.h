#ifndef ZOPCODE_IMPL_H
#define ZOPCODE_IMPL_H

#include "zopcode.h"
#include "zcpu.h"

namespace ZOpcodeImpl{
	const int JUMP_NONE=0;
	const int JUMP_OFFSET=1;
	const int JUMP_POSITION=2;
	void registerJumpFlag(int* f);
	void registerJumpValue(szword* o);
	void registerZCpuObj(ZCpu* o);
	// 2OP
	int JE(ZOpcode& zOp);
	int JL(ZOpcode& zOp);
	int JG(ZOpcode& zOp);
	int DEC_CHK(ZOpcode& zOp);
	int INC_CHK(ZOpcode& zOp);
	int JIN(ZOpcode& zOp);
	int TEST(ZOpcode& zOp);
	int OR(ZOpcode& zOp);
	int AND(ZOpcode& zOp);
	int TEST_ATTR(ZOpcode& zOp);
	int SET_ATTR(ZOpcode& zOp);
	int CLEAR_ATTR(ZOpcode& zOp);
	int STORE(ZOpcode& zOp);
	int INSERT_OBJ(ZOpcode& zOp);
	int LOADW(ZOpcode& zOp);
	int LOADB(ZOpcode& zOp);
	int GET_PROP(ZOpcode& zOp);
	int GET_PROP_ADDR(ZOpcode& zOp);
	int GET_NEXT_PROP(ZOpcode& zOp);
	int ADD(ZOpcode& zOp);
	int SUB(ZOpcode& zOp);
	int MUL(ZOpcode& zOp);
	int DIV(ZOpcode& zOp);
	int MOD(ZOpcode& zOp);
	int CALL_2S(ZOpcode& zOp);
	int CALL_2N(ZOpcode& zOp);
	int SET_COLOUR_FB(ZOpcode& zOp);
	int THROW(ZOpcode& zOp);

	// 1OP
	int JZ(ZOpcode& zOp);
	int GET_SIBLING(ZOpcode& zOp);
	int GET_CHILD(ZOpcode& zOp);
	int GET_PARENT(ZOpcode& zOp);
	int GET_PROP_LEN(ZOpcode& zOp);
	int INC(ZOpcode& zOp);
	int DEC(ZOpcode& zOp);
	int PRINT_ADDR(ZOpcode& zOp);
	int CALL_1S(ZOpcode& zOp);
	int REMOVE_OBJ(ZOpcode& zOp);
	int PRINT_OBJ(ZOpcode& zOp);
	int RET(ZOpcode& zOp);
	int JUMP(ZOpcode& zOp);
	int PRINT_PADDR(ZOpcode& zOp);
	int LOAD(ZOpcode& zOp);
	int NOT(ZOpcode& zOp);
	int CALL_1N(ZOpcode& zOp);

	// 0OP
	int RTRUE(ZOpcode& zOp);
	int RFALSE(ZOpcode& zOp);
	int PRINT(ZOpcode& zOp);
	int PRINT_RET(ZOpcode& zOp);
	int NOP(ZOpcode& zOp);
	int SAVE_LABEL(ZOpcode& zOp);
	int SAVE_RESULT(ZOpcode& zOp);
	int RESTART(ZOpcode& zOp);
	int RET_POPPED(ZOpcode& zOp);
	int POP(ZOpcode& zOp);
	int CATCH(ZOpcode& zOp);
	int QUIT(ZOpcode& zOp);
	int NEW_LINE(ZOpcode& zOp);
	int SHOW_STATUS(ZOpcode& zOp);
	int VERIFY(ZOpcode& zOp);
	int PIRACY(ZOpcode& zOp);
	// VAROP
	int CALL(ZOpcode& zOp);
	int CALL_VS(ZOpcode& zOp);
	int STOREW(ZOpcode& zOp);
	int STOREB(ZOpcode& zOp);
	int PUT_PROP(ZOpcode& zOp);
	int READ(ZOpcode& zOp);
	int PRINT_CHAR(ZOpcode& zOp);
	int PRINT_NUM(ZOpcode& zOp);
	int RANDOM(ZOpcode& zOp);
	int PUSH(ZOpcode& zOp);
	int PULL(ZOpcode& zOp);
	int SPLIT_WINDOW(ZOpcode& zOp);
	int SET_WINDOW(ZOpcode& zOp);
	int CALL_VS2(ZOpcode& zOp);
	int ERASE_WINDOW(ZOpcode& zOp);
	int ERASE_LINE(ZOpcode& zOp);
	int SET_CURSOR(ZOpcode& zOp);
	int GET_CURSOR(ZOpcode& zOp);
	int SET_TEXT_STYLE(ZOpcode& zOp);
	int BUFFER_MODE(ZOpcode& zOp);
	int OUTPUT_STREAM_N(ZOpcode& zOp);
	int INPUT_STREAM(ZOpcode& zOp);
	int SOUND_EFFECT(ZOpcode& zOp);
	int READ_CHAR(ZOpcode& zOp);
	int SCAN_TABLE(ZOpcode& zOp);
	int NOT_V(ZOpcode& zOp);
	int CALL_VN(ZOpcode& zOp);
	int CALL_VN2(ZOpcode& zOp);
	int TOKENISE(ZOpcode& zOp);
	int ENCODE_TEXT(ZOpcode& zOp);
	// EXT
	int LOG_SHIFT(ZOpcode& zOp);
}

#endif
