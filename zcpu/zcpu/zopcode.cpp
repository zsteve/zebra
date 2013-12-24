#include "zopcode.h"
#include "../../zglobal/zglobal.h"
#include "../../zglobal/zglobaldefines.h"
#include "../../zmemory/zmemory/zmemory.h"
#include "../../zstack/zstack/zstack.h"

namespace ZCpuOps{
	zbyte ZOpcodeNums_2[]={
		0x0,
		0x1,
		0x2,
		0x3,
		0x4,
		0x5,
		0x6,
		0x7,
		0x8,
		0x9,
		0xa,
		0xb,
		0xc,
		0xd,
		0xe,
		0xf,
		0x10,
		0x11,
		0x12,
		0x13,
		0x14,
		0x15,
		0x16,
		0x17,
		0x18,
		0x19,
		0x1a,
		0x1b,
		0x1c
	};

	zbyte ZOpcodeNums_1[]={
		0x0,
		0x1,
		0x2,
		0x3,
		0x4,
		0x5,
		0x6,
		0x7,
		0x8,
		0x9,
		0xa,
		0xb,
		0xc,
		0xd,
		0xe,
		0xf
	};

	zbyte ZOpcodeNums_0[]={
		0x0,
		0x1,
		0x2,
		0x3,
		0x4,
		0x5,
		0x6,
		0x7,
		0x8,
		0x9,
		0xa,
		0xb,
		0xc,
		0xd,
		0xe,
		0xf
	};

	zbyte ZOpcodeNums_VAR[]={
		0x0,
		0x1,
		0x2,
		0x3,
		0x4,
		0x5,
		0x6,
		0x7,
		0x8,
		0x9,
		0xa,
		0xb,
		0xc,
		0xd,
		0xe,
		0xf,
		0x10,
		0x11,
		0x12,
		0x13,
		0x14,
		0x15,
		0x16,
		0x17,
		0x18,
		0x19,
		0x1a,
		0x1b,
		0x1c,
		0x1d,
		0x1e,
		0x1f
	};

	zbyte ZOpcodeNums_EXT[]={
		0x0,
		0x1,
		0x2,
		0x3,
		0x4,
		0x5,
		0x6,
		0x7,
		0x8,
		0x9,
		0xa,
		0xb,
		0xc,
		0xd,
		0xe,
		0xf,
		0x10,
		0x11,
		0x12,
		0x13,
		0x14,
		0x15,
		0x16,
		0x17,
		0x18,
		0x19,
		0x1a,
		0x1b,
		0x1c
	};
}

using namespace ZCpuOps;

ZOpcode::ZOpcode() : operandTypes(0){
    this->operandCount=ZOPCOUNT_0;
    this->opcodeName=NULL;
    this->opcodeType=ZOPTYPE_LONG;
};

ZOpcode::ZOpcode(ulong addr, ZMemory& zMem) : operandTypes(0){
    decodeOp(addr, zMem);
};

/**
 * returns an integer (index into the ZOpcode_<op_type_here> array)
 * which can be converted to a ZOpcodeName enum
 * @param zOp opcode byte
 * @throw ZException on invalid opcode byte
 */
int ZOpcode::getZOpcodeName(zbyte zOp) throw (ZException){
	if(opcodeType==ZOPTYPE_EXT){
		for(int i=0; i<sizeof(ZOpcodeNums_EXT); i++){
			if(ZOpcodeNums_EXT[i]==zOp){
				return i;
			}
		}
		throw ZException();
	}else if(opcodeType==ZOPTYPE_VAR){
		for(int i=0; i<sizeof(ZOpcodeNums_VAR); i++){
			if(ZOpcodeNums_VAR[i]==zOp){
				return i;
			}
		}
		throw ZException();
	}else{
		// operand count 0, 1, 2
		if(operandCount==0){
			for(int i=0; i<sizeof(ZOpcodeNums_0); i++){
				if(ZOpcodeNums_0[i]==zOp){
					return i;
				}
			}
			throw ZException();
		}else if(operandCount==1){
			for(int i=0; i<sizeof(ZOpcodeNums_1); i++){
				if(ZOpcodeNums_1[i]==zOp){
					return i;
				}
			}
			throw ZException();
		}else if(operandCount==2){
			for(int i=0; i<sizeof(ZOpcodeNums_2); i++){
				if(ZOpcodeNums_2[i]==zOp){
					return i;
				}
			}
			throw ZException();
		}
	}
	return NULL;
}

