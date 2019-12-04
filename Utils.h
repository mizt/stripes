#import "stdarg.h"
#import "math.h"

namespace Utils {    
    const char *stringWithFormat(const char *format, ...){
        char *str = new char[(long)fmax(1024,(pow(2,(floor(log(strlen(format))/log(2)))+1)))];
        va_list arg;
        va_start(arg,format); vsprintf(str,format,arg); va_end(arg);	
        return str;
    }
}