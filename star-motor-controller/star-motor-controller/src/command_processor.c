#include "asf.h"
#include "command_processor.h"
#include "comm.h"
#include <semphr.h>
#include <portmacro.h>
#include <string.h>

void CommandProcessorTask(void *data)
{
	uint32_t bufferIndex = 0;
	
	for(;;)
	{
		uint32_t taskNotificationResult = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));
		
		if (taskNotificationResult != 0)
		{
			xSemaphoreTake( ApplicationBufferMutex, portMAX_DELAY );
			
			bool foundHeader = true;
			
			while (foundHeader)
			{
				CommPacket * packet;
				foundHeader = false;

				
				for (bufferIndex = 0; bufferIndex  < ApplicationBufferLevel; ++bufferIndex)
				{
					packet = (CommPacket*)&ApplicationBuffer[bufferIndex];
					if (packet->header == COMM_PACKET_HEADER)
					{
						foundHeader = true;
						// Move everything back to 0 since we found a header marker
						// ... unless were already at 0
						if (bufferIndex != 0)
						{
							memmove(ApplicationBuffer, &ApplicationBuffer[bufferIndex], ApplicationBufferLevel - bufferIndex );
							ApplicationBufferLevel -= bufferIndex ;
						}
						break;
					}
				}
				
				if (foundHeader)
				{
					switch(packet->command)
					{
						case SIGNAL_BOOT:
						break;
						case SIGNAL_MOTOR_DEC_STATE:
						break;
						case SIGNAL_MOTOR_DEC_POSITION:
						break;
						case SIGNAL_MOTOR_RA_STATE:
						break;
						case SIGNAL_MOTOR_RA_POSITION:
						break;
						case SIGNAL_LNB_POWER_STATE:
						break;
						case COMMAND_MOTOR_DEC_SET_STATE:
						break;
						case COMMAND_MOTOR_DEC_SET_POSITION:
						break;
						case COMMAND_MOTOR_RA_SET_STATE:
						break;
						case COMMAND_MOTOR_RA_SET_POSITION:
						break;
						case COMMAND_LNB_SET_POWER_STATE:
						break;
						case COMMAND_STOP_ALL_MOTORS:
						break;
						case ERROR:
						break;
						default:
						break;
					}
				}
			}
			xSemaphoreGive( ApplicationBufferMutex );
			
		}
	}
}