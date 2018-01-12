#include "Watch.h"
#ifdef ANDROID
#include <sys/time.h>
struct timeval tpstart,tpend;
void Watcher::start()
{
   gettimeofday(&tpstart,NULL);
}

double  Watcher::stop()
{
   gettimeofday(&tpend,NULL);
   double timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;     //注意，秒的读数和微秒的读数都应计算在内
   
   return 	timeuse;
}
#endif