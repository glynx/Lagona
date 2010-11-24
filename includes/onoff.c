#include <includes/onoff.h>

void onoff_init(void)
{
  onoff_dev0_DDR |= (1<<onoff_dev0_PIN);
  onoff_dev0_PORT &= ~(0<<onoff_dev0_PIN);
}

void onoff_dev(char dev_num, char setto)
{
  switch(dev_num)
  {
    case 0:
      if(setto)
      {
	onoff_dev0_PORT |= (1<<onoff_dev0_PIN);
      }
      else
      {
	onoff_dev0_PORT &= ~(1<<onoff_dev0_PIN);
      }
      break;
#if onoff_n_devices > 1
    case 1:
      if(setto)    
	onoff_dev1_PORT |= (1<<onoff_dev1_PIN);
      else
	onoff_dev1_PORT &= ~(1<<onoff_dev1_PIN);
      break;
#if onoff_n_devices > 2
    case 2:
      if(setto)    
	onoff_dev2_PORT |= (1<<onoff_dev2_PIN);
      else
	onoff_dev2_PORT &= ~(1<<onoff_dev2_PIN);
      break;
#endif
//......
#endif

#if uart == 1
    default:
      uart_send_pgm_string(wrong_dev);
#endif
  }
}
