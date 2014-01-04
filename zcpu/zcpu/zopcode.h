#ifndef ZOPCODE_H
#define ZOPCODE_H

#include "../../zglobal/zglobal.h"
#include "../../zglobal/zglobaldefines.h"
#include "../../zmemory/zmemory/zmemory.h"
#include "../../zstack/zstack/zstack.h"

#include <vector>

using namespace std;

class IllegalZOpcode : ZException{
    public:
    IllegalZOpcode(){
        zErrorLogger.addError("Error : IllegalZOpcode thrown");
    }
    IllegalZOpcode(const int line, const char* function, const char* file){
        zErrorLogger.addError(("Error : IllegalZOpcode thrown at : "+compileErrorMsg(line, function, file)).c_str());
    }
};

/**
 * namespace for z-cpu enums
 */
namespace ZCpuOps
{
    enum ZOpcodeType{
        ZOPTYPE_LONG, ZOPTYPE_SHORT, ZOPTYPE_EXT, ZOPTYPE_VAR
    };

    enum ZOperandCount{
        ZOPCOUNT_0, ZOPCOUNT_1, ZOPCOUNT_2, ZOPCOUNT_VAR
    };

    enum ZOperandType{
        ZOPERANDTYPE_LARGE_CONST, ZOPERANDTYPE_SMALL_CONST, ZOPERANDTYPE_VAR, ZOPERANDTYPE_OMITTED
    };

    enum ZOpcodeName_2{
        // 2 OPS
        JE=1,
        JL,
        JG,
        DEC_CHK,
        INC_CHK,
        JIN,
        TEST,
        OR,
        AND,
        TEST_ATTR,
        SET_ATTR,
        CLEAR_ATTR,
        STORE,
        INSERT_OBJ,
        LOADW,
        LOADB,
        GET_PROP,
        GET_PROP_ADDR,
        GET_NEXT_PROP,
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        CALL_2S,
        CALL_2N,
        SET_COLOUR_FB,
        SET_COLOUR_FBW,
        THROW
	};

	 enum ZOpcodeName_1{
        // 1 OP

        JZ,
        GET_SIBLING,
        GET_CHILD,
        GET_PARENT,
        GET_PROP_LEN,
        INC,
        DEC,
        PRINT_ADDR,
        CALL_1S,
        REMOVE_OBJ,
        PRINT_OBJ,
        RET,
        JUMP,
        PRINT_PADDR,
        LOAD,
        NOT,
        CALL_1N
	 };

	  enum ZOpcodeName_0{

        // O OP
		/** edit made, commenting out some of the
			enum entries as they have the same hex value
			see Z-Machine standards document 14/0OP **/
        RTRUE,
        RFALSE,
        PRINT,
        PRINT_RET,
        NOP,
        SAVE_LABEL,
        //SAVE_RESULT,
        RESTORE_LABEL,
        //RESTORE_RESULT,
        RESTART,
        RET_POPPED,
        POP,
        //CATCH,
        QUIT,
        NEW_LINE,
        SHOW_STATUS,
        VERIFY,
        EXTENDED,
        PIRACY
	  };

	   enum ZOpcodeName_VAR{

        // VAR OP

        CALL,
       // CALL_VS,
        STOREW,
        STOREB,
        PUT_PROP,
        READ,
       // SREAD_PTR,
       // AREAD,
        PRINT_CHAR,
        PRINT_NUM,
        RANDOM,
        PUSH,
       // PULL_VAL,
        PULL_VAR,
        SPLIT_WINDOW,
        SET_WINDOW,
        CALL_VS2,
        ERASE_WINDOW,
        ERASE_LINE,
      //  ERASE_LINE_PIX,
        SET_CURSOR_LC,
       // SET_CURSOR_LCW,
        GET_CURSOR,
        SET_TEXT_STYLE,
        BUFFER_MODE,
        OUTPUT_STREAM_N,
       // OUTPUT_STREAM_NT,
       // OUTPUT_STREAM_NTW,
        INPUT_STREAM,
        SOUND_EFFECT,
        READ_CHAR,
        SCAN_TABLE,
        NOT_V,
        CALL_VN,
        CALL_VN2,
        TOKENISE,
        ENCODE_TEXT,
        COPY_TABLE,
        PRINT_TABLE,
        CHECK_ARG_COUNT
	};

