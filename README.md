cpp-traceback
=============

Simple multiplatform traceback/stack trace printer for C++ - works on POSIX and
MS Windows.

- POSIX version uses backtrace() and family.
- MS Windows version uses StackWalker (http://stackwalker.codeplex.com/).

For compiler options and libraries see example SConstruct.