/**
 * returns operand type
 * from integer. see Z-Machine Standard, Chapter 4, 4.2
 * @param types type integer
 */
ZOperandType ZOpcode::getOperandType(int types){
	if(types==3){
		// omitted
		return (ZOPERANDTYPE_OMITTED);
	}else if(types==0){
		// large constant
		return (ZOPERANDTYPE_LARGE_CONST);
	}else if(types==1){
		// small constant
		return (ZOPERANDTYPE_SMALL_CONST);
	}else if(types==2){
		// variable
		return (ZOPERANDTYPE_VAR);
	}
}

/*
 * reads an operand
 * @param addr address
 * @param type type of operand
 * @param zMem ZMemory object
 * @param counter counter to increment (because the number of bytes to
 * step ahead by is determined at runtime)
 */
ulong ZOpcode::getOperand(ulong addr, ZOperandType type, ZMemory& zMem, int& counter)
{
	if(type==ZOPERANDTYPE_LARGE_CONST){
		// 16 bit constant
		counter+=2;
		return zMem.readZWord(addr);
	}else if(type==ZOPERANDTYPE_SMALL_CONST){
		// 8 bit constant
		counter++;
		return zMem.readZByte(addr);
	}else if(type==ZOPERANDTYPE_VAR){
		// 8 bit variable
		counter++;
		return zMem.readZByte(addr);
	}
}

