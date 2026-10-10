#include <stdio.h>
#include "sqlite3.h"
int main(void){ const char*s; for(int i=0;(s=sqlite3_compileoption_get(i));i++) puts(s); return 0; }
