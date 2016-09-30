#pragma once 

#include <fstream>
#include <stdarg.h>

class CLogSystem
{
   public:
      CLogSystem() {}

      bool ClearLog(const char *filename);
      bool WriteLog(const char *filename, const char *message, ...);
      bool AppendLog(const char *filename, const char *message, ...);
      
   protected:
      bool Write(std::ofstream &file, const char *message);
      bool Write(std::ofstream &file, const char *message, va_list &args);

   protected:
      std::ofstream errorLog;
};