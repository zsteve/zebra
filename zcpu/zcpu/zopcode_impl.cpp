#include "zopcode_impl.h"
#include "zopcode.h"
#include "zcpu.h"
#include "../../zstack/zstack/zstack.h"

namespace ZOpcodeImpl{
	/**
	 *flag for jump (branch)
	 * if true, a jump needs to be made
	 */
	int* jumpFlag=NULL;
	int* jumpOffset=NULL;	/// pointer to var to hold byte offset for jump 
	ZCpu* cpuObj=NULL;

	void registerJumpFlag(int* f){
		jumpFlag=f;
	}

	void registerJumpOffset(int* o){
		jumpOffset=o;
	}

	void registerZCpuObj(ZCpu* o){
		cpuObj=o;
	}

	// implementations for z-machine opcodes

	/// implementation of JE instruction
	int JE(ZOpcode& zOp){
		// operand 1 : a, operand 2 : b
		// jumps if a == b
		try{
			ZOperandType a, b;
			int a_val, b_val;
			a=zOp.getOperandTypes()[0];
			b=zOp.getOperandTypes()[1];
			if(a==ZOPERANDTYPE_LARGE_CONST || a==ZOPERANDTYPE_SMALL_CONST){
				a_val=zOp.getOperands()[0];
			}else if(a==ZOPERANDTYPE_VAR){
				if(zOp.getOperands()[0]<0x10 && zOp.getOperands()[0]>0){
					a_val=cpuObj->currentRoutine->readLocalVar(cpuObj->zStack, zOp.getOperands()[0]);
				}else{
					a_val=cpuObj->zMem.readGlobalVar(zOp.getOperands()[0]);
				}
			}
			if(b==ZOPERANDTYPE_LARGE_CONST || b==ZOPERANDTYPE_SMALL_CONST){
				b_val=zOp.getOperands()[1];
			}else if(b==ZOPERANDTYPE_VAR){
				if(zOp.getOperands()[1]<0x10 && zOp.getOperands()[1]>0){
					b_val=cpuObj->currentRoutine->readLocalVar(cpuObj->zStack, zOp.getOperands()[1]);
				}else{
					b_val=cpuObj->zMem.readGlobalVar(zOp.getOperands()[1]);
				} 
			}else if(b==ZOPERANDTYPE_OMITTED){
				b_val=a_val-1;			// if b is omitted, JE always fails, so we make sure b_val!=a_val
			}
			if(a_val==b_val){
				*jumpFlag=1;
				*jumpOffset=zOp.branchInfo.branchOffset;
				return 1;
			}
			*jumpFlag=0;
			return 0;
		}catch(...){
			throw IllegalZOpcode();
		}
		return 0;
	}

	/// implementation of JL instruction
	int JL(ZOpcode& zOp){
		// operand 1 : a, operand 2 : b
		// jumps if a < b
		try{
			ZOperandType a, b;
			int a_val, b_val;
			a=zOp.getOperandTypes()[0];
			b=zOp.getOperandTypes()[1];
			if(a==ZOPERANDTYPE_LARGE_CONST || a==ZOPERANDTYPE_SMALL_CONST){
				a_val=zOp.getOperands()[0];
			}else if(a==ZOPERANDTYPE_VAR){
				if(zOp.getOperands()[0]<0x10 && zOp.getOperands()[0]>0){
					a_val=cpuObj->currentRoutine->readLocalVar(cpuObj->zStack, zOp.getOperands()[0]);
				}else{
					a_val=cpuObj->zMem.readGlobalVar(zOp.getOperands()[0]);
				}
			}
			if(b==ZOPERANDTYPE_LARGE_CONST || b==ZOPERANDTYPE_SMALL_CONST){
				b_val=zOp.getOperands()[1];
			}else if(b==ZOPERANDTYPE_VAR){
				if(zOp.getOperands()[1]<0x10 && zOp.getOperands()[1]>0){
					b_val=cpuObj->currentRoutine->readLocalVar(cpuObj->zStack, zOp.getOperands()[1]);
				}else{
					b_val=cpuObj->zMem.readGlobalVar(zOp.getOperands()[1]);
				} 
			}
			if(a_val<b_val){
				*jumpFlag=1;
				*jumpOffset=zOp.branchInfo.branchOffset;
				return 1;
			}
			*jumpFlag=0;
			return 0;
		}catch(...){
			throw IllegalZOpcode();
		}
		return 0;
	}

