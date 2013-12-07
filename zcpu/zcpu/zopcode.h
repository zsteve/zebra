#ifndef ZOPCODE_H
#define ZOPCODE_H

#include "zcpu.h"
#include "../../zglobal/zglobal.h"
#include "../../zglobal/zglobaldefines.h"
#include "../../zmemory/zmemory/zmemory.h"

#include <vector>

using namespace std;

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
        ZOPERANDTYPE_LARGE_CONST, ZOPERANDTYPE_SMALL_CONST, ZOPERANDTYPE_VAR, ZOPERAND_OMITTED
    };

    enum ZOpcodeName{
        // 2 OPS
        JE,
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
        THROW,

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
        CALL_1N,

        // O OP

        RTRUE,
        RFALSE,
        PRINT,
        PRINT_RET,
        NOP,
        SAVE_LABEL,
        SAVE_RESULT,
        RESTORE_LABEL,
        RESTORE_RESULT,
        RESTART,
        RET_POPPED,
        POP,
        CATCH,
        QUIT,
        NEW_LINE,
        SHOW_STATUS,
        VERIFY,
        EXTENDED,
        PIRACY,

        // VAR OP

        CALL,
        CALL_VS,
        STOREW,
        STOREB,
        PUT_PROP,
        SREAD_P,
        SREAD_PTR,
        SREAD_PTRR,
        AREAD,
        PRINT_CHAR,
        PRINT_NUM,
        RANDOM,
        PUSH,
        PULL_VAL,
        PULL_VAR,
        SPLIT_WINDOW,
        SET_WINDOW,
        CALL_VS2,
        ERASE_WINDOW,
        ERASE_LINE,
        ERASE_LINE_PIX,
        SET_CURSOR_LC,
        SET_CURSOR_LCW,
        GET_CURSOR,
        SET_TEXT_STYLE,
        BUFFER_MODE,
        OUTPUT_STREAM_N,
        OUTPUT_STREAM_NT,
        OUTPUT_STREAM_NTW,
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
        CHECK_ARG_COUNT,

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
        MOVE_WINDOW,
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
        PICTURE_TABLE,

        // SPECIAL

        UNDEFINED   // if an opcode is UNDEFINED, we should halt with an error
    };

    unsigned char ZOpcodeNums[]={
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
        0x1f,

        0x80,
        0x81,
        0x82,
        0x83,
        0x84,
        0x85,
        0x86,
        0x87,
        0x88,
        0x89,
        0x8a,
        0x8b,
        0x8c,
        0x8d,
        0x8e,
        0x8f,

        0xb0,
        0xb1,
        0xb2,
        0xb3,
        0xb4,
        0xb5,
        0xb6,
        0xb7,
        0xb8,
        0xb9,
        0xba,
        0xbb,
        0xbc,
        0xbd,
        0xbe,
        0xbf,

        0xe0,
        0xe1,
        0xe2,
        0xe3,
        0xe4,
        0xe5,
        0xe6,
        0xe7,
        0xe8,
        0xe9,
        0xea,
        0xeb,
        0xec,
        0xed,
        0xee,
        0xef,
        0xf0,
        0xf1,
        0xf2,
        0xf3,
        0xf4,
        0xf5,
        0xf6,
        0xf7,
        0xf8,
        0xf9,
        0xfa,
        0xfb,
        0xfc,
        0xfd,
        0xfe,
        0xff
    };

};

class ZOpcode{
    private:
    ulong addr;
    ZOpcodeType opcodeType;
    ZOperandCount operandCount;
    vector<ZOperandType> operandTypes;
    ZOpcodeName opcodeName;

    decodeOp(ulong addr, ZMemory& zMem);
    public:
    ZOpcode();
    ZOpcode(ulong addr, ZMemory& zMem);
    protected:
};

#endif
