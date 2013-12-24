#ifndef ZCPU_H
#define ZCPU_H

#include "../../zmemory/zmemory/zmemory.h"
#include "../../zmemory/zmemory/zobject.h"
#include "../../zstack/zstack/zstack.h"
#include "../../zglobal/zglobal.h"
#include "../../ztext/ztext/ztext.h"
#include "zopcode.h"

extern int zVersion;

namespace ZCpuInternal{

	/// CPU routine information class
	class ZCpuRoutine{
	private:
		bool varNumisInRange(int v){
			// checks if var num is in range
			if(v>0 && v<=localCount && v<15){
				return true;
			}
			return false;
		}
	public:
		ZCpuRoutine() : localInitialVals(0){
			beginAddr=codeStartAddr=0;
			setLocalCount(0);
			parentRoutine=NULL;
		}

		void loadAddr(ulong addr, ZMemory& zMem) throw (ZException){
			try{
				// A routine begins with one byte indicating the number of local variables it has
				setLocalCount(zMem.readZByte(addr));
				if(zVersion<=4){
					// versions 1-4, initialization list of zwords follow
					for(int i=0; i<localCount; i++){
						localInitialVals.push_back(zMem.readZWord(addr+1+(2*i)));
					}
				} // otherwise, for versions 5 and up, all vars are initialized as 0
				// executable code begins at the byte after this information
				// that is : addr+1+(2*(localCount-1))+1 --> first byte of executable code
				codeStartAddr=addr+1+(2*(localCount-1))+1;
				return;
			}catch(...){
				throw ZException();
			}
		}

		/**
		 * creates local variables 
		 * @param zStack reference of stack object
		 * @throw ZException on any exception
		 */
		void createLocalVars(ZStack& zStack) throw (ZException){
			try{
				for(int i=0; i<localCount; i++){
					zStack.push(localInitialVals[i]);
				}
				return;
			}catch(...){
				throw ZException();
			}
		}

		/** 
			* destroys local variables
			* @param zStack reference of stack object
			* @throw ZException on any exception
			*/
		void destoryLocalVars(ZStack& zStack) throw (ZException){
			try{
				zStack.setStackPtr(zStack.getStackPtr()-localCount);
			}catch(...){
				throw ZException();
			}
		}

		/**
		 * reads local variable
		 * @param ZStack reference of stack object
		 * @throw ZException on any exception
		 */
		zword readLocalVar(ZStack& zStack, int varNum) throw (ZException){
			try{
				if(varNumisInRange(varNum)){
					return (zword)*(zStack.getStackData()+(zStack.getStackPtr()-localCount+(varNum-1)));
				}else{
					throw ZException();
				}
			}catch(...){
				throw ZException();
			}
		}

		/**
		 * stores local variable
		 * @param ZStack reference of stack object
		 * @throw ZException on any exception
		 */
		void storeLocalVar(ZStack& zStack, int varNum, zword value) throw (ZException){
			try{
				if(varNumisInRange(varNum)){
					*(zStack.getStackData()+(zStack.getStackPtr()-localCount+(varNum-1)))=value;
				}else{
					throw ZException();
				}
			}catch(...){
				throw ZException();
			}
		}

		ulong localCount;	/// number of local variables in current routine [0..15]
		void setLocalCount(int count) throw (ZException){
			if( count<0 || count>15 ) throw ZException();
			localCount=count;
		}
		vector<zword> localInitialVals; /// versions 4 and 5 : all initial values are 0

		ulong beginAddr;		/// address of the header of the routine
		ulong codeStartAddr;	/// address of where the actual assembly code starts

		ZCpuRoutine* parentRoutine;	/** pointer to ZCpuRoutine object of parent routine
									 * if NULL, then this routine has no parent. (must be main()!)
									 */
	};

	class ZCpuStackFrame{
	public:
		struct entry{
			ulong pCounter;
			
		}entry;
	private:
	protected:
	};

}

using namespace ZCpuInternal;

class ZCpu
{
public:
	ZCpu() : zMem(zMem), zStack(zStack){}
	ZCpu(ZMemory& zMem, ZStack& zStack);
	int startExecution();
	ZMemory& zMem;
	ZStack& zStack;
private:
	ulong pCounter;				/// instruction pointer
	bool haltFlag;
	void incrementPCounter(ZOpcode& zOp);
	void changePCounter(int offset);
	int start();				/// start execution
	int mainLoop(ZOpcode& zOp);				/// main loop for executing instructions
public:
	ZCpuRoutine *currentRoutine;	/// ZCpuRoutine for current routine
	ZCpuRoutine *mainRoutine;		/// ZCpuRoutine for main routine
protected:
};

#endif
