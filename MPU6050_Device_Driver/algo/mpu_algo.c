#include <hcsr04.h> // ultrasonic
#include <lcd.h> // 8 bit
#include <buzzer.h> 
#include <led.h> 
#include <speaker.h> 
#include <motor.h> 
#include <ir.h>
#include <mpu.h>

extern char ir // var ir must be created in main_algo

//-------------------------------(Ima_Algorithm)-----------------------------

int Ima_Algorithm(int dir_sec_car, int bridge_case_sec_car ) // var bridge_case_sec_car must be created in main_algo
{
    int dir_my_car,heading;
    int bridge_case;
    dir_my_car = get_my_direction();       // get_my_direction() the mpu6050 >> suld be created in mpu_driver
    bridge_case_my_car = get_bridge_case(); // get_bridge_case() from the mpu6050 >> suld be created in mpu_driver
    heading = dir_my_car - dir_sec_car;
    bridge_case = ((bridge_case_sec_car) == (bridge_case_my_car))

    switch (heading)                       // check intersection angle
    {
        case 20  ... 160 :
        case 200 ... 340 :
            switch(ir)                     // ir var should be externed in my file
			{
			    case 0:
				switch(bridge_case)
				{
					case 1:
					// change id to ima_id
					break;
					default : break;
				}
				break;
				default : break;
			}
            
        break;
		default : break;
    }

}

//-------------------------------(Ima_Algorithm_ID)-----------------------------

void Ima_Algorithm_ID(void)
{
    int us_get_distance;
    //lcd warning on my lcd
    LCD_vDisplay("detected");
	//rgb led color
	//speaker message
	//buzzer beeping
    us_get_distance = US_u8GetDistance(void); // untill we know the id of us that we want
    //check us sensor (call us_get_distance func here)
    switch(us_get_distance)
    {
    	case 10 ... 40 :
    	//decrease motors speed
    	break;
    	default : break;
    }
}
	

/* 
   void get_ir_point(char *ir)   // check if i passed the intersection or not >>> should be created in the IR driver file
   {
	   
	  *ir = get_gpio_pin0();
	   
   }
*/




// int get_my_direction(void); >>> should be created in the mpu6050 driver file
/*
    int get_my_direction(void);
    void bridge_status(void);      // bridge_case var should be externed in my file
*/