	/// implementation of JG instruction
	int JG(ZOpcode& zOp){
		// operand 1 : a, operand 2 : b
		// jumps if a > b
		try{
			ZOperandType a, b;
			int a_val, b_val;
			a=zOp.getOperandTypes()[0];
			b=zOp.getOperandTypes()[1];
			if(a==ZOPERANDTYPE_LARGE_CONST || a==ZOPERANDTYPE_SMALL_CONST){
				a_val=zOp.getOperands()[0];
			}else if(a==ZOPERANDTYPE_VAR){
				if(zOp.getOperands()[0]<0x10 && zOp.getOperands()[0]>0){
					a_val=cpuObj->currentRoutine->readLocalVar(cpuObj->zStack, zOp.getOperands()[0]);
				}else{
					a_val=cpuObj->zMem.readGlobalVar(zOp.getOperands()[0]);
				}
			}
			if(b==ZOPERANDTYPE_LARGE_CONST || b==ZOPERANDTYPE_SMALL_CONST){
				b_val=zOp.getOperands()[1];
			}else if(b==ZOPERANDTYPE_VAR){
				if(zOp.getOperands()[1]<0x10 && zOp.getOperands()[1]>0){
					b_val=cpuObj->currentRoutine->readLocalVar(cpuObj->zStack, zOp.getOperands()[1]);
				}else{
					b_val=cpuObj->zMem.readGlobalVar(zOp.getOperands()[1]);
				} 
			}
			if(a_val>b_val){
				*jumpFlag=1;
				*jumpOffset=zOp.branchInfo.branchOffset;
				return 1;
			}
			*jumpFlag=0;
			return 0;
		}catch(...){
			throw IllegalZOpcode();
		}
		return 0;
	}

	/// implementation of DEC_CHK instruction
	int DEC_CHK(ZOpcode& zOp){
		/*Decrement variable, and branch if it is
		now less than the given value.
		*/
		try{
			ZOperandType val;
			int var_val, val_val;
			val=zOp.getOperandTypes()[1];

			// var's type is implied as a VARIABLE
			if(zOp.getOperands()[0]<0x10 && zOp.getOperands()[0]>0){
				var_val=cpuObj->currentRoutine->readLocalVar(cpuObj->zStack, zOp.getOperands()[0]);
			}else{
				var_val=cpuObj->zMem.readGlobalVar(zOp.getOperands()[0]);
			}

			if(val==ZOPERANDTYPE_LARGE_CONST || val==ZOPERANDTYPE_SMALL_CONST){
				val_val=zOp.getOperands()[1];
			}else if(val==ZOPERANDTYPE_VAR){
				if(zOp.getOperands()[1]<0x10 && zOp.getOperands()[1]>0){
					val_val=cpuObj->currentRoutine->readLocalVar(cpuObj->zStack, zOp.getOperands()[1]);
				}else{
					val_val=cpuObj->zMem.readGlobalVar(zOp.getOperands()[1]);
				} 
			}
			
			var_val--;
			if(var_val<val_val){
				// branch
				*jumpFlag=1;
				*jumpOffset=zOp.branchInfo.branchOffset;
			}else{
				*jumpFlag=0;
			}
			// now we need to store again
			if(zOp.getOperands()[0]<0x10 && zOp.getOperands()[0]>0){
				cpuObj->currentRoutine->storeLocalVar(cpuObj->zStack, zOp.getOperands()[0], var_val);
			}else{
				cpuObj->zMem.storeGlobalVar(zOp.getOperands()[0], var_val);
			}
		}catch(...){
			throw IllegalZOpcode();
		}
	}

	int CALL(ZOpcode& zOp){
		//call routine ...up to 3 args... -> (result)
		try{
			
		}catch(...){
			throw IllegalZOpcode();
		}
	}
};