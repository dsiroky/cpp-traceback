/// @license MIT license http://opensource.org/licenses/MIT
/// @author David Siroky <siroky@dasir.cz>
/// @file

#include <stdio.h>

//##########################################################################
//##########################################################################

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)

#include "StackWalker.h"

class StackWalkerToConsole : public StackWalker
{
protected:
  // do not print modules initialization
  void OnLoadModule(LPCSTR, LPCSTR, DWORD64, DWORD, DWORD, LPCSTR, LPCSTR, ULONGLONG) {}
  // do not print symbols initialization
  void OnSymInit(LPCSTR, DWORD, LPCSTR) {}
  virtual void OnOutput(LPCSTR szText) { printf("%s", szText); }
};

void print_traceback()
{
  printf("traceback:\n");
  StackWalkerToConsole sw; sw.ShowCallstack();
}

//##########################################################################
//##########################################################################

#else // other OSes

#include <execinfo.h>
#include <stdlib.h>
#include <cxxabi.h>

void print_traceback()
{
  printf("traceback:\n");

  // storage array for stack trace address data
  void* addrlist[100];

  // retrieve current stack addresses
  int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

  if (addrlen == 0) {
     printf("  <empty, possibly corrupt>\n");
     return;
  }

  // resolve addresses into strings containing "filename(function+address)",
  // this array must be free()-ed
  char** symbollist = backtrace_symbols(addrlist, addrlen);

  // allocate string which will be filled with the demangled function name
  size_t funcnamesize = 256;
  char* funcname = (char*)malloc(funcnamesize);

  // iterate over the returned symbol lines. skip the first, it is the
  // address of this function.
  for (int i = 1; i < addrlen; i++)
  {
     char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

     // find parentheses and +address offset surrounding the mangled name:
     // ./module(function+0x15c) [0x8048a6d]
     for (char *p = symbollist[i]; *p; ++p)
     {
         if (*p == '(')
             begin_name = p;
         else if (*p == '+')
             begin_offset = p;
         else if (*p == ')' && begin_offset) {
             end_offset = p;
             break;
         }
     }

     if (begin_name && begin_offset && end_offset
         && begin_name < begin_offset)
     {
         *begin_name++ = '\0';
         *begin_offset++ = '\0';
         *end_offset = '\0';

         // mangled name is now in [begin_name, begin_offset) and caller
         // offset in [begin_offset, end_offset). now apply
         // __cxa_demangle():

         int status;
         char* ret = abi::__cxa_demangle(begin_name,
                                         funcname, &funcnamesize, &status);
         if (status == 0) {
             funcname = ret; // use possibly realloc()-ed string
             printf("  %s : %s+%s\n",
                     symbollist[i], funcname, begin_offset);
         }
         else {
             // demangling failed. Output function name as a C function with
             // no arguments.
             printf("  %s : %s()+%s\n",
                     symbollist[i], begin_name, begin_offset);
         }
     }
     else
     {
         // couldn't parse the line? print the whole line.
         printf("  %s\n", symbollist[i]);
     }
  }

  free(funcname);
  free(symbollist);
}

#endif