	enum ZOpcodeName_EXT{
        // EXT OP
        SAVE,
        RESTORE,
        LOG_SHIFT,
        ART_SHIFT,
        SET_FONT,
        DRAW_PICTURE,
        PICTURE_DATA,
        ERASE_PICTURE,
        SET_MARGINS,
        SAVE_UNDO,
        RESTORE_UNDO,
        PRINT_UNICODE,
        CHECK_UNICODE,
		// skip opcode numbers 13, 14, 15
        MOVE_WINDOW=16,
        WINDOW_SIZE,
        WINDOW_STYLE,
        GET_WIND_PROP,
        SCROLL_WINDOW,
        POP_STACK,
        READ_MOUSE,
        MOUSE_WINDOW,
        PUSH_STACK,
        PUT_WIND_PROP,
        PRINT_FORM,
        MAKE_MENU,
        PICTURE_TABLE
	};

};

using namespace ZCpuOps;

class ZOpcode{
private:
    ulong addr;
    ZOpcodeType opcodeType;
    ZOperandCount operandCount;
    vector<ZOperandType> operandTypes;
	vector<ulong> operands;
    int opcodeName;
	zbyte opcodeNum;
	zbyte opcodeByte;
	int opcodeSize;			/// opcode size in bytes
	zword* opcodeString;	/// trailing string (if any)

	int getZOpcodeName(zbyte zOp) throw (ZException);
	ulong getOperand(ulong addr, ZOperandType type, ZMemory& zMem, int& counter);
	ZOperandType getOperandType(int types);
	bool opcodeHasBranch();
	bool opcodeHasStore();
	bool opcodeHasTrailingString();
	bool longOpcodeHasLargeOperands();

public:
    ZOpcode();
    ZOpcode(ulong addr, ZMemory& zMem);
	~ZOpcode();
    void decodeOp(ulong addr, ZMemory& zMem);
	// accessor functions
	int getOpcodeSize(){return opcodeSize;}
	ZOpcodeType getOpcodeType(){return opcodeType;}
	ZOperandCount getOperandCount(){return operandCount;}
	int countOperands(){
		for(int i=0; i<operands.size(); i++){
			if(operandTypes[i]==ZOPERANDTYPE_OMITTED){
				return i+1;
			}
		}
		return operands.size();
	}
	vector<ZOperandType>& getOperandTypes(){return operandTypes;}
	vector<ulong>& getOperands(){return operands;}
	int getOpcodeName(){return opcodeName;}
	zbyte getOpcodeNum(){return opcodeNum;}
	zbyte getOpcodeByte(){return opcodeByte;}
	zword* getOpcodeString(){return opcodeString;}

	struct branchInfo{
		branchInfo(){
			// constructor
			hasBranch=false;
			branchSize=0;
			branchCond=false;
			branchOffset=0;
		}
		// branch physical info
		bool hasBranch;		/// opcode has branch / has no branch
		int branchSize;		/// branch size can be 1 or 2 bytes
		zbyte byte1, byte2;	/// bytes 1 and 2

		// branch logical info
		bool branchCond;	/// condition for branch to be taken
		int branchOffset;	/// offset for branch to take
	}branchInfo;

	struct storeInfo{
		storeInfo(){
			// constructor
			storeVar=0;
			hasStore=false;
		}
		bool hasStore;
		zbyte storeVar;		/// variable number to store into
	}storeInfo;

	bool extraTypeByte;		/// opcode has extra type byte (either call_vn2 or call_vs2)
protected:
};

#endif
