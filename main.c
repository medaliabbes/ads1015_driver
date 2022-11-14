


#include "stdio.h"
#include "ads1015_.h"
#include "stdbool.h"
#include <wiringPi.h>

int main()
{
    wiringPiSetup();
    ads_init() ;
    int fd = ads_begin() ;

    ads_SetADCChannel(fd , 1) ;

    while(ads_IsDataReady() != true)
    {

    }

    int ret = ads_GetADCChannel(fd , 1) ;
    printf("ret :%d\r\n" , ret);
    return 0 ;
}
