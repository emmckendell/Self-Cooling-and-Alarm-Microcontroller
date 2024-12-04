int get_duty_cycle(int , int);
int get_RPM();
void Toggle_Fan();
void Turn_Off_Fan();
void Turn_On_Fan();
void Increase_Duty_Cycle();
void Decrease_Duty_Cycle();
void Set_DC_RGB(int);
void Set_RPM_RGB(int);
void Set_TempC_RGB(signed char);

#define FAN_EN			PORTAbits.RA5
#define FAN_MODE		PORTAbits.RA3