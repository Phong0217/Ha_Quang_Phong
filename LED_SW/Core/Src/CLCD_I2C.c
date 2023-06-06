/******************************************************************************************************************

******************************************************************************************************************/
#include "CLCD_I2C.h"

//************************** Low Level Function ****************************************************************//
static void CLCD_Delay(uint16_t Time)
{
	HAL_Delay(Time); // tao do tre
}
// gui du lieu hoac lenh toi toi man hinh LCD qua I2C
static void CLCD_WriteI2C(CLCD_I2C_Name* LCD, uint8_t Data, uint8_t Mode)
{
	/*N?u du?c b?t, th� th�m bit backlight v�o Data_H v� Data_L.
N?u ch? d? l� CLCD_DATA, th�m bit RS (Register Select) v�o Data_H v� Data_L d? ch? d?nh g?i d? li?u.
N?u ch? d? l� CLCD_COMMAND, x�a bit RS trong Data_H v� Data_L d? ch? d?nh g?i l?nh di?u khi?n.
G?n bit EN (Enable) v�o Data_H v� Data_L d? ch? d?nh k�ch ho?t d? li?u.
G?i c�c gi� tr? Data_H, Data_H, Data_L v� Data_L t?i m�n h�nh LCD qua giao ti?p I2C b?ng h�m HAL_I2C_Master_Transmi8*/
	char Data_H;
	char Data_L;
	uint8_t Data_I2C[4];
	Data_H = Data&0xF0;
	Data_L = (Data<<4)&0xF0;
	if(LCD->BACKLIGHT)
	{
		Data_H |= LCD_BACKLIGHT; 
		Data_L |= LCD_BACKLIGHT; 
	}
	if(Mode == CLCD_DATA)
	{
		Data_H |= LCD_RS;
		Data_L |= LCD_RS;
	}
	else if(Mode == CLCD_COMMAND)
	{
		Data_H &= ~LCD_RS;
		Data_L &= ~LCD_RS;
	}
	Data_I2C[0] = Data_H|LCD_EN;
	CLCD_Delay(1);
	Data_I2C[1] = Data_H;
	Data_I2C[2] = Data_L|LCD_EN;
	CLCD_Delay(1);
	Data_I2C[3] = Data_L;
	HAL_I2C_Master_Transmit(LCD->I2C, LCD->ADDRESS, (uint8_t *)Data_I2C, sizeof(Data_I2C), 1000);
}


//************************** High Level Function ****************************************************************//
void CLCD_I2C_Init(CLCD_I2C_Name* LCD, I2C_HandleTypeDef* hi2c_CLCD, uint8_t Address, uint8_t Colums, uint8_t Rows)
{//Kh?i t?o m�n h�nh LCD
	LCD->I2C = hi2c_CLCD;
	LCD->ADDRESS = Address;
	LCD->COLUMS = Colums;
	LCD->ROWS = Rows;
	
	LCD->FUNCTIONSET = LCD_FUNCTIONSET|LCD_4BITMODE|LCD_2LINE|LCD_5x8DOTS;
	LCD->ENTRYMODE = LCD_ENTRYMODESET|LCD_ENTRYLEFT|LCD_ENTRYSHIFTDECREMENT;
	LCD->DISPLAYCTRL = LCD_DISPLAYCONTROL|LCD_DISPLAYON|LCD_CURSOROFF|LCD_BLINKOFF;
	LCD->CURSORSHIFT = LCD_CURSORSHIFT|LCD_CURSORMOVE|LCD_MOVERIGHT;
	LCD->BACKLIGHT = LCD_BACKLIGHT;

	CLCD_Delay(50);
	CLCD_WriteI2C(LCD, 0x33, CLCD_COMMAND);
//	CLCD_Delay(5);
	CLCD_WriteI2C(LCD, 0x33, CLCD_COMMAND);
	CLCD_Delay(5);
	CLCD_WriteI2C(LCD, 0x32, CLCD_COMMAND);
	CLCD_Delay(5);
	CLCD_WriteI2C(LCD, 0x20, CLCD_COMMAND);
	CLCD_Delay(5);
	
	CLCD_WriteI2C(LCD, LCD->ENTRYMODE,CLCD_COMMAND);
	CLCD_WriteI2C(LCD, LCD->DISPLAYCTRL,CLCD_COMMAND);
	CLCD_WriteI2C(LCD, LCD->CURSORSHIFT,CLCD_COMMAND);
	CLCD_WriteI2C(LCD, LCD->FUNCTIONSET,CLCD_COMMAND);
	
	CLCD_WriteI2C(LCD, LCD_CLEARDISPLAY,CLCD_COMMAND);
	CLCD_WriteI2C(LCD, LCD_RETURNHOME,CLCD_COMMAND);
}

