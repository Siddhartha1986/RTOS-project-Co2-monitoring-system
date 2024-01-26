/*
 ===============================================================================
 Name        : main.c
 Author      : Siddhartha
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "heap_lock_monitor.h"
#include "retarget_uart.h"
#include "Modbus/ModbusRegister.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"
#include <string>
#include <ctype.h>
#include "eeprom.h"
#include "board.h"
#include "mqtt_demo/MQTT.h"

// Define extern C functions for MQTT
extern "C"
 {
	uint32_t prvGetTimeMs(void);
	PlaintextTransportStatus_t prvConnectToServerWithBackoffRetries( NetworkContext_t * pxNetworkContext );
	void prvCreateMQTTConnectionWithBroker( MQTTContext_t * pxMQTTContext, NetworkContext_t * pxNetworkContext );
	void prvMQTTSubscribeWithBackoffRetries( MQTTContext_t * pxMQTTContext );
	void prvMQTTPublishToTopic( char *, MQTTContext_t * pxMQTTContext );
	MQTTStatus_t MQTT_ProcessLoop( MQTTContext_t * pContext, uint32_t timeoutMs );

 }

// Define external C function for runtime stats
extern "C"
{

	void vConfigureTimerForRunTimeStats(void)
	{
		Chip_SCT_Init(LPC_SCTSMALL1);
		LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
		LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
	}

}

// Global variables
std::string co2;
int co2level = 0;
int co2value = 0, temperaturevalue = 0, humidityvalue = 0, Valvevalue=0;

// Function to read CO2 level from EEPROM
void read_co2_level(void)
{
	// Turn on EEPROM
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_EEPROM);
    Chip_SYSCTL_PeriphReset(RESET_EEPROM);

	// Read the EEPROM
    uint8_t buffer[100];
    Chip_EEPROM_Read(0x00000100, buffer, 100);

	int i = 0;

	if(buffer[i]=='-')
	{
		co2 += buffer[i];
		i++;
	}

	while(isdigit(buffer[i]))
	{
		co2 += buffer[i];
		i++;
	}
}

// Function to write CO2 level to EEPROM
void write_co2_level(void)
	{
    	uint8_t buffer[100];
    	for(int i = 0; i < co2.length(); i++)
    	{
    		buffer[i] = co2[i];
    	}

    	buffer[co2.length()] = 'x';
    	Chip_EEPROM_Write(0x00000100, buffer, 100);
	}


// Define shared buffer and other global variables
uint8_t ucSharedBuffer[ mqttexampleSHARED_BUFFER_SIZE ];
uint32_t ulGlobalEntryTimeMs;
uint16_t usSubscribePacketIdentifier;
uint16_t usUnsubscribePacketIdentifier;


// MQTT Task function
static void prvMQTTTask( void * pvParameters )
{
    uint32_t ulPublishCount = 0U, ulTopicCount = 0U;
    const uint32_t ulMaxPublishCount = 5UL;
    NetworkContext_t xNetworkContext = { 0 };
    PlaintextTransportParams_t xPlaintextTransportParams = { 0 };
    MQTTContext_t xMQTTContext;
    MQTTStatus_t xMQTTStatus;
    PlaintextTransportStatus_t xNetworkStatus;

    /* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

    /* Set the pParams member of the network context with desired transport. */
    xNetworkContext.pParams = &xPlaintextTransportParams;

    ulGlobalEntryTimeMs = prvGetTimeMs();
    char buff[60];

    for( ; ; )
    {
        /****************************** Connect. ******************************/

        /* Attempt to connect to the MQTT broker. If connection fails, retry after
         * a timeout. The timeout value will exponentially increase until the
         * maximum number of attempts are reached or the maximum timeout value is
         * reached. The function below returns a failure status if the TCP connection
         * cannot be established to the broker after the configured number of attempts. */
        xNetworkStatus = prvConnectToServerWithBackoffRetries( &xNetworkContext );
        configASSERT( xNetworkStatus == PLAINTEXT_TRANSPORT_SUCCESS );

        /* Sends an MQTT Connect packet over the already connected TCP socket,
         * and waits for a connection acknowledgment (CONNACK) packet. */
        LogInfo( ( "Creating an MQTT connection to %s.", democonfigMQTT_BROKER_ENDPOINT ) );
        prvCreateMQTTConnectionWithBroker( &xMQTTContext, &xNetworkContext );


        /******************* Publish and Keep Alive Loop. *********************/

        /* Publish messages with QoS0, then send and process Keep Alive messages. */
        for( ;; )
        {

        	sprintf(buff, "field1=%d&field2=%d&field3=%d&field4=%d&field5=%d", co2value, humidityvalue, temperaturevalue, Valvevalue, co2level);
        	prvMQTTPublishToTopic( buff, &xMQTTContext );
        	vTaskDelay(2000);
        }
    }
}




