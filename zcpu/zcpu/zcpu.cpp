#include "zcpu.h"
#include "zopcode_impl.h"
#include "../../zstack/zstack/zstack.h"

ZCpu::ZCpu(ZMemory& zMem, ZStack& zStack) : zMem(zMem), zStack(zStack){
	// initial value of program counter is found at 0x6
	// (packed address)
	pCounter=zMem.readZWord(0x6);
	// now we need to initialize the mainRoutine and currentRoutine objects
	zStack.initStack(4096);	// 4kb stack size
	mainRoutine.loadAddr(pCounter-1, zMem);
	mainRoutine.createLocalVars(zStack);
	currentRoutine=mainRoutine;
	ZOpcode(pCounter, zMem);
	haltFlag=false;
}

void ZCpu::incrementPCounter(ZOpcode& zOp){
	pCounter+=zOp.getOpcodeSize();
}

void ZCpu::changePCounter(int offset){
	// Address after branch data + Offset - 2.
	pCounter+=(offset-2);
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
	int jumpOffset=0;
	ZOpcodeImpl::registerJumpFlag(&jumpMade);
	ZOpcodeImpl::registerJumpOffset(&jumpOffset);
	ZOpcodeImpl::registerZCpuObj(this);
	switch(zOp.getOperandCount()){
	case ZOPCOUNT_0:
		{
			switch(zOp.getOpcodeName()){
			default:
				break;
			}
		}
		break;
	case ZOPCOUNT_1:
		{
			switch(zOp.getOpcodeName()){
			default:
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
			default:
				break;
			}
		}
		break;
	case ZOPCOUNT_VAR:
		{
			switch(zOp.getOpcodeName()){
			default:
				break;
			}
		}
		break;
	default:
		throw IllegalZOpcode();	// wasn't 0, 1, 2, or VAR ops, must be a bad one!
	}
	if(jumpMade){
		changePCounter(zOp.getOpcodeSize()+jumpOffset);
		return 1;
	}
	return 0;
}
