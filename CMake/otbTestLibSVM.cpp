#include "otbConfigure.h"
int main(int argc, char *argv[])
{
  #ifdef OTB_USE_LIBSVM
  return 0;
  #else
  return -1;
  #endif
}
