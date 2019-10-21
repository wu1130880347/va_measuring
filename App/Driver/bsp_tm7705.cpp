#include "bsp_tm7705.h"
#include "bsp_spi.h"
#include "bsp_systimer.h"
//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_tm7705: "
#define EN_LOG PINK
#endif

#define REG_COMM      0X00	 /* 通信寄存器 */
#define REG_SETUP     0X10	 /* 设置寄存器 */
#define REG_CLOCK     0X20	 /* 时钟寄存器 */
#define REG_DATA      0X30	 /* 数据寄存器 */
#define REG_ZERO_CH1  0X60	 /* CH1 偏移寄存器 */
#define REG_FULL_CH1  0X70	 /* CH1 满量程寄存器 */
#define REG_ZERO_CH2  0X61	 /* CH2 偏移寄存器 */
#define REG_FULL_CH2  0X71	 /* CH1 满量程寄存器 */

#define WRITE         0X00	 /* 写操作 */
#define READ          0X08	 /* 读操作 */

#define CH_0                0		 /* 通道0 */
#define CH_1                1		 /* 通道1 */
#define CH_2                2		 /* 通道2 */
#define CH_3                3		 /* 通道3 */

#define CLKDIS_0            0X00	 /* 时钟输出使能 （当外接晶振时，必须使能才能振荡） */
#define CLKDIS_1            0X10	 /* 时钟禁止 （当外部提供时钟时，设置该位可以禁止MCK_OUT引脚输出时钟以省电 */

/*
		2.4576MHz（CLKDIV=0 ）或为 4.9152MHz （CLKDIV=1 ），CLK 应置 “0”。
		1MHz （CLKDIV=0 ）或 2MHz   （CLKDIV=1 ），CLK 该位应置  “1”
	*/
#define CLK_4_9152M         0X08
#define CLK_2_4576M         0X00
#define CLK_1M              0X04
#define CLK_2M              0X0C

#define FS_50HZ             0X00
#define FS_60HZ             0X01
#define FS_250HZ            0X02
#define FS_500HZ            0X04

#define MD_NORMAL		    (0 << 6)	/* 正常模式 */
#define	MD_CAL_SELF		    (1 << 6)	/* 自校准模式 */
#define	MD_CAL_ZERO		    (2 << 6)	/* 校准0刻度模式 */
#define	MD_CAL_FULL		    (3 << 6)	/* 校准满刻度模式 */

#define	GAIN_1			    (0 << 3)	/* 增益 */
#define	GAIN_2			    (1 << 3)	/* 增益 */
#define	GAIN_4			    (2 << 3)	/* 增益 */
#define	GAIN_8			    (3 << 3)	/* 增益 */
#define	GAIN_16			    (4 << 3)	/* 增益 */
#define	GAIN_32			    (5 << 3)	/* 增益 */
#define	GAIN_64			    (6 << 3)	/* 增益 */
#define	GAIN_128		    (7 << 3)	/* 增益 */

	/* 无论双极性还是单极性都不改变任何输入信号的状态，它只改变输出数据的代码和转换函数上的校准点 */
#define	BIPOLAR			    (0 << 2)	/* 双极性输入 */
#define	UNIPOLAR		    (1 << 2)	/* 单极性输入 */

#define	BUF_NO			    (0 << 1)	/* 输入无缓冲（内部缓冲器不启用) */
#define	BUF_EN			    (1 << 1)	/* 输入有缓冲 (启用内部缓冲器) */

#define	FSYNC_0			     0		    /* 启用滤波器同步 */
#define	FSYNC_1			     1		    /* 不启用滤波器同步 */

/* 通道1和通道2的增益,输入缓冲，极性 */
#define CH1_GAIN_BIPOLAR_BUF (GAIN_1 | UNIPOLAR | BUF_NO)
#define CH2_GAIN_BIPOLAR_BUF (GAIN_1 | UNIPOLAR | BUF_NO)

uint8_t buf[2] = {0};

