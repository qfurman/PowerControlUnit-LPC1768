/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2003
 *
 *    File name   : LPC_Rtc.c
 *    Description : Define API for RTI
 *
 *    History :
 *    1. Data        : August 10, 2004
 *       Author      : Shawn Zhang
 *       Description : Create
 *
 *    2. Data        : Oct 12, 2004
 *       Author      : Stanimir Bonev
 *       Description: Modify the interface of some functions
 *
 *    $Revision: 1.1 $
 **************************************************************************/

#include "type.h"
#include    <myiolpc1768.h>
#include    <stdbool.h>
#include "main.h"
//#include "modbus.h"

#define RTC_YEARMIN         1901
#define RTC_YEARMAX         2099

static int RTC_MonthVal[]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// 1901.1.1 DOW = 2
#define RTC_BASEYEAR        1901
#define RTC_BASEMONTH       1
#define RTC_BASEDAY         1
#define RTC_BASEDOW         2


/*************************************************************************
 * Function Name: IsLeapYear
 * Parameters: unsigned short Year
 *
 * Return: bool
 *
 * Description: Judge whether the specifying year is leap year.
 *
 *************************************************************************/
bool IsLeapYear (unsigned short year)
{
  if (!(year%4) && (year%100) || !(year%400))
    return true;
  return false;
}

/*************************************************************************
 * Function Name: GetDOY
 * Parameters:  unsigned short Year
 *	  	unsigned char month
 *		unsigned char day
 *
 * Return: int
 *
 * Description: Get the day of year according to the date
 *
 *************************************************************************/

/*************************************************************************
 * Function Name: GetDOW
 * Parameters:  unsigned short Year
 *		unsigned char month
 *		unsigned char day
 *
 * Return: int -- (0~6)
 *
 * Description: Get the day of week according to the date.
 *
 * NOTE: Year is not smaller than RTC_YEARMIN (1901).
 *
 *************************************************************************/


/*************************************************************************
 * Function Name: RTC_Enable
 * Parameters: void
 * Return: void
 *
 * Description: Enable RTC, let RTC run.
 *
 *************************************************************************/
void RTC_Enable(void)
{
  RTCCCR_bit.CLKEN = true;
  return ;
}

/*************************************************************************
 * Function Name: RTC_Disable
 * Parameters: void
 * Return: void
 *
 * Description: Disable RTC, let RTC stop.
 *
 *************************************************************************/
void RTC_Disable(void)
{
  RTCCCR_bit.CLKEN = false;
  return ;
}

/*************************************************************************
 * Function Name: RTC_Init
 * Parameters: void
 *
 * Return: int
 *             	0: sucess
 *		1: fail
 *
 * Description: Initialize RTC, configure prescaler, CIIR and AMR register
 *
 *************************************************************************/
int RTC_Init(void)
{
  RTC_Disable();

  /* reset*/
  RTCCCR_bit.CTCRST = 1;
  RTCCCR_bit.CTCRST = 0;
  // initialize interrupt mask register of RTC
  RTCAMR=0;
  RTCCIIR_bit.IMSEC = 1;

  // clear all interrupt of RTC
  RTCILR=0x3;
/*	
  // initialize Date and Time
  if (RTC_SetDateTime(&RTC_InitDateTime))
    return 1;
*/
  //RTCCCR_bit.CLKEN = 1;
  RTCCCR_bit.CCALEN = 1;
  RTC_Enable();
  //PCONP_bit.PCRTC = 0;  //switch off the peripheral clock (pclk) to RTC. Any further writes to the RTC would
                        //require this bit to be set.This will reduce the power consumed from VBAT.
  return 0;
}


/*************************************************************************
 * Function Name: RTC_GetDate
 * Parameters: LPC_Rtc_Date_t *pDate
 * Return: int
 *             	0: sucess
 *		1: fail
 * Description: Get the current date
 *
 *************************************************************************/
int RTC_GetDate (LPC_Rtc_Date_t *pDate)
{

  pDate->day = RTCDOM;
  pDate->month = RTCMONTH;
  pDate->year = RTCYEAR;

  pDate->DOW = RTCDOW;
  pDate->DOY = RTCDOY;

  return 0;
}

/*************************************************************************
 * Function Name: RTC_GetTime
 * Parameters: LPC_Rtc_Time_t *pTime
 * Return: int
 *             	0: sucess
 *		1: fail
 * Description: Set the current time
 *
 *************************************************************************/
int RTC_GetTime (LPC_Rtc_Time_t *pTime)
{

  pTime->hour = RTCHOUR;
  pTime->minute = RTCMIN;
  pTime->second = RTCSEC;

  return 0;
}

/*************************************************************************
 * Function Name: RTC_GetDateTime
 * Parameters: LPC_Rtc_DateTime_t *pDateTime
 * Return: int
 *             	0: sucess
 *		1: fail
 * Description: Get the current date and time
 *
 *************************************************************************/
