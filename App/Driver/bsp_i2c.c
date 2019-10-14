#include "bsp_i2c.h"
#include "bsp_system.h"
#include "stm32l0xx_hal.h"

#define I2C_GPIO ((i2c_ch_sel == 1)?I2C1_GPIO:(i2c_ch_sel == 3)?I2C3_GPIO:I2C1_GPIO)
#define SDA_Pin ((i2c_ch_sel == 1)?I2C1_SDA_Pin:(i2c_ch_sel == 3)?I2C3_SDA_Pin:I2C1_SDA_Pin)
#define SCL_Pin ((i2c_ch_sel == 1)?I2C1_SCL_Pin:(i2c_ch_sel == 3)?I2C3_SCL_Pin:I2C1_SCL_Pin)

#define I2C1_SDA_H do{HAL_GPIO_WritePin(I2C_GPIO, SDA_Pin, GPIO_PIN_SET);bsp_delay_nus(100);}while(0);
#define I2C1_SDA_L do{HAL_GPIO_WritePin(I2C_GPIO, SDA_Pin, GPIO_PIN_RESET);bsp_delay_nus(100);}while(0);

#define I2C1_SCL_H  do{HAL_GPIO_WritePin(I2C_GPIO, SCL_Pin, GPIO_PIN_SET);bsp_delay_nus(100);}while(0);
#define I2C1_SCL_L	do{HAL_GPIO_WritePin(I2C_GPIO, SCL_Pin, GPIO_PIN_RESET);bsp_delay_nus(100);}while(0);

#define I2C1_SDA_IN() do{i2c_sda_in();}while(0);
#define I2C1_SDA_OUT() do{i2c_sda_out();}while(0);
#define I2C1_SDA_READ (HAL_GPIO_ReadPin(I2C_GPIO,SDA_Pin) == GPIO_PIN_SET)


void I2C_Init(uint8_t i2c_ch);
void I2C_DeInit(uint8_t i2c_ch);
uint8_t I2C_Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data);
uint8_t I2C_Read_One_Byte(uint8_t daddr, uint8_t addr);
void Rtc_Read_n_Byte(uint8_t daddr, uint8_t *dat,uint8_t len);
void Rtc_Wirte_n_Byte(uint8_t daddr, uint8_t *dat,uint8_t len);

static uint8_t i2c_ch_sel = 1;
//I2C 软件模拟驱动 
void i2c_sda_in(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(I2C_GPIO, &GPIO_InitStruct);
}
void i2c_sda_out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(I2C_GPIO, &GPIO_InitStruct);
}