extern "C"
{
    uint32_t ch_get_value[20] = {0};
    const uint16_t para_check_ch[20] = 
    {
        1001,
        1001,
        1000,
        101,

        1001,
        1001,
        1000,
        101,

        1001,
        1001,
        1000,
        101,

        1001,
        1001,
        1000,
        101,

        1001,
        1001,
        1000,
        101,
    };
    void reset_spi_ic(void)
    {
        spi_read_write(0xFF);
        spi_read_write(0xFF);
        spi_read_write(0xFF);
        spi_read_write(0xFF);
        spi_read_write(0xFF);
    }
    void reset_ic(void)
    {
        //硬件复位
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        bsp_delay_nms(10);
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        bsp_delay_nms(10);
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        bsp_delay_nms(10);
    }
    void AD7705_CalibSelf(uint8_t ch)
    {
        if (ch == 0)                                                                                      //选择通道0
        {                                                                                                 /* 自校准CH1 */
            spi_read_write(REG_SETUP | WRITE | CH_0); /* 写通信寄存器，下一步是写设置寄存器，通道0 */ //0X10
            spi_read_write(MD_CAL_SELF | CH1_GAIN_BIPOLAR_BUF | FSYNC_0);                             /* 启动自校准 */
            bsp_delay_nms(200);                                                                            /* 等待内部操作完成  --- 时间较长，约180ms */
        }
        else if (ch == 1)
        {                                                                     /* 自校准CH1 */
            spi_read_write(REG_SETUP | WRITE | CH_1);                     /* 写通信寄存器，下一步是写设置寄存器，通道0 */
            spi_read_write(MD_CAL_SELF | CH2_GAIN_BIPOLAR_BUF | FSYNC_0); /* 启动自校准 */
            bsp_delay_nms(200);                                                /* 等待内部操作完成  --- 时间较长，约180ms */
        }
    }
    void Init_AD7705(void)
    {
        /*
		在接口序列丢失的情况下，如果在DIN 高电平的写操作持续了足够长的时间（至少 32个串行时钟周期），
		TM7705 将会回到默认状态。
	    */
        /* 配置时钟寄存器 */
        spi_read_write(REG_CLOCK | WRITE | CH_0); /* 先写通信寄存器，下一步是写时钟寄存器 */ //0X00
        spi_read_write(CLKDIS_0 | CLK_4_9152M | FS_60HZ); /*4.9152MHZ晶振 刷新速率50Hz */    //0X08

        /* 每次上电进行一次自校准 */
        AD7705_CalibSelf(0); /* 内部自校准 CH0 */
        bsp_delay_nms(5);  //延时10MS

        /* 配置时钟寄存器 */
        spi_read_write(REG_CLOCK | WRITE | CH_1); /* 先写通信寄存器，下一步是写时钟寄存器 */ //0X00
        spi_read_write(CLKDIS_0 | CLK_4_9152M | FS_60HZ); /*4.9152MHZ晶振 刷新速率50Hz */    //0X08

        AD7705_CalibSelf(1); /* 内部自校准 CH0 */
        bsp_delay_nms(5);  //延时10MS
    }
    static void bsp_tm7705_test(void)
    {
        for(uint8_t i = 0;i<20;i+=2)
        {
            SPI_CS_ENABLE(i);
            reset_spi_ic();
            spi_read_write(0x38);
            bsp_delay_nms(50);
            buf[0] = spi_read_write(0xff);
            buf[1] = spi_read_write(0xff);
            uint16_t temp = (buf[0] << 8 | buf[1]);
            float val = temp * 3.3 * para_check_ch[i] / 65535;
            ch_get_value[i] = uint32_t(val * 1000);

            reset_spi_ic();
            spi_read_write(0x39);
            bsp_delay_nms(50);
            buf[0] = spi_read_write(0xff);
            buf[1] = spi_read_write(0xff);
            temp = (buf[0] << 8 | buf[1]);
            val = temp * 3.3 * para_check_ch[i+1] / 65535;
            ch_get_value[i+1] = uint32_t(val * 1000);
        }
        BSP_ADD_TIMER(bsp_tm7705_test, 1000); //开机5s后进入睡眠
    }
}
BspTm7705::BspTm7705()
{}
BspTm7705::~BspTm7705()
{}
BspTm7705 *BspTm7705::BspTm7705_registered(void)
{
    static BspTm7705 t_ms_this;
    return &t_ms_this;
}

bool check_tm7705(void)//校验芯片是否正常
{
    uint32_t ret_dat = 0;
    uint8_t *p_dat = 0;
    p_dat = (uint8_t *)&ret_dat;
    reset_spi_ic();
    p_dat[0] = spi_read_write(0x68);
    p_dat[0] = spi_read_write(0xFF);
    p_dat[1] = spi_read_write(0xFF);
    p_dat[2] = spi_read_write(0xFF);
    if(ret_dat == 0x0000401F)
    {
        return true;
    }
    return false;
}
void BspTm7705::init(void)
{
    spi_init();
    init_hc595();
    uint32_t ret = 0;
    reset_ic();
    for (uint8_t i = 0; i < 20; i+=2)
    {
        SPI_CS_ENABLE(i);
        if(check_tm7705() == true)
        {
            reset_spi_ic();
            Init_AD7705();
            ret |= (0x01 << (i/2));
        }
    }
    bsp_delay_nms(300);
    BSP_ADD_TIMER(bsp_tm7705_test, 1000);
}