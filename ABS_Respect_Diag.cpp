#include "ABS_Respect_Diag.h"//delshadi
#include "MSG_ABS_Respect_Diag.h"
#include "ArrayTools.h"
#include "OtherHeader\ABS_Respect_Diag_OH.h"
ABS_Respect_Diag::ABS_Respect_Diag(void)
{
}
ABS_Respect_Diag::~ABS_Respect_Diag(void)
{
}
//کالیبراسیون سنسور شتاب YAW
bool ABS_Respect_Diag::YawSensorCalibration(CommandAdorner* Command)
{
    byte response[255]={};
	  OBDii.Security_Pass(Command, 1);
		byte SessionFrame[2] = { 0x10, 0x03 };
		OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response));
		if(!OnShowMessage_Messages(ABSRespectDiag_MachineCondition, Yes_No, true))
			return false;
		byte Routine1[4] = { 0x31, 0x03, 0x31, 0x17 };
		OBDii.Send_Receive(sizeof(Routine1), Routine1, response, sizeof(response));
		if ((OBDii.Send_Receive(sizeof(Routine1), Routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F)|| (response[4] != 0x02))
				{
				return false; 
			  }
			Sleep(500);
			
					byte Routine2[4] = { 0x31, 0x01, 0x31, 0x17 };
					OBDii.Send_Receive(sizeof(Routine2), Routine2, response, sizeof(response)) ;
					
			
					float accLong = 0.0f;
					byte Routine3[3] = { 0x22, 0x30, 0x0E };
					OBDii.Send_Receive(sizeof(Routine3), Routine3, response, sizeof(response));
    
					if (response[3] > 0 && response[3] < 10)
					{
							accLong = 6.912f * response[3];
					}
					float accLat = 0.0f;
					byte Routine4[3] = { 0x22, 0x30, 0x0D };
					if ((OBDii.Send_Receive(sizeof(Routine4), Routine4, response, sizeof(response)) == NULL)|| (response[0] == 0x7F))
					{
							return false;
					}

					if (response[3] > 0 && response[3] < 10)
					{
							accLat = 6.912f * response[3];
					}
					char MSG[128] = "";
           
					snprintf(MSG, sizeof(MSG), "Long: %.2f\nLat: %.2f", accLong, accLat);

					OnShowMessage(MSG, Yes, true);			
				
	OnShowMessage_Messages(ABSRespectDiag_done, Yes_No, true);
			
	return true; 
}	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//هواگری بوش
bool ABS_Respect_Diag::RepairBleedBosch(CommandAdorner* Command)
{   int i =0;
    byte response[255]={};
  	OBDii.Security_Pass(Command, 1);
		byte SessionFrame[2] = { 0x10, 0x03 };
		OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response));
		if(!OnShowMessage_Messages(ABSRespectDiag_BrakePedalPressure, Yes_No, true))
			return false;			
		byte Routine1[3] = { 0x22, 0x30, 0x11};
		OBDii.Send_Receive(sizeof(Routine1), Routine1, response, sizeof(response));
		if(!OnShowMessage_Messages(ABSRespectDiag_BatteryVoltageCheck, Yes_No, true))
			return false;
		
    if (response[3] <= 0x64) {
      return false;
     }
		if ((OBDii.Send_Receive(sizeof(Routine1), Routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F))
			{
				return false; 
			}
			else 
			Sleep(500);
			if(!OnShowMessage_Messages(ABSRespectDiag_RelayCheck, Yes_No, true))
				return false;
			if(!OnShowMessage_Messages(ABSRespectDiag_BleedingSequence, Yes_No, true))
				return false;
			for (i = 0; i < 2; i++)
				{OBDii.Send_Receive(Command_SendingFrames(Command, i)->Lengh, Command_SendingFrames(Command, i)->ByteFrame, response, sizeof(response));
				if ( Command_SendingFrames(Command, i)->ByteFrame[1]==0x03)
				{	if (response[4]==1)
							{	if(!OnShowMessage_Messages(ABSRespectDiag_OperationInProgress, Yes_No, true))
									return false;}
						else if (response[4]==2)
						{if(!OnShowMessage_Messages(ABSRespectDiag_OperationSuccess, Yes_No, true))
									return false;}
						else if (response[4]==3)
						{if(!OnShowMessage_Messages(ABSRespectDiag_OperationFailed, Yes_No, true))
							return false;}
						else 
								return false;}
							}
		return true; 		
}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//هواگیری MK100
bool ABS_Respect_Diag::RepairBleed_MK100(CommandAdorner* Command)
{
  byte response[255]={};
  OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response));
	if(!OnShowMessage_Messages(ABSRespectDiag_BleedingDeviceRequired, Yes_No, true))
		return false;
	byte Routine1[4] = { 0x31 , 0x03 , 0x31 , 0x13 };
	if ( (OBDii.Send_Receive(sizeof(Routine1), Routine1, response, sizeof(response)) == NULL) || (response[4] != 0x01)|| (response[0] == 0x7F) )
	{
    return false;
	}

	StopKeepAlive();
	
	byte frame[4] = { 0x31 , 0x03 , 0x31 , 0x13};
	for(int i = 0; i < 4; i++)
	{
	  do
		{
		Sleep(1300);
		OBDii.Send_Receive(sizeof(frame), frame, response, sizeof(response));
		}while(response[4]!=0x03);
		
			  do
		{
		Sleep(1300);
		OBDii.Send_Receive(sizeof(frame), frame, response, sizeof(response));
		}while(response[4]!=0x01);
	}
			  do
		{
		Sleep(1300);
		OBDii.Send_Receive(sizeof(frame), frame, response, sizeof(response));
		}while(response[4]!=0x02);
	
	StartKeepAlive();
		
	byte Routine10[5] = { 0x31 , 0x03 , 0x31 , 0x13, 0x01 };
	OBDii.Send_Receive(sizeof(Routine10), Routine10, response, sizeof(response));
	if(!OnShowMessage_Messages(ABSRespectDiag_BrakePedalPress, Yes_No, true))
		return false;
	if(!OnShowMessage_Messages(ABSRespectDiag_FrontLeftComplete, Yes_No, true))
		return false;
	if(!OnShowMessage_Messages(ABSRespectDiag_ConnectBleederDevice, Yes_No, true))
		return false;
	byte Routine2[5]={0x31, 0x01,0x31, 0x13, 0x02 };
	OBDii.Send_Receive(sizeof(Routine2), Routine2, response, sizeof(response));
	if(!OnShowMessage_Messages(ABSRespectDiag_FrontRightComplete, Yes_No, true))
		return false;
	byte Routine3[5]={0x31, 0x01,0x31, 0x13, 0x03 };
	OBDii.Send_Receive(sizeof(Routine3), Routine3, response, sizeof(response));
	if(!OnShowMessage_Messages(ABSRespectDiag_RearRightComplete, Yes_No, true))
		return false;
	byte Routine[5]={0x31, 0x01,0x31, 0x13, 0x04 };
	OBDii.Send_Receive(sizeof(Routine), Routine, response, sizeof(response));
	OnShowMessage_Messages(ABSRespectDiag_RearLeftComplete, Yes_No, true);
	OnShowMessage_Messages(ABSRespectDiag_PressPedalUntilBubbleFree, Yes_No, true);
	OnShowMessage_Messages(ABSRespectDiag_BleedingComplete, Yes_No, true);
			return true;
}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//تعویض لنت ترمز عقب
bool ABS_Respect_Diag::FPBReplacement(CommandAdorner* Command)
{
    int i =0;
	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
		
		for (i = 0; i < 2; i++)
	  OBDii.Send_Receive(Command_SendingFrames(Command, i)->Lengh, Command_SendingFrames(Command, i)->ByteFrame, response, sizeof(response));
	    if ( Command_SendingFrames(Command, i)->ByteFrame[1]==0x03)
		  {
		     if (response[4]==1)
				 {if(! OnShowMessage_Messages(ABSRespectDiag_OperationInProgress, Yes_No, true))
					 return false;}
				 else if (response[4]==2)
				 {if(!	OnShowMessage_Messages(ABSRespectDiag_OperationSuccess, Yes_No, true))
					 return false;}
				 else if (response[4]==3)
				 {if(!OnShowMessage_Messages(ABSRespectDiag_OperationFailed, Yes_No, true))
					 return false;}
				 else 
					 return false;
	    }
		
	return true; 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//هواگیری دستی 
bool ABS_Respect_Diag::ManualBleeding(CommandAdorner* Command)
{ 
	int i =0;
	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
		if(!OnShowMessage_Messages(ABSRespectDiag_BrakePedalPressure, Yes_No, true))
			return false;			
		byte Routine1[3] = { 0x22, 0x30, 0x11};
		if ((OBDii.Send_Receive(sizeof(Routine1), Routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F))
			{
				return false; 
			}
			else
		if(!OnShowMessage_Messages(ABSRespectDiag_BatteryVoltageCheck, Yes_No, true))
			return false;
		 if (response[3] <= 0x64) {
      return false;
     }
			Sleep(500);
			if(!OnShowMessage_Messages(ABSRespectDiag_RelayCheck, Yes_No, true))
				return false;
			if(!OnShowMessage_Messages(ABSRespectDiag_BleedingSequence, Yes_No, true))
				return false;
	for (i = 0; i < 2; i++)
  	OBDii.Send_Receive(Command_SendingFrames(Command, i)->Lengh, Command_SendingFrames(Command, i)->ByteFrame, response, sizeof(response));		
	
	byte Routine[4] ={0x31,0x03,0x30,0x3B};
	if ((OBDii.Send_Receive(sizeof(Routine), Routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	 if (response[4]==1)
	 {if(! OnShowMessage_Messages(ABSRespectDiag_OperationInProgress, Yes_No, true))
		 return false;}
				 else if (response[4]==2)
				 {if(!	OnShowMessage_Messages(ABSRespectDiag_OperationSuccess, Yes_No, true))
					 return false;}
				 else if (response[4]==3)
				 {if(!	OnShowMessage_Messages(ABSRespectDiag_OperationFailed, Yes_No, true))
					 return false;}
				 else 
					 return false;
	return true; 
}		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
bool ABS_Respect_Diag::HydrolicAirVolume(CommandAdorner* Command)
{
//	int i =0;
	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	byte routine[4] = {0x31,0x03,0x30,0x42};
	if ((OBDii.Send_Receive(sizeof(routine), routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	char MSG[100] = "";
	int Out = 0;
	if ((response[7] % 2) == 0)
	{
		Out = 4 + response[8] / 85;
	}
	else
	{
		Out = 3 * response[8] / 255;
	}
	snprintf(MSG, sizeof(MSG), "Hydrolic Air Volume : %d", Out);
	if(!OnShowMessage(MSG, Yes, true))
		return false;
	byte routine1[4]={0x31,0x01,0x30,0x42};
	if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
			{
				return false; 
			}
return true; 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::HydrolicAirSpeed(CommandAdorner* Command)
{
//	int i =0;
	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	byte routine[4] = {0x31,0x03,0x30,0x42};
	if ((OBDii.Send_Receive(sizeof(routine), routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	char MSG[100] = "";
	int Out = 0;
	unsigned char val = response[8];
	if      (/*val >= 0x00 &&*/ val <= 0x0F) Out = 0;
	else if (val >= 0x10 && val <= 0x1F) Out = 1;
	else if (val >= 0x20 && val <= 0x2F) Out = 2;
	else if (val >= 0x30 && val <= 0x3F) Out = 3;
	else if (val >= 0x40 && val <= 0x4F) Out = 4;
	else if (val >= 0x50 && val <= 0x5F) Out = 5;
	else if (val >= 0x60 && val <= 0x6F) Out = 6;
	else if (val >= 0x70 && val <= 0x7F) Out = 7;
	else if (val >= 0x80 && val <= 0x8F) Out = 8;
	else if (val >= 0x90 && val <= 0x9F) Out = 9;
	else if (val >= 0xA0 && val <= 0xAF) Out = 10;
	else if (val >= 0xB0 && val <= 0xBF) Out = 11;
	else if (val >= 0xC0 && val <= 0xCF) Out = 12;
	else if (val >= 0xD0 && val <= 0xDF) Out = 13;
	else if (val >= 0xE0 && val <= 0xEF) Out = 14;
	else if (val >= 0xF0 && val <= 0xFF) Out = 15;
	snprintf(MSG, sizeof(MSG),  " Hydrolic Air Speed: %d", Out);
	if(!OnShowMessage(MSG, Yes, true))
		return false;
	byte routine1[4] = {0x31,0x01,0x30,0x42};
		if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
			{
				return false; 
			}
	return true; 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::PlungerairVolume(CommandAdorner* Command)
{
//	int i =0;
	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	byte routine[4] = {0x31,0x03,0x30,0x42};
	if ((OBDii.Send_Receive(sizeof(routine), routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	char MSG[100] = "";
	int Out = 0;
	Out = (response[7] & 0x7F) >> 3;   // 0..15
	snprintf(MSG, sizeof(MSG), "Plunger air volume : %d", Out);
	if(!OnShowMessage(MSG, Yes, true))
		return false;
	byte routine1[4] = {0x31,0x01,0x30,0x42};
		if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
			{
				return false; 
			}
	return true; 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::HydrolicLeakVolume(CommandAdorner* Command)
{
//	int i =0;
	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	byte routine[4] = {0x31,0x03,0x30,0x42};
	if ((OBDii.Send_Receive(sizeof(routine), routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	char MSG[100] = "";
	int Out = 0;

	unsigned char val = (unsigned char)response[9] & 0x7F; 
	Out = (val / 2) * 16;                                   

	snprintf(MSG, sizeof(MSG), "Hydraulic leak volume : %d", Out);

	if(!OnShowMessage(MSG, Yes, true))
		return false;
	byte routine1[4] = {0x31,0x01,0x30,0x42};
		if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
			{
				return false; 
			}
	return true; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::HydrolicLeakSpeed(CommandAdorner* Command)
{
//	int i =0;
	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	byte routine[4] = {0x31,0x03,0x30,0x42};
	if ((OBDii.Send_Receive(sizeof(routine), routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	char MSG[100] = "";
	int Out = 0;

	unsigned char val9  = (unsigned char)response[9];
	unsigned char val10 = (unsigned char)response[10];


	int base = (val10 >> 3) * 16;   

	if ((val9 & 1) == 0)           
	{
		Out = base;
	}
	else                            
	{
		Out = base + 512;
	}
	snprintf(MSG, sizeof(MSG), "Hydraulic leak speed : %d", Out);

	if(!OnShowMessage(MSG, Yes, true))
		return false;

		byte routine1[4] = {0x31,0x01,0x30,0x42};
		if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
			{
				return false; 
			}
	return true; 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::InputrodPos100(CommandAdorner* Command)
{
//	int i =0;
	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	
	byte routine[4] = {0x31,0x03,0x30,0x3F};
	if ((OBDii.Send_Receive(sizeof(routine), routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	char MSG[100] = "";
	int Out = 0;
	bool valid = true;

	unsigned char val = (unsigned char)response[8];

	if (val <= 0x63)
	{
		valid = false;
	}
	else if (val >= 0x64 && val <= 0x7F)
	{
		
		Out = 50 + ((int)(val - 0x64) * 14) / (0x7F - 0x64);
	}
	else if (val >= 0x80 && val <= 0xED)
	{
		
		Out = -64 + ((int)(val - 0x80) * 54) / (0xED - 0x80);
	}
	else 
	{
		valid = false;
	}

	if (!valid)
	{
		 snprintf(MSG, sizeof(MSG), "Input rod pos 100 =Invalid");
	}
	else
	{
		
		 snprintf(MSG, sizeof(MSG), "Input rod pos 100 : %d", Out);
	}

	if(!OnShowMessage(MSG, Yes, true))
		return false;
	byte routine1[9] = {0x31,0x01,0x30,0x3F,0x03,0xC0,0x0A,0x00,0x01};
	if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}

	if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	return true; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::InputrodPos200(CommandAdorner* Command)
{

	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	
	byte routine[4] = {0x31,0x03,0x30,0x3F};
	if ((OBDii.Send_Receive(sizeof(routine), routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	char MSG[100] = "";
	int Out = 0;
	bool valid = true;

	unsigned char val = (unsigned char)response[12];

	if (val <= 0x63)
	{
		valid = false;
	}
	else if (val >= 0x64 && val <= 0x7F)
	{
	
		Out = 50 + ((int)(val - 0x64) * 14) / (0x7F - 0x64);
	}
	else if (val >= 0x80 && val <= 0xED)
	{
		
		Out = -64 + ((int)(val - 0x80) * 54) / (0xED - 0x80);
	}
	else 
	{
		valid = false;
	}

	if (!valid)
	{
		 snprintf(MSG, sizeof(MSG), "Input rod pos 100 =Invalid");
	}
	else
	{
		
		 snprintf(MSG, sizeof(MSG), "Input rod pos 200 : %d", Out);
	}

	if(!OnShowMessage(MSG, Yes, true))
		return false;
	byte routine1[9] = {0x31,0x01,0x30,0x3F,0x03,0xC0,0x0A,0x00,0x01};
	if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}

	if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	return true; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::TMCPressP100(CommandAdorner* Command)
{

	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
		byte routine[4] = {0x31,0x03,0x30,0x3F};
	if ((OBDii.Send_Receive(sizeof(routine), routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
		char MSG[100] = "";
    int Out = 0;
		unsigned char val = (unsigned char)response[6];
		if (val <= 0x1F)
    {
        Out = val * 8;
        snprintf(MSG, sizeof(MSG), "TMC Press P100 : %d", Out);
    }
    else
    {
        snprintf(MSG, sizeof(MSG), "TMC Press P100 = Invalid");
    }
		 
	if(!OnShowMessage(MSG, Yes, true))
		return false;
	byte routine1[9] = {0x31,0x01,0x30,0x3F,0x03,0xC0,0x0A,0x00,0x01};
	if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
	return true; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::TMCPressP200(CommandAdorner* Command)
{
	//int i =0;
	byte response[255]={};
	OBDii.Security_Pass(Command, 1);
	byte SessionFrame[2] = { 0x10, 0x03 };
	if ((OBDii.Send_Receive(sizeof(SessionFrame), SessionFrame, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
		byte routine[4] = {0x31,0x03,0x30,0x3F};
	if ((OBDii.Send_Receive(sizeof(routine), routine, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}
		char MSG[100] = "";
    int Out = 0;
	unsigned char val = (unsigned char)response[10];
	if (val <= 0x1F)
			{
					Out = val * 8;
					snprintf(MSG, sizeof(MSG), "TMC Press P100 : %d", Out);
			}
			else
			{
					snprintf(MSG, sizeof(MSG), "TMC Press P100 = Invalid");
			}
	if(!OnShowMessage(MSG, Yes, true))
		return false;
	byte routine1[9] = {0x31,0x01,0x30,0x3F,0x03,0xC0,0x0A,0x00,0x01};
	if ((OBDii.Send_Receive(sizeof(routine1), routine1, response, sizeof(response)) == NULL) || (response[0] == 0x7F) )
		{
			return false; 
		}	
	return true; 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::delay_exitsuported(u32 delay, char* msg, bool show_timer)
{
	u16 cunt = 10;
	char msg_2[255] = "";
	MSG_Redraw redraw = MSG_Redraw_true;
	while (delay && !Manager.BreakFlag)
	{
		Sleep(100);
		if ((msg || show_timer) && cunt == 10)
		{
			if (msg) sprintf(msg_2, "%s\n%s: [%02d : %02d]", (msg == null ? "" : msg), "Remaining Time", (delay / 60000), (delay % 60000) / 1000);
			else sprintf(msg_2, "%s: [%02d : %02d]", "Remaining Time", (delay / 60000), (delay % 60000) / 1000);
			OnShowMessage(msg_2, NoneWithoutWait, false, redraw);
			cunt = 0;
		}
		redraw = MSG_Redraw_false;
		cunt++;
		delay -= 100;
	}
	if (Manager.BreakFlag)
	{
		Manager.BreakFlag = false;
		GetLastKey();
		return false;
	}
	Sleep(delay % 100);
	return true;
}

bool ABS_Respect_Diag::Keyboard_Actuator(CommandAdorner* Command)
{
	byte response[255] = {};
	OBDii.Security_Pass(Command, 1);
	char MSG[20] = "";
	sprintf(MSG, "0 - %d", (u8)(Command->DB->FaultCodeLength));
	u8 Output = 0;
	if ((Command->DB->FaultCodeLength) == 28) 
	{	
		
		Output = (u8)(( (GetDecimalDataDialog(MSG, 0, 28) > 99 ? 99 : GetDecimalDataDialog(MSG, 0, 28)) * 4.82) + 0x6a);
	}
	else if ((Command->DB->FaultCodeLength) == 20) 
	{
		Output = (u8)(( (GetDecimalDataDialog(MSG, 0, 20) > 106 ? 106 : GetDecimalDataDialog(MSG, 0, 20)) * (-4.82)) + 0x6a);
	}
	else if ((Command->DB->FaultCodeLength) == 100)	
	{
		
		Output = (u8)(( (GetDecimalDataDialog(MSG, 0, 100) > 2550 ? 2550 : GetDecimalDataDialog(MSG, 0, 100)) * 2.4));
	}
	CommandAdorner::SendingFrame_ByteArrayFrame_structure* sendFrame = Command_SendingFrames(Command, 0);
	byte send_buff[5] = { sendFrame->ByteFrame[0], sendFrame->ByteFrame[1], sendFrame->ByteFrame[2], sendFrame->ByteFrame[3], Output };

	if ((OBDii.Send_Receive(sizeof(send_buff), send_buff, response, sizeof(response)) == null) || (response[0] == 0x7F))
	{
		Manager.BreakFlag = true;
		Command->State.IsFaulty = true;
		return true;
	}
		
	delay_exitsuported(5000, null, true);
	OBDii.Send_Receive(Command_SendingFrames(Command, 1)->Lengh, Command_SendingFrames(Command, 1)->ByteFrame, response, sizeof(response));
	OBDii.Send_Receive(Command_SendingFrames(Command, 2)->Lengh, Command_SendingFrames(Command, 2)->ByteFrame, response, sizeof(response));
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////
bool ABS_Respect_Diag::ABSDTC(CommandAdorner* Command)
{
    byte res[1424];
	  OBDii.Security_Pass(Command, 1);
    int res_len = 0;
    memset(res, 0, sizeof(res));
    u32 code = 0;
    byte send_buff[3] = { 0x19, 0x02, 0xFF };
    FaultCodeAdorner _FaultCodeAdorner;

    res_len =  (int)OBDii.Send_Receive(sizeof(send_buff), send_buff, res, sizeof(res));

    if (res_len < 7 || res[0] == 0x7F)
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
        return true;
    }

    for (int i = 3; (i + 3) < res_len; i += 4)
    {
      code = 0;
			code |= (res[i] << 16);
			code |= (res[i + 1] << 8);
			code |= res[i + 2];

        if (code == 0xFFFF || code == 0x0000)
            continue;

        byte state = res[i + 3];

        if (_FaultCodeAdorner.CheckFault(code,state, Command))
        {
           if (((state & 0x09) == 0x09))
            {
                _FaultCodeAdorner.AddFault(code,null,GetDTCCategory(code, Command->DB->FaultCodeLength),GetDTCinCategoryMode(code,Command->DB->FaultCodeLength));
            }
        }
    }

    _FaultCodeAdorner.ShowFaults(Command, true);
    Manager.BreakFlag = true;
    return true;
}