void CLCD_I2C_SetCursor(CLCD_I2C_Name* LCD, uint8_t Xpos, uint8_t Ypos)
{
	/*h?i t?o bi?n DRAM_ADDRESS v?i gi� tr? m?c d?nh l� 0x00.
Ki?m tra v� di?u ch?nh gi� tr? Xpos n?u vu?t qu� s? c?t c?a LCD.
Ki?m tra v� di?u ch?nh gi� tr? Ypos n?u vu?t qu� s? h�ng c?a LCD.
D?a v�o gi� tr? c?a Ypos, t�nh to�n gi� tr? DRAM_ADDRESS tuong ?ng v?i v? tr� hi?n th? tr�n DDRAM c?a LCD.
G?i h�m CLCD_WriteI2C d? g?i l?nh d?t v? tr� hi?n th? (LCD_SETDDRAMADDR) k�m theo gi� tr? DRAM_ADDRESS*/
	uint8_t DRAM_ADDRESS = 0x00;
	if(Xpos >= LCD->COLUMS)
	{
		Xpos = LCD->COLUMS - 1;
	}
	if(Ypos >= LCD->ROWS)
	{
		Ypos = LCD->ROWS -1;
	}
	if(Ypos == 0)
	{
		DRAM_ADDRESS = 0x00 + Xpos;
	}
	else if(Ypos == 1)
	{
		DRAM_ADDRESS = 0x40 + Xpos;
	}
	else if(Ypos == 2)
	{
		DRAM_ADDRESS = 0x14 + Xpos;
	}
	else if(Ypos == 3)
	{
		DRAM_ADDRESS = 0x54 + Xpos;
	}
	CLCD_WriteI2C(LCD, LCD_SETDDRAMADDR|DRAM_ADDRESS, CLCD_COMMAND);
}

void CLCD_I2C_WriteChar(CLCD_I2C_Name* LCD, char character)
{
	CLCD_WriteI2C(LCD, character, CLCD_DATA);
}

void CLCD_I2C_WriteString(CLCD_I2C_Name* LCD, char *String)
{
	while(*String)CLCD_I2C_WriteChar(LCD, *String++);
}

void CLCD_I2C_Clear(CLCD_I2C_Name* LCD)
{
	CLCD_WriteI2C(LCD, LCD_CLEARDISPLAY, CLCD_COMMAND);
	CLCD_Delay(5);
}

void CLCD_I2C_ReturnHome(CLCD_I2C_Name* LCD)
{
	CLCD_WriteI2C(LCD, LCD_RETURNHOME, CLCD_COMMAND);
	CLCD_Delay(5);
}

void CLCD_I2C_CursorOn(CLCD_I2C_Name* LCD)
{
	LCD->DISPLAYCTRL |= LCD_CURSORON;
	CLCD_WriteI2C(LCD, LCD->DISPLAYCTRL, CLCD_COMMAND);
}

void CLCD_I2C_CursorOff(CLCD_I2C_Name* LCD)
{
	LCD->DISPLAYCTRL &= ~LCD_CURSORON;
	CLCD_WriteI2C(LCD, LCD->DISPLAYCTRL, CLCD_COMMAND);
}

void CLCD_I2C_BlinkOn(CLCD_I2C_Name* LCD)
{
	LCD->DISPLAYCTRL |= LCD_BLINKON;
	CLCD_WriteI2C(LCD, LCD->DISPLAYCTRL, CLCD_COMMAND);
}

void CLCD_I2C_BlinkOff(CLCD_I2C_Name* LCD)
{
	LCD->DISPLAYCTRL &= ~LCD_BLINKON;
	CLCD_WriteI2C(LCD, LCD->DISPLAYCTRL, CLCD_COMMAND);
}
