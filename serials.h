#ifndef _IS_INCLUDED_SERIALS_H
#define _IS_INCLUDED_SERIALS_H

#define SER_EXEC(a)	( ~((a << 1) | (a >> 31)) )
//#pragma diag_suppress=Pe061 
const unsigned int refserials[][5]={
  {SER_EXEC(0x00000000), SER_EXEC(0x0D0D0910), SER_EXEC(0x535719BF), SER_EXEC(0x4F58D32F), SER_EXEC(0xF5000003)},

};

#endif
