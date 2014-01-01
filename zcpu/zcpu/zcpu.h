#ifndef ZCPU_H
#define ZCPU_H

#include "../../zmemory/zmemory/zmemory.h"
#include "../../zmemory/zmemory/zobject.h"
#include "../../zstack/zstack/zstack.h"
#include "../../zdictionary/zdictionary/zdictionary.h"
#include "../../zglobal/zglobal.h"
#include "../../ztext/ztext/ztext.h"
#include "../../zinout/zinout/zinout.h"
#include "zopcode.h"

extern int zVersion;

namespace ZCpuInternal{

	/// CPU routine information class
	class ZCpuRoutine{
	private:
		bool varNumisInRange(int v){
			// checks if var num is in range\
			// v==0 == top of stack
			if(v>=0 && v<=localCount && v<15){
				return true;
			}
			return false;
		}
	public:
		ZCpuRoutine() : localInitialVals(0){
			beginAddr=codeStartAddr=0;
			setLocalCount(0);
			parentRoutine=NULL;
			resumeAddr=NULL;
			keepRetValue=true;
		}

		void loadAddr(ulong addr, ZMemory& zMem) throw (ZException){
			try{
				// set begin addr
				beginAddr=addr;
				// A routine begins with one byte indicating the number of local variables it has
				setLocalCount(zMem.readZByte(addr));
				if(zVersion<=4){
					// versions 1-4, initialization list of zwords follow
					for(int i=0; i<localCount; i++){
						localInitialVals.push_back(zMem.readZWord(addr+1+(2*i)));
					}
				} // otherwise, for versions 5 and up, all vars are initialized as 0
				// executable code begins at the byte after this information
				// that is : addr+1+(2*(localCount)) --> first byte of executable code
				codeStartAddr=addr+1+(2*(localCount));
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
				// first, set the address of where the local vars may be found
				localStartAddr=zStack.getStackPtr()-1;	// -1 because the stack ptr is decremented before pushing a value
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
		 * note : possibly a redundant function
		 */
		void destoryLocalVars(ZStack& zStack) throw (ZException){
			try{
				return;	// it is indeed redundant!
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
					// buggy : does NOT take care of variables manually added to stack by
					// read (0) or write(0)
					// return endianize(*(zStack.getStackData()+(zStack.getStackPtr()-localCount+(varNum-1))));
					return endianize(*(zStack.getStackData()+(localStartAddr-(varNum-1))));
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
					// *(zStack.getStackData()+(zStack.getStackPtr()-localCount+(varNum-1)))=endianize(value); 
					// above statement buggy
					*((zStack.getStackData())+(localStartAddr-(varNum-1)))=endianize(value);
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
		ulong localStartAddr;	/// address _ON_THE_STACK_ of where the _STACK_ vars are

		ZCpuRoutine* parentRoutine;	/** pointer to ZCpuRoutine object of parent routine
									 * if NULL, then this routine has no parent. (must be main()!)
									 */

		bool keepRetValue;		/// flag for keep/don't keep return value
		zbyte retValueDest;		/// variable in which to put the return value

		ulong resumeAddr;		/** address at which to resume execution upon a RETURN
								 * should be set by the CALL instruction 
								 */
	};

	class ZCpuStackFrame{
	public:
		ZCpuStackFrame() : stackFrame(0){
			
		}

		int getSize(){return stackFrame.size();}

		ulong readEntry(int n){
			if(n<stackFrame.size()) return stackFrame[n];
			return 0;
		}

		void storeEntry(int n, ulong val){
			if(n<stackFrame.size()){
				stackFrame[n]=val;
				return;
			}
			return;
		}

		void pushEntry(ulong stackPtr){
			stackFrame.push_back(stackPtr);
		}

		ulong pullEntry(){
			if(stackFrame.size()!=0){
				ulong pulledEntry=stackFrame[stackFrame.size()-1];
				stackFrame.pop_back();
				return pulledEntry;
			}
			return 0;
		}

	private:
		vector<ulong> stackFrame;
	protected:
	};

}

using namespace ZCpuInternal;

class ZCpu
{
public:
	ZCpu() : zMem(zMem), zStack(zStack), zObject(zObject), zInOut(zInOut), zDict(zDict){}
	ZCpu(ZMemory& zMem, ZStack& zStack, ZObjectTable& zObject, ZInOut& zInOut, ZDictionary& zDict);
	int startExecution();
	ulong getPCounter(){return pCounter;}
	void incrementPCounter(ZOpcode& zOp);
	void branchPCounter(int offset);
	void setPCounter(ulong pCounter);

	// peripherals
	ZMemory& zMem;
	ZStack& zStack;
	ZObjectTable& zObject;
	ZInOut& zInOut;
	ZDictionary& zDict;
	//

	bool haltFlag;
private:
	ulong pCounter;				/// instruction pointer
	int start();				/// start execution
	int mainLoop(ZOpcode& zOp);				/// main loop for executing instructions
public:

	ZCpuRoutine *currentRoutine;	/// ZCpuRoutine for current routine
	ZCpuRoutine *mainRoutine;		/// ZCpuRoutine for main routine
	ZCpuStackFrame stackFrame;		/// stack frame for cpu

protected:
};

#endif
