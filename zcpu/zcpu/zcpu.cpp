#include "zcpu.h"
#include "zopcode_impl.h"
#include "../../zstack/zstack/zstack.h"
#include "../../zmemory/zmemory/zobject.h"
#include "../../zdictionary/zdictionary/zdictionary.h"

#include <cstdlib>
#include <cstdio>

ZCpu::ZCpu(ZMemory& zMem, ZStack& zStack, ZObjectTable& zObject, ZInOut& zInOut, ZDictionary& zDict)\
	: zMem(zMem), zStack(zStack), zObject(zObject), zInOut(zInOut), zDict(zDict){
	// initial value of program counter is found at 0x6
	// (packed address)
	pCounter=zMem.readZWord(0x6);
	// now we need to initialize the mainRoutine and currentRoutine objects
	zStack.initStack(4096);	// 4kb stack size
	mainRoutine=new ZCpuRoutine();
	mainRoutine->loadAddr(pCounter-1, zMem);
	mainRoutine->createLocalVars(zStack);
	currentRoutine=new ZCpuRoutine(*mainRoutine);
	mainRoutine->loadAddr(pCounter-1, zMem);
	mainRoutine->createLocalVars(zStack);
	*currentRoutine=*mainRoutine;
	haltFlag=false;
}

void ZCpu::incrementPCounter(ZOpcode& zOp){
	pCounter+=zOp.getOpcodeSize();
}

void ZCpu::branchPCounter(int offset){
	// Address after branch data + Offset - 2.
	pCounter+=(offset-2);
}

void ZCpu::setPCounter(ulong pCounter){
	this->pCounter=(ulong)pCounter;
}

/**
 * starts execution
 * does not return until ZCpu::haltFlag
 * is set to true or when the user exits
 * should be run in a separate thread
 * @return undefined
 */

int ZCpu::startExecution(){
	return start();
}

/**
 * private start execution
 * @return as yet undefined
 */
int ZCpu::start(){
	ZOpcode* opCode;
	int i=0;
	while(!haltFlag){
		{
			FILE* f=fopen("dbg_out.txt", "a");
			fprintf(f, "%x\n", pCounter);
			fclose(f);
		}
		opCode=new ZOpcode(pCounter, zMem);
		if(mainLoop(*opCode)!=1){
			incrementPCounter(*opCode);
		}
		delete opCode;
		i++;
	}
	return 0;
}

/**
 * main loop for ZCpu
 * @return 1 if there was a branch
 */
