#define pwm_channels 3
unsigned char count0;
unsigned char max_types = pwm_channels;
unsigned char match[pwm_channels] = {100, 50, 20}; //pwm einstellungen 0 - 255 wobei 255 volle helligkeit
unsigned char match_tmp[pwm_channels + 1] = {255, 255, 255, 255};
unsigned char match_count = 0;

unsigned char match_do[pwm_channels];

void pwm_init(); //pwm wird initialisiert

void sort_matches(); //hier werden die einträge sortiert und in max_types geschrieben wie viele verschiedene es gibt