void IIC_Start(void)
{
	I2C1_SDA_OUT(); //sda线输出
	I2C1_SDA_H;
	I2C1_SCL_H;
	I2C1_SDA_L; //START:when CLK is high,DATA change form high to low
	I2C1_SCL_L; //钳住I2C总线，准备发送或接收数据
}
void IIC_Stop(void)
{
	I2C1_SDA_OUT(); //sda线输出
	I2C1_SCL_L;
	I2C1_SDA_L; //STOP:when CLK is high DATA change form low to high
	I2C1_SCL_H;
	I2C1_SDA_H; //发送I2C总线结束信号
}
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	I2C1_SDA_IN(); //SDA设置为输入
	I2C1_SDA_H;
	I2C1_SCL_H;
	while (I2C1_SDA_READ)
	{
		ucErrTime++;
		bsp_delay_nus(100);
		if (ucErrTime > 250)
		{
			IIC_Stop();
			return 1;
		}
	}
	I2C1_SCL_L; //时钟输出0
	return 0;
}
void IIC_Ack(void)
{
	I2C1_SCL_L;
	I2C1_SDA_OUT();
	I2C1_SDA_L;
	I2C1_SCL_H;
	I2C1_SCL_L;
}
void IIC_NAck(void)
{
	I2C1_SCL_L;
	I2C1_SDA_OUT();
	I2C1_SDA_H;
	I2C1_SCL_H;
	I2C1_SCL_L;
}
void IIC_Send_Byte(uint8_t txd)
{
	uint8_t t;
	I2C1_SDA_OUT();
	I2C1_SCL_L; //拉低时钟开始数据传输
	for (t = 0; t < 8; t++)
	{
		if (txd & 0x80)
		{
			I2C1_SDA_H;
		}
		else
		{
			I2C1_SDA_L;
		}
		txd <<= 1;
		I2C1_SCL_H;
		I2C1_SCL_L;
	}
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	I2C1_SDA_IN(); //SDA设置为输入
	for (i = 0; i < 8; i++)
	{
		I2C1_SCL_L;
		I2C1_SCL_H;
		receive <<= 1;
		if (I2C1_SDA_READ)
			receive++;
	}
	if (!ack)
		IIC_NAck(); //发送nACK
	else
		IIC_Ack(); //发送ACK
	return receive;
}
uint8_t I2C_Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data)
{
	IIC_Start();
	IIC_Send_Byte(daddr);
	if (IIC_Wait_Ack())
		return 1;
	IIC_Send_Byte(addr);
	if (IIC_Wait_Ack())
		return 1;
	IIC_Send_Byte(data);
	if (IIC_Wait_Ack())
		return 1;
	IIC_Stop();
	return 0;
}
uint8_t I2C_Read_One_Byte(uint8_t daddr, uint8_t addr)
{
	uint8_t temp = 0;
	IIC_Start();
	IIC_Send_Byte(daddr);
	if (IIC_Wait_Ack())
		return 1;
	IIC_Send_Byte(addr);
	if (IIC_Wait_Ack())
		return 1;
	//IIC_Stop();

	IIC_Start();
	IIC_Send_Byte(daddr + 1);
	if (IIC_Wait_Ack())
		return 1;
	temp = IIC_Read_Byte(0);
	IIC_Stop();

	return temp;
}
void Rtc_Read_n_Byte(uint8_t daddr, uint8_t *dat,uint8_t len)
{
	uint8_t i = 0;
	IIC_Start();
	IIC_Send_Byte(daddr);
	if (IIC_Wait_Ack())
		return ;
	for (i = 0; i < len-1; i++)
	{
		dat[i] = IIC_Read_Byte(1);
	}
	dat[i] = IIC_Read_Byte(0);
	IIC_Stop();
}
void Rtc_Wirte_n_Byte(uint8_t daddr, uint8_t *dat,uint8_t len)
{
	uint8_t i = 0;
	IIC_Start();
	IIC_Send_Byte(daddr);
	if (IIC_Wait_Ack())
		return ;
	for(i = 0;i<len;i++)
	{
		IIC_Send_Byte(dat[i]);
		if (IIC_Wait_Ack())
			return ;
	}
	IIC_Stop();
}
void I2C_Init(uint8_t i2c_ch)
{
    GPIO_InitTypeDef GPIO_InitStruct;
	i2c_ch_sel = i2c_ch;
	if (i2c_ch_sel == 1)
	{
		//cap en pin config
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Pin = CAP_EN_Pin;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(CAP_EN_GPIO, &GPIO_InitStructure);
		HAL_GPIO_WritePin(CAP_EN_GPIO, CAP_EN_Pin, GPIO_PIN_RESET); //disable cap i2c1 en
	}
	/**I2C1 GPIO Configuration
            PB8 ------> I2C1_SCL
            PB9 ------> I2C1_SDA
            */
	GPIO_InitStruct.Pin = SCL_Pin | SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(I2C_GPIO, &GPIO_InitStruct);
}
void I2C_DeInit(uint8_t i2c_ch)
{
    /**I2C1 GPIO Configuration
            PB8 ------> I2C1_SCL
            PB9 ------> I2C1_SDA
            */
	i2c_ch_sel = i2c_ch;
	if (i2c_ch_sel == 1)
	{
		HAL_GPIO_DeInit(CAP_EN_GPIO, CAP_EN_Pin);
	}
    HAL_GPIO_DeInit(I2C_GPIO, SCL_Pin | SDA_Pin);
}