int ZCpu::mainLoop(ZOpcode& zOp){
	int jumpMade=0;
	slong jumpValue=0;
	ZOpcodeImpl::registerJumpFlag(&jumpMade);
	ZOpcodeImpl::registerJumpValue(&jumpValue);
	ZOpcodeImpl::registerZCpuObj(this);
	switch(zOp.getOperandCount()){
	case ZOPCOUNT_0:
		{
			switch(zOp.getOpcodeName()){
			case RTRUE:
				ZOpcodeImpl::RTRUE(zOp);
				break;
			case RFALSE:
				ZOpcodeImpl::RFALSE(zOp);
				break;
			case PRINT:
				ZOpcodeImpl::PRINT(zOp);
				break;
			case PRINT_RET:
				ZOpcodeImpl::PRINT_RET(zOp);
				break;
			case NOP:
				ZOpcodeImpl::NOP(zOp);
				break;
			case SAVE_LABEL:
				ZOpcodeImpl::SAVE_LABEL(zOp);
				break;
			/*case SAVE_RESULT:
				ZOpcodeImpl::SAVE_RESULT(zOp);
				break;*/
			case RESTART:
				ZOpcodeImpl::RESTART(zOp);
				break;
			case RET_POPPED:
				ZOpcodeImpl::RET_POPPED(zOp);
				break;
			case POP:
				ZOpcodeImpl::POP(zOp);
				break;
			/*case CATCH:
				ZOpcodeImpl::CATCH(zOp);
				break;*/
			case QUIT:
				ZOpcodeImpl::QUIT(zOp);
				break;
			case NEW_LINE:
				ZOpcodeImpl::NEW_LINE(zOp);
				break;
			case SHOW_STATUS:
				ZOpcodeImpl::SHOW_STATUS(zOp);
				break;
			case VERIFY:
				ZOpcodeImpl::VERIFY(zOp);
				break;
			case PIRACY:
				ZOpcodeImpl::PIRACY(zOp);
				break;
			default:
				throw IllegalZOpcode();
				break;
			}
		}
		break;
	case ZOPCOUNT_1:
		{
			switch(zOp.getOpcodeName()){
			case JZ:
				ZOpcodeImpl::JZ(zOp);
				break;
			case GET_SIBLING:
				ZOpcodeImpl::GET_SIBLING(zOp);
				break;
			case GET_CHILD:
				ZOpcodeImpl::GET_CHILD(zOp);
				break;
			case GET_PARENT:
				ZOpcodeImpl::GET_PARENT(zOp);
				break;
			case INC:
				ZOpcodeImpl::INC(zOp);
				break;
			case DEC:
				ZOpcodeImpl::DEC(zOp);
				break;
			case PRINT_ADDR:
				ZOpcodeImpl::PRINT_ADDR(zOp);
				break;
			case CALL_1S:
				ZOpcodeImpl::CALL_1S(zOp);
				break;
			case REMOVE_OBJ:
				ZOpcodeImpl::REMOVE_OBJ(zOp);
				break;
			case PRINT_OBJ:
				ZOpcodeImpl::PRINT_OBJ(zOp);
				break;
			case RET:
				ZOpcodeImpl::RET(zOp);
				break;
			case JUMP:
				ZOpcodeImpl::JUMP(zOp);
				break;
			case PRINT_PADDR:
				ZOpcodeImpl::PRINT_PADDR(zOp);
				break;
			case LOAD:
				ZOpcodeImpl::LOAD(zOp);
				break;
			case NOT:
				ZOpcodeImpl::NOT(zOp);
				break;
			case CALL_1N:
				ZOpcodeImpl::CALL_1N(zOp);
				break;
			case GET_PROP_LEN:
				ZOpcodeImpl::GET_PROP_LEN(zOp);
				break;
			default:
				throw IllegalZOpcode();
				break;
			}
		}
		break;
	case ZOPCOUNT_2:
		{
			switch(zOp.getOpcodeName()){
			case JE:
				ZOpcodeImpl::JE(zOp);
				break;
			case JG:
				ZOpcodeImpl::JG(zOp);
				break;
			case JL:
				ZOpcodeImpl::JL(zOp);
				break;
			case DEC_CHK:
				ZOpcodeImpl::DEC_CHK(zOp);
				break;
			case INC_CHK:
				ZOpcodeImpl::INC_CHK(zOp);
				break;
			case JIN:
				ZOpcodeImpl::JIN(zOp);
				break;
			case TEST:
				ZOpcodeImpl::TEST(zOp);
				break;
			case OR:
				ZOpcodeImpl::OR(zOp);
				break;
			case AND:
				ZOpcodeImpl::AND(zOp);
				break;
			case TEST_ATTR:
				ZOpcodeImpl::TEST_ATTR(zOp);
				break;
			case SET_ATTR:
				ZOpcodeImpl::SET_ATTR(zOp);
				break;
			case CLEAR_ATTR:
				ZOpcodeImpl::CLEAR_ATTR(zOp);
				break;
			case STORE:
				ZOpcodeImpl::STORE(zOp);
				break;
			case INSERT_OBJ:
				ZOpcodeImpl::INSERT_OBJ(zOp);
				break;
			case LOADW:
				ZOpcodeImpl::LOADW(zOp);
				break;
			case LOADB:
				ZOpcodeImpl::LOADB(zOp);
				break;
			case GET_PROP:
				ZOpcodeImpl::GET_PROP(zOp);
				break;
			case GET_PROP_ADDR:
				ZOpcodeImpl::GET_PROP_ADDR(zOp);
				break;
			case GET_NEXT_PROP:
				ZOpcodeImpl::GET_NEXT_PROP(zOp);
				break;
			case ADD:
				ZOpcodeImpl::ADD(zOp);
				break;
			case SUB:
				ZOpcodeImpl::SUB(zOp);
				break;
			case MUL:
				ZOpcodeImpl::MUL(zOp);
				break;
			case DIV:
				ZOpcodeImpl::DIV(zOp);
				break;
			case MOD:
				ZOpcodeImpl::MOD(zOp);
				break;
			case CALL_2S:
				ZOpcodeImpl::CALL_2S(zOp);
				break;
			case CALL_2N:
				ZOpcodeImpl::CALL_2N(zOp);
				break;
			case SET_COLOUR_FB:
				ZOpcodeImpl::SET_COLOUR_FB(zOp);
				break;
			case THROW:
				ZOpcodeImpl::THROW(zOp);
				break;
			default:
				throw IllegalZOpcode();
				break;
			}
		}
		break;
	case ZOPCOUNT_VAR:
		{
			switch(zOp.getOpcodeName()){
			case CALL:
				ZOpcodeImpl::CALL(zOp);
				break;
			/*case CALL_VS:
				ZOpcodeImpl::CALL_VS(zOp);
				break;*/
			case STOREW:
				ZOpcodeImpl::STOREW(zOp);
				break;
			case STOREB:
				ZOpcodeImpl::STOREB(zOp);
				break;
			case PUT_PROP:
				ZOpcodeImpl::PUT_PROP(zOp);
				break;
			case READ:
				ZOpcodeImpl::READ(zOp);
				break;
			case PRINT_CHAR:
				ZOpcodeImpl::PRINT_CHAR(zOp);
				break;
			case PRINT_NUM:
				ZOpcodeImpl::PRINT_NUM(zOp);
				break;
			case RANDOM:
				ZOpcodeImpl::RANDOM(zOp);
				break;
			case PUSH:
				ZOpcodeImpl::PUSH(zOp);
				break;
			case PULL_VAR:
				ZOpcodeImpl::PULL(zOp);
				break;
			case SPLIT_WINDOW:
				ZOpcodeImpl::SPLIT_WINDOW(zOp);
				break;
			case SET_WINDOW:
				ZOpcodeImpl::SET_WINDOW(zOp);
				break;
			case CALL_VS2:
				ZOpcodeImpl::CALL_VS2(zOp);
				break;
			case ERASE_WINDOW:
				ZOpcodeImpl::ERASE_WINDOW(zOp);
				break;
			case ERASE_LINE:
				ZOpcodeImpl::ERASE_LINE(zOp);
				break;
			case SET_CURSOR_LC:
				ZOpcodeImpl::SET_CURSOR(zOp);
				break;
			case GET_CURSOR:
				ZOpcodeImpl::GET_CURSOR(zOp);
				break;
			case SET_TEXT_STYLE:
				ZOpcodeImpl::SET_TEXT_STYLE(zOp);
				break;
			case BUFFER_MODE:
				ZOpcodeImpl::BUFFER_MODE(zOp);
				break;
			case OUTPUT_STREAM_N:
				ZOpcodeImpl::OUTPUT_STREAM_N(zOp);
				break;
			case INPUT_STREAM:
				ZOpcodeImpl::INPUT_STREAM(zOp);
				break;
			case SOUND_EFFECT:
				ZOpcodeImpl::SOUND_EFFECT(zOp);
				break;
			case READ_CHAR:
				ZOpcodeImpl::READ_CHAR(zOp);
				break;
			case SCAN_TABLE:
				ZOpcodeImpl::SCAN_TABLE(zOp);
				break;
			case NOT_V:
				ZOpcodeImpl::NOT_V(zOp);
				break;
			case CALL_VN:
				ZOpcodeImpl::CALL_VN(zOp);
				break;
			case CALL_VN2:
				ZOpcodeImpl::CALL_VN2(zOp);
				break;
			case TOKENISE:
				ZOpcodeImpl::TOKENISE(zOp);
				break;
			case ENCODE_TEXT:
				ZOpcodeImpl::ENCODE_TEXT(zOp);
				break;
			default:
				throw IllegalZOpcode();
				break;
			}
		}
		break;
	default:
		throw IllegalZOpcode();	// wasn't 0, 1, 2, or VAR ops, must be a bad one!
	}
	if(jumpMade){
		if(jumpMade==ZOpcodeImpl::JUMP_OFFSET){
			branchPCounter(zOp.getOpcodeSize()+jumpValue);
		}else if(jumpMade==ZOpcodeImpl::JUMP_POSITION){
			setPCounter((ulong)jumpValue);
		}
		return 1;
	}
	return 0;
}
