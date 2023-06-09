#include "MPU6050.h"
#include <math.h>

int16_t Accel_X_RAW = 0;
int16_t Accel_Y_RAW = 0;
int16_t Accel_Z_RAW = 0;
int16_t Gyro_X_RAW = 0;
int16_t Gyro_Y_RAW = 0;
int16_t Gyro_Z_RAW = 0;
double curAccelZ = 0, preAccelZ = 0;
unsigned int thresholdCount = 0, stepCount = 0;

void MPU6050_Init (void)
{
	uint8_t check;
	uint8_t Data;

	// check device ID WHO_AM_I

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR,WHO_AM_I_REG,1, &check, 1, 1000);

	if (check == 104)  // 0x68 will be returned by the sensor if everything goes well
	{
		// cau hinh thanh ghi quan ly nguon PWR_MGMT_1_REG -> wake up
		Data = 0;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1,&Data, 1, 1000);

		// cau hinh toc do du lieu truyen du lieu Data Rate = 1KHz
		Data = 0x07;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, 1000);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g
		Data = 0x00;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);

		// cau hinh gia toc
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> � 250 �/s
		Data = 0x00;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, 1000);
	}

}

double MPU6050_Read_AccelX(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from ACCEL_XOUT_H register

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 2, 1000);

	Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	
	/*** convert the RAW values into acceleration in 'g'
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0.               ****/

	return Accel_X_RAW/16384.0;
}

double MPU6050_Read_AccelY(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from ACCEL_YOUT_H register

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, ACCEL_YOUT_H_REG, 1, Rec_Data, 2, 1000);

	Accel_Y_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
 // chuyen doi du lieu nhi phan 16 bit tu cac thanh ghi.

	return Accel_Y_RAW/16384.0;
}

double MPU6050_Read_AccelZ(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from ACCEL_ZOUT_H register

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, ACCEL_ZOUT_H_REG, 1, Rec_Data, 2, 1000);

	Accel_Z_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	// chuyen doi du lieu nhi phan 16 bit tu cac thanh ghi.
	

	return Accel_Z_RAW/16384.0 * 10;
}

double MPU6050_Read_GyroX(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from GYRO_XOUT_H register

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 2, 1000);

	Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);

	// chuyen doi du lieu nhi phan 16 bit tu cac thanh ghi.

	return Gyro_X_RAW/131.0;
}

double MPU6050_Read_GyroY(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from GYRO_YOUT_H register

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, GYRO_YOUT_H_REG, 1, Rec_Data, 2, 1000);

	Gyro_Y_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);

	/*** convert the RAW values into dps (�/s)
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 131.0
	     for more details check GYRO_CONFIG Register              ****/

	return Gyro_Y_RAW/131.0;
}

double MPU6050_Read_GyroZ(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from GYRO_ZOUT_H register

	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, GYRO_ZOUT_H_REG, 1, Rec_Data, 2, 1000);

	Gyro_Z_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);

	// chuyen doi du lieu nhi phan 16 bit tu cac thanh ghi.

	return Gyro_Z_RAW/131.0;
}

int MPU6050_Counter(void) 
{
	curAccelZ = MPU6050_Read_AccelZ();
	
	if(fabs(curAccelZ - preAccelZ) > 0.6)
		thresholdCount++;
	
	if(thresholdCount > 3) {
		stepCount++;
		thresholdCount = 0;
	}
	
	preAccelZ = curAccelZ;
	
	return stepCount;
}