bool ZOpcode::opcodeHasBranch(){
	if(opcodeType==ZOPTYPE_EXT){
		// extended opcodes are special
		switch(opcodeName){
		case PICTURE_DATA:
		case PUSH_STACK:
		case MAKE_MENU:
			return true;
			break;
		default:
			break;
		}
	}else{
		switch(operandCount){
		case ZOPCOUNT_0:
			switch(opcodeName){
			case SAVE_LABEL:
			case RESTORE_LABEL:
			case VERIFY:
			case PIRACY:
				return true;
				break;
			default:
				break;
			}
			break;
		case ZOPCOUNT_1:
			switch(opcodeName){
			case JZ:
			case GET_SIBLING:
			case GET_CHILD:
				return true;
				break;
			default:
				break;
			}
			break;
		case ZOPCOUNT_2:
			switch(opcodeName){
			case JE:
			case JL:
			case JG:
			case DEC_CHK:
			case INC_CHK:
			case JIN:
			case TEST:
			case TEST_ATTR:
				return true;
				break;
			default:
				break;
			}
			break;
		case ZOPCOUNT_VAR:
			switch(opcodeName){
			case SCAN_TABLE:
			case CHECK_ARG_COUNT:
				return true;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	return false;
}

bool ZOpcode::opcodeHasStore(){
	if(opcodeType==ZOPTYPE_EXT){
		switch(opcodeName){
		case SAVE:
		case RESTORE:
		case LOG_SHIFT:
		case ART_SHIFT:
		case SET_FONT:
		case SAVE_UNDO:
		case RESTORE_UNDO:
		case GET_WIND_PROP:
			return true;
			break;
		default:
			break;
		}
	}else{
		switch(operandCount){
		case ZOPCOUNT_0:
			switch(opcodeName){
			case CATCH:
				return true;
				break;
			default:
				break;
			}
			break;
		case ZOPCOUNT_1:
			switch(opcodeName){
			case GET_SIBLING:
			case GET_CHILD:
			case GET_PARENT:
			case GET_PROP_LEN:
			case CALL_1S:
			case LOAD:
			case NOT:
				return true;
				break;
			default:
				break;
			}
			break;
		case ZOPCOUNT_2:
			switch(opcodeName){
			case OR:
			case AND:
			case LOADW:
			case LOADB:
			case GET_PROP:
			case GET_PROP_ADDR:
			case GET_NEXT_PROP:
			case ADD:
			case SUB:
			case MUL:
			case DIV:
			case MOD:
			case CALL_2S:
				return true;
				break;
			default:
				break;
			}
			break;
		case ZOPCOUNT_VAR:
			switch(opcodeName){
			case CALL:
			case AREAD:
			case RANDOM:
			case PULL_VAL:
			case CALL_VS2:
			case READ_CHAR:
			case SCAN_TABLE:
			case NOT_V:
			case CHECK_ARG_COUNT:
				return true;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	return false;
}

/**
 * decodes opcode
 * @param addr address of opcode
 * @param zMem ZMemory object reference
 */
void ZOpcode::decodeOp(ulong addr, ZMemory& zMem){
	try{
		// first, we must look up the opcode number
		this->addr=addr;
		opcodeSize=1;	// opcode must be at least 1 byte
		char opCode=zMem.readZByte(addr+0);
		opcodeByte=opCode;
		if(opCode==0xBE){
			// opcode is EXTENDED
			opcodeType=ZOPTYPE_EXT;
			// in extended form, opcount is VAR
			operandCount=ZOPCOUNT_VAR;
			// opcode number is given in next opcode byte
			zbyte opNum=opcodeNum=zMem.readZByte(addr+1);
			// we got a second byte
			opcodeSize++;
			opcodeName=getZOpcodeName(opCode);
		}else{
			int top2Bits=(opCode & (BIT_7 | BIT_6))>>6;
			if(top2Bits==3){
				// top 2 bits are 0b11, opcode form is variable
				opcodeType=ZOPTYPE_VAR;
				// if bit 5 is 0, count is 2OP, if 1, VAR
				int opCount=(opCode & (BIT_5))>>5;
				if(!opCount){
					operandCount=ZOPCOUNT_2;
				}else{
					operandCount=ZOPCOUNT_VAR;
				}
				// opcode number is given in bottom 5 bits
				zbyte opNum=(opCode &
							(BIT_0 |
							 BIT_1 |
							 BIT_2 |
							 BIT_3 |
							 BIT_4));
				opcodeName=opcodeNum=getZOpcodeName(opNum);
			}else if(top2Bits==2){
				// top 2 bits are 0b10, opcode form is SHORT
				opcodeType=ZOPTYPE_SHORT;
				int opCount=(opCode & (BIT_5 | BIT_4))>>4;
				if(opCount==3){
					// if opTypes is 0b11, opcount is 0OP
					operandCount=ZOPCOUNT_0;
				}else{
					// otherwise, opcount is 1OP
					operandCount=ZOPCOUNT_1;
				}
				// opcode number is given in bottom 4 bits
				zbyte opNum=(opCode &
							(BIT_0 |
							BIT_1 |
							BIT_2 |
							BIT_3));
				opcodeName=opcodeNum=getZOpcodeName(opNum);
			}else{
				// otherwise opcode form is LONG
				opcodeType=ZOPTYPE_LONG;
				// in long form, opcount is always 2OP
				operandCount=ZOPCOUNT_2;
				// opcode number is given in bottom 5 bits
				zbyte opNum=(opCode &
							(BIT_0 |
							 BIT_1 |
							 BIT_2 |
							 BIT_3 |
							 BIT_4));
				opcodeName=opcodeNum=getZOpcodeName(opNum);
			}
		}
		// and now we deal with the operand types
		if(opcodeType==ZOPTYPE_SHORT){
			// bits 4 and 5 give opcode type
			int types=(opcodeByte & (BIT_4 | BIT_5))>>4;
			operandTypes.push_back(getOperandType(types));
		}else if(opcodeType==ZOPTYPE_LONG){
			// bit 6 == op1
			// bit 5 == op2
			// value 0 == small constant
			// value 1 == variable
			int types=(opcodeByte & (BIT_6))>>6;
			operandTypes.push_back(types ? ZOPERANDTYPE_VAR : ZOPERANDTYPE_SMALL_CONST);
			types=(opcodeByte & (BIT_5))>>5;
			operandTypes.push_back(types ? ZOPERANDTYPE_VAR : ZOPERANDTYPE_SMALL_CONST);
		}else if(opcodeType==ZOPTYPE_VAR || opcodeType==ZOPTYPE_EXT){
			zbyte typeByte; // separate operand type byte
			if(opcodeType==ZOPTYPE_VAR){
				typeByte=zMem.readZByte(addr+1);
			}else{
				typeByte=zMem.readZByte(addr+2);
			}
			// add another byte to size
			opcodeSize++;
			int types=(typeByte & (BIT_7 | BIT_6))>>6;
			operandTypes.push_back(getOperandType(types));
			types=(typeByte & (BIT_5 | BIT_4))>>4;
			operandTypes.push_back(getOperandType(types));
			types=(typeByte & (BIT_3 | BIT_2))>>2;
			operandTypes.push_back(getOperandType(types));
			types=(typeByte & (BIT_1 | BIT_0));
			operandTypes.push_back(getOperandType(types));
			/// TODO : add support for "double variable" VAR opcodes call_vs2 and call_vn2 (opcode numbers 12 and 26)
		}
		// operands are given next
		if(opcodeType==ZOPTYPE_SHORT){
			// short ops can be 0OP or 1OP
			if(opcodeNum==ZOPCOUNT_0){
			}else{
				int read_offset=0;
				for(int i=0; i<operandTypes.size() && operandTypes[i]!=ZOPERANDTYPE_OMITTED && i<1; i++){
					operands.push_back(getOperand((addr+1+read_offset), operandTypes[i], zMem, read_offset));
				}
				opcodeSize+=read_offset;
			}
		}else if(opcodeType==ZOPTYPE_LONG){
			// long ops are always 2OP
			int read_offset=0;
			for(int i=0; i<operandTypes.size() && operandTypes[i]!=ZOPERANDTYPE_OMITTED && i<2; i++){
				operands.push_back(getOperand((addr+1+read_offset), operandTypes[i], zMem, read_offset));
			}
			opcodeSize+=read_offset;
		}else if(opcodeType==ZOPTYPE_EXT || opcodeType==ZOPTYPE_VAR){
			// extended ops are VAR OP
			int read_offset=0;
			for(int i=0; i<operandTypes.size() && operandTypes[i]!=ZOPERANDTYPE_OMITTED; i++){
				operands.push_back(getOperand((addr+2+read_offset), operandTypes[i], zMem, read_offset));
			}
			opcodeSize+=read_offset;
		}
		/*	now we need to check for branch/no branch
			otherwise our program will run amok	*/
		if(opcodeHasBranch()){
			// if there's a branch, it resides at 
			// addr+opcodeSize
			branchInfo.hasBranch=true;
			branchInfo.byte1=zMem.readZByte(addr+opcodeSize);
			// there is a second branch byte if bit 6 is not set
			int bit6=(branchInfo.byte1|BIT_6);
			if(!bit6){
				// fetch second byte
				branchInfo.byte2=zMem.readZByte(addr+opcodeSize+1);
				branchInfo.branchSize=2;
			}else{
				branchInfo.branchSize=1;
			}
			branchInfo.branchCond=(bool)(branchInfo.byte1|BIT_7);
			if(branchInfo.branchSize==1){
				branchInfo.branchOffset=(branchInfo.byte1 &
										(BIT_5|
										 BIT_4|
										 BIT_3|
										 BIT_2|
										 BIT_1|
										 BIT_0 ));
			}else if(branchInfo.branchSize==2){
				int offset=(branchInfo.byte1 &
										(BIT_5|
										 BIT_4|
										 BIT_3|
										 BIT_2|
										 BIT_1|
										 BIT_0 ));
				offset<<=8;
				offset|=(branchInfo.byte2);
				branchInfo.branchOffset=offset;
			}
			opcodeSize+=branchInfo.branchSize;
		}
		/* "Store" instructions return a value: e.g., mul multiplies its two operands together.
		Such instructions must be followed by a single byte giving the variable number of
		where to put the result. 
		*/
		if(opcodeHasStore()){
			storeInfo.hasStore=true;
			storeInfo.storeVar=zMem.readZByte(addr+this->opcodeSize);
			opcodeSize++;
		}
	}catch(ZMemoryReadOutOfBounds e){
		throw IllegalZOpcode();
	}catch(ZException e){
		throw IllegalZOpcode();
	}catch(...){
		throw IllegalZOpcode();
	}
}
