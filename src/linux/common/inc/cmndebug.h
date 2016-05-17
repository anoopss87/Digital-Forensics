/********************************************************************
 *
 * File         : cmndebug.h
 * Author       : Naveen 
 * Date created : October 01, 2014
 * Description  : This file contains the debug macros used         
 *                by the all the modules 
 *
 *******************************************************************/
#ifdef TRACE_WANTED
#define DEBUG_LOG(Fmt) printf(Fmt)

#define DEBUG_LOG1(Fmt, Arg) printf(Fmt, Arg)

#define DEBUG_LOG2(Fmt, Arg1, Arg2) printf(Fmt, Arg1, Arg2)

#define DEBUG_LOG3(Fmt, Arg1, Arg2, Arg3) printf(Fmt, Arg1, Arg2, Arg3)

#define DEBUG_LOG4(Fmt, Arg1, Arg2, Arg3, Arg4) \
                                      printf(Fmt, Arg1, Arg2, Arg3, Arg4)
#define DEBUG_LOG5(Fmt, Arg1, Arg2, Arg3, Arg4, Arg5) \
                                      printf(Fmt, Arg1, Arg2, Arg3, Arg4, Arg5)
#define DEBUG_LOG6(Fmt, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) \
                                      printf(Fmt, Arg1, Arg2, Arg3, Arg4, Arg5, \
                                              Arg6)

#else
#define DEBUG_LOG(Fmt)           
#define DEBUG_LOG1(Fmt, Arg)     
#define DEBUG_LOG2(Fmt, Arg1, Arg2)                      
#define DEBUG_LOG3(Fmt, Arg1, Arg2, Arg3)                
#define DEBUG_LOG4(Fmt, Arg1, Arg2, Arg3, Arg4)          
#define DEBUG_LOG5(Fmt, Arg1, Arg2, Arg3, Arg4, Arg5)
#define DEBUG_LOG6(Fmt, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)
#endif
