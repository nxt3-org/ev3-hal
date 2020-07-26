#ifndef AUTOOFF
#define AUTOOFF

#include <stdbool.h>

bool autooff_open(void);
void autooff_close(void);
bool autooff_should_exit(void);

#endif //AUTOOFF