int RTC_GetDateTime (LPC_Rtc_DateTime_t *pDateTime)
{

  pDateTime->day = RTCDOM;
  pDateTime->month = RTCMONTH;
  pDateTime->year = RTCYEAR;

  pDateTime->hour = RTCHOUR;
  pDateTime->minute = RTCMIN;
  pDateTime->second = RTCSEC;

  pDateTime->DOW = RTCDOW;
  pDateTime->DOY = RTCDOY;

  return 0;
}
/*************************************************************************/
/*
int RTC_GetAlarmDateTime (LPC_Rtc_DateTime_t *pDateTime)
{

  pDateTime->day = ALDOM;
  pDateTime->month = ALMON;
  pDateTime->year = ALYEAR;

  pDateTime->hour = ALHOUR;
  pDateTime->minute = ALMIN;
  pDateTime->second = ALSEC;

  pDateTime->DOW = ALDOW;
  pDateTime->DOY = ALDOY;

  return 0;
}
*/
/*************************************************************************
 * Function Name: RTC_SetInctInt
 * Parameters: unsigned char IncIntType
 * Return: void
 *
 * Description: Set increment interrupt type
 *
 *************************************************************************/
void RTC_SetIncInt (unsigned char IncIntType)
{
    //PCONP_bit.PCRTC = 1;  //switch on the peripheral clock (pclk) to RTC.
    RTCCIIR = IncIntType & 0xFF;
    //PCONP_bit.PCRTC = 0;  //switch off the peripheral clock (pclk) to RTC.
  return;
}

/*************************************************************************
 * Function Name: RTC_DisableIncInt
 * Parameters: void
 * Return: void
 *
 * Description: Disable RTC increment interrupt.
 *
 *************************************************************************/
void RTC_DisableIncInt(void)
{
    //PCONP_bit.PCRTC = 1;  //switch on the peripheral clock (pclk) to RTC.
    RTCCIIR = 0;
    //PCONP_bit.PCRTC = 0;  //switch off the peripheral clock (pclk) to RTC.
  return ;
}

/*************************************************************************
 * Function Name: RTC_SetAlarmtInt
 * Parameters: unsigned char AlarmIntType
 * Return: void
 *
 * Description: Set alarm interrupt type
 *
 *************************************************************************/
void RTC_SetAlarmInt (unsigned char AlarmIntType)
{
    //PCONP_bit.PCRTC = 1;  //switch on the peripheral clock (pclk) to RTC.
    RTCAMR = ~AlarmIntType & 0xFF;
    //PCONP_bit.PCRTC = 0;  //switch off the peripheral clock (pclk) to RTC.
  return;
}

/*************************************************************************
 * Function Name: RTC_DisableAlarmInt
 * Parameters: void
 * Return: void
 *
 * Description: Disable RTC alarm interrupt.
 *
 *************************************************************************/
void RTC_DisableAlarmInt(void)
{
    //PCONP_bit.PCRTC = 1;  //switch on the peripheral clock (pclk) to RTC.
    RTCAMR = 0xFF;
    //PCONP_bit.PCRTC = 0;  //switch off the peripheral clock (pclk) to RTC.
}
/*************************************************************************
 * Function Name: IsValidDay
 * Parameters:  unsigned short Year
 *		unsigned char month
 *	        unsigned char day
 *
 * Return:
 *		false -- not valid day
 *		true -- valid day
 *
 * Description: Check if the specify day is valid
 *
 * NOTE: Year is not smaller than RTC_YEARMIN (1901).
 *
 *************************************************************************/
static bool IsValidDay (unsigned short year, unsigned char month, unsigned char day)
{
  /* Valid Judge */
  if (year < RTC_YEARMIN || year > RTC_YEARMAX ||day <1)
    return false;

  switch(month)
  {
  case 1:
  case 3:
  case 5:
  case 7:
  case 8:
  case 10:
  case 12:
    if (day>31)
      return false;
    else
      break;
  case 4:
  case 6:
  case 9:
  case 11:
    if (day>30)
      return false;
    else
      break;
  case 2:
    if (IsLeapYear(year))
      if (day>29)
        return false;
      else
        break;
    else
      if (day>28)
        return false;
      else
        break;
  default:
    return false;
  }

  return true;
}
/*************************************************************************
 * Function Name: GetDOY
 * Parameters:  unsigned short Year
 *	  	unsigned char month
 *		unsigned char day
 *
 * Return: int
 *
 * Description: Get the day of year according to the date
 *
 *************************************************************************/
int GetDOY (unsigned short year, unsigned char month, unsigned char day)
{
int DOY=0, i;

  for(i=1; i<month; i++)
    DOY+=RTC_MonthVal[i];

  if (month>2)
    if (IsLeapYear(year))
      DOY++;

  return (DOY+day);	
}

/*************************************************************************
 * Function Name: GetDOW
 * Parameters:  unsigned short Year
 *		unsigned char month
 *		unsigned char day
 *
 * Return: int -- (0~6)
 *
 * Description: Get the day of week according to the date.
 *
 * NOTE: Year is not smaller than RTC_YEARMIN (1901).
 *
 *************************************************************************/