// LCD Task function
void lcd_task(void *pvParameters)
{
	// create lcd
	DigitalIoPin pinrs(0, 29, DigitalIoPin::output);
	DigitalIoPin pinen(0, 9, DigitalIoPin::output);
	DigitalIoPin pind4(0, 10, DigitalIoPin::output);
	DigitalIoPin pind5(0, 16, DigitalIoPin::output);
	DigitalIoPin pind6(1, 3, DigitalIoPin::output);
	DigitalIoPin pind7(0, 0, DigitalIoPin::output);
	LiquidCrystal lcd(&pinrs, &pinen, &pind4, &pind5, &pind6, &pind7);
	lcd.begin(16, 2);

	DigitalIoPin button(1, 8, DigitalIoPin::pullup,true);
	DigitalIoPin siga(0, 5, DigitalIoPin::pullup);
	DigitalIoPin sigb(0, 6, DigitalIoPin::pullup);

	bool set_co2 = false;
    co2level = atoi(co2.c_str());
	bool lasta = siga.read();


	while(true)
	{

		std::string co2string = std::to_string(co2value),
		temperaturestring = std::to_string(temperaturevalue),
		humiditystring = std::to_string(humidityvalue),
		co2levelstring = std::to_string(co2level),
		Valvestring = std::to_string(Valvevalue);


		lcd.setCursor(0, 0);
		lcd.print("C="+co2string + " " +"t=" + temperaturestring + " " +"rh="+ humiditystring + "            ");

		lcd.setCursor(0, 1);
		lcd.print("co2setPT=" + co2levelstring + " " +"V=" + Valvestring + "               ");


		  if(set_co2)
			{
				  bool a = siga.read();
				  bool b = sigb.read();


				if(lasta == true && a == false)
				{
						if(b == false)
					{
						co2level += 10;
						DEBUGSTR("RIGHT\r\n");
					}

						else if(b == true)
						{
							co2level -= 10;
							DEBUGSTR("LEFT\r\n");
						}

				}
				lasta=a;
			}



		if(button.read())
		{
			DEBUGSTR("Click\r\n");
			vTaskDelay(400);
			if(set_co2)
			{
				co2 = std::to_string(co2level);
				write_co2_level();   // writing the value to EEPROM
				set_co2 = false;
			}
				else
				  {
					set_co2 = true;
				  }
		}

		vTaskDelay(60);
	}
}

// Sensor Task function
void sensor_task(void *pvParameters)
{
	ModbusMaster humiditytemperature(241);
	humiditytemperature.begin(9600);
	ModbusRegister humidity(&humiditytemperature, 256, true);
	ModbusRegister temperature(&humiditytemperature, 257, true);

	ModbusMaster co2sensor(240);
	co2sensor.begin(9600);

	ModbusRegister co2val(&co2sensor, 257, true);

	ModbusRegister co2status(&co2sensor, 0x800, true);
	ModbusRegister humiditytemperaturestatus(&humiditytemperature, 0x200, true);

	DigitalIoPin Valve(0, 27, DigitalIoPin::output);

	int offset = 10;
	while(true){


	if(humiditytemperaturestatus.read())
	{
		vTaskDelay(10);
		temperaturevalue = temperature.read() / 10;
		vTaskDelay(10);
		humidityvalue = humidity.read() / 10;
	}


	vTaskDelay(10);
	if(co2status.read() == 0)
		{
			vTaskDelay(10);
			co2value = co2val.read() * 10;
		}

				if (co2value <= 0)
				{
				    Valve.write(false);
					Valvevalue = false;
				}
				else if(co2level > (co2value + offset))
				{
					Valve.write(true);
					Valvevalue = true;

			    }
				else
				{
					Valve.write(false);
					Valvevalue = false;

			    }


					vTaskDelay(500);


	}
}



int main(void)
{

	#if defined (__USE_LPCOPEN)
		// Read clock settings and update SystemCoreClock variable
		SystemCoreClockUpdate();
	#if !defined(NO_BOARD_LIB)
		// Set up and initialize all required blocks and
		// functions related to the board hardware
		Board_Init();
		// Set the LED to the state of "On"
		Board_LED_Set(0, true);
	#endif
	#endif
	heap_monitor_setup();

	read_co2_level();

	// Creating three separate tasks for MQTT_task, sensor_task, lcd_task

	xTaskCreate( prvMQTTTask,"MQTT-TASK",
	configMINIMAL_STACK_SIZE+1024,NULL,tskIDLE_PRIORITY +1UL,NULL );

	xTaskCreate(sensor_task, "sensor_task",
	configMINIMAL_STACK_SIZE * 8, NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t*) NULL);

	xTaskCreate(lcd_task, "lcd_task",
	configMINIMAL_STACK_SIZE * 8, NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t*) NULL);


	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
