#include "alsa/asoundlib.h"

void volume_init(void);
void volume_uninit(void);
int volume_read(void);
void volume_write(int);