static int GetDOW (unsigned short year, unsigned char month, char day)
{
int i, DOW=0;

  for (i = RTC_BASEYEAR, DOW = 0; i < year; i++)
  {
    DOW +=365;
    if  (IsLeapYear(i))
      DOW++;
  }

  DOW +=  GetDOY (year, month, day) - 1;
  DOW = (DOW + RTC_BASEDOW) % 7;

  return DOW;	
}

/*************************************************************************
 * Function Name: RTC_SetDate
 * Parameters: LPC_Rtc_Date_t *pDate
 * Return: int
 *             	0: sucess
 *		1: fail
 * Description: Set your specifying date
 *
 *************************************************************************/
int RTC_SetDate (LPC_Rtc_Date_t *pDate)
{
  // Valid Judge
  if (IsValidDay(pDate->year, pDate->month, pDate->day) == false)
    return 1;

  // Calulate DOW, DOY
  pDate->DOY = GetDOY(pDate->year, pDate->month, pDate->day);
  pDate->DOW = GetDOW(pDate->year, pDate->month, pDate->day);

  //PCONP_bit.PCRTC = 1;  //switch on the peripheral clock (pclk) to RTC.
  RTCDOM=pDate->day;
  RTCMONTH=pDate->month;
  RTCYEAR=pDate->year;
  RTCDOW=pDate->DOW;
  RTCDOY=pDate->DOY;
  //PCONP_bit.PCRTC = 0;  //switch off the peripheral clock (pclk) to RTC.
  return 0;
}

/*************************************************************************
 * Function Name: RTC_SetAlarmDateTime
 * Parameters: LPC_Rtc_DateTime_t *pDateTime
 * Return: int
 *             	0: sucess
 *		1: fail
 * Description: Set your specifying alarm date and time
 *
 *************************************************************************/
int RTC_SetAlarmDateTime (LPC_Rtc_DateTime_t *pDateTime)
{
  // Valid Judge
  /*if (IsValidDay(pDateTime->year, pDateTime->month, pDateTime->day) == false)
    return 1;

  if ( pDateTime->hour > 23 || pDateTime->minute > 59 ||pDateTime->second > 59)
    return 1;

  // Calulate DOW, DOY
  pDateTime->DOY = GetDOY(pDateTime->year, pDateTime->month, pDateTime->day);
  pDateTime->DOW = GetDOW(pDateTime->year, pDateTime->month, pDateTime->day);
  */
  //PCONP_bit.PCRTC = 1;  //switch on the peripheral clock (pclk) to RTC.   цея відмінив бо споживання від батарейки мені не критичне
  RTCALDOM=pDateTime->day;
  RTCALMON=pDateTime->month;
  RTCALYEAR=pDateTime->year;
  RTCALDOW=pDateTime->DOW;
  RTCALDOY=pDateTime->DOY;

  RTCALHOUR = pDateTime->hour;
  RTCALMIN = pDateTime->minute;
  RTCALSEC = pDateTime->second;
  //PCONP_bit.PCRTC = 0;  //switch off the peripheral clock (pclk) to RTC.
  return 0;
}

/*************************************************************************
 * Function Name: RTC_ClearInt
 * Parameters: unsigned long IntType
 * Return: int
 *             	0: sucess
 *		1: fail
 *
 * Description: Clear RTC interrupt.
 *
 *************************************************************************/
int RTC_ClearInt(unsigned long IntType)
{
  if (IntType<1 || IntType>3)
    return 1;

  //PCONP_bit.PCRTC = 1;  //switch on the peripheral clock (pclk) to RTC.
  RTCILR = (IntType & 0x3);
  //PCONP_bit.PCRTC = 0;  //switch on the peripheral clock (pclk) to RTC.
  return 0;
}

/*************************************************************************
 * Function Name: RTC_CheckIntType
 * Parameters: void
 * Return:  unsigned long
 *	    RTCIncrementInt(1) | RTCAlarmInt(2)
 *
 * Description: Get RTC interrupt Type.
 *
 *************************************************************************/
unsigned long RTC_CheckIntType(void)
{
  return (RTCILR & 0x3);
}

/*************************************************************************
 * Function Name: RTC_ISR
 * Parameters:  void
 * Return: void
 *
 * Description: Rtc interrupt subroutine
 *
 *************************************************************************/
void pack_control();//задекларовано в файлі pack.c

void RTC_ISR (void)
{
int IntStatus;

  IntStatus = RTC_CheckIntType() & 0x3;
  RTC_ClearInt(IntStatus);
	/*
  if (IntStatus & RTCIncrementInt)	// Increment Interrupt
  {

  }

  if (IntStatus & RTCAlarmInt)	        // Alarm Interrupt
  {
    //Alarm();
  }
  VICVectAddr = 0;
  */
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/

