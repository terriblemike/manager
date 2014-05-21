#ifndef PSTATUS_H
#define PSTATUS_H

class BITS{
public:
    int b00 : 1;
    int b01 : 1;
    int b02 : 1;
    int b03 : 1;
    int b04 : 1;
    int b05 : 1;
    int b06 : 1;
    int b07 : 1;
    int b08 : 1;
    int b09 : 1;
    int b10 : 1;
    int b11 : 1;
    int b12 : 1;
    int b13 : 1;
    int b14 : 1;
    int b15 : 1;
};

class BYTES {
public:
    BYTE byte1;
    BYTE byte2;
};

typedef union {
    BITS bits;
    int value;
    BYTES bytes;
} PStatus;

#endif // PSTATUS_H
