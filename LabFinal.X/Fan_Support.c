#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"
#include "utils.h"

extern char duty_cycle;
extern char fan_set_temp;
extern signed char tempC;

int get_duty_cycle(int tempC, int fan_set_temp)
{	
    if(tempC < fan_set_temp) duty_cycle = 0;
    else if(tempC >= fan_set_temp)
    {
        int diff_temp = tempC - 16;
        duty_cycle = diff_temp *5;
        
        if (duty_cycle > 100) duty_cycle = 100;
    }
    return duty_cycle;
}

void Monitor_Fan()
{       
    if ((FAN_MODE == 1) && (FAN_EN == 1)){      
        
        if (tempC >= fan_set_temp) {
            printf ("=== FAN MODE ON ===\r\n");
        
            duty_cycle = get_duty_cycle(tempC, fan_set_temp);
            do_update_pwm(duty_cycle);
        }
        else {
            printf ("=== FAN MODE OFF ===\r\n");
            duty_cycle = 0;
            do_update_pwm(duty_cycle);
        }
    }
    else if (FAN_EN == 1){
        printf ("=== FAN ENABLED ===\r\n");  
        do_update_pwm(duty_cycle);
    }           
    else {
        printf ("=== FAN DISABLED ===\r\n");  
        do_update_pwm(duty_cycle);
    }
}

int get_RPM()
{
    int RPS = TMR3L / 2; // read the count. Since there are 2 pulses per rev then RPS = count /2
    TMR3L = 0; // clear out the count
    return (RPS * 60); // return RPM = 60 * RPS
}

void Toggle_Fan()
{
    FAN_EN = !FAN_EN;
    if(FAN_EN == 1) Turn_On_Fan();
    else Turn_Off_Fan();
}

void Turn_Off_Fan()
{
    printf("*Disabling Fan\r\n\n"); 
    FAN_EN = 0;    
}

void Turn_On_Fan()
{
    printf("*Enabling Fan\r\n\n");
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
}

void Increase_Duty_Cycle()
{
    if(duty_cycle == 100) {
        Do_Beep();
        Do_Beep();
        do_update_pwm(100); // Update PWM, Do_Beep(); breaks PWM
        printf("*Maximum speed reached\r\n\n");
    }
    else {
        duty_cycle += 5;
        do_update_pwm(duty_cycle);
        printf("*Duty Cycle increased by 5\r\n\n");
    }
}

void Decrease_Duty_Cycle()
{
    if(duty_cycle == 0){
        Do_Beep();
        Do_Beep();
        do_update_pwm(0); // Update PWM, Do_Beep(); breaks PWM
        printf("*Minimum speed reached\r\n\n");
    }
    else {
        duty_cycle -= 5;
        do_update_pwm(duty_cycle);
        printf("*Duty Cycle decreased by 5\r\n\n");
    }
}