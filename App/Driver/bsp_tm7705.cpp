#include "bsp_tm7705.h"
#include "bsp_spi.h"
#include "bsp_systimer.h"
//���ڵ��Զ����
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_tm7705: "
#define EN_LOG PINK
#endif

#define REG_COMM      0X00	 /* ͨ�żĴ��� */
#define REG_SETUP     0X10	 /* ���üĴ��� */
#define REG_CLOCK     0X20	 /* ʱ�ӼĴ��� */
#define REG_DATA      0X30	 /* ���ݼĴ��� */
#define REG_ZERO_CH1  0X60	 /* CH1 ƫ�ƼĴ��� */
#define REG_FULL_CH1  0X70	 /* CH1 �����̼Ĵ��� */
#define REG_ZERO_CH2  0X61	 /* CH2 ƫ�ƼĴ��� */
#define REG_FULL_CH2  0X71	 /* CH1 �����̼Ĵ��� */

#define WRITE         0X00	 /* д���� */
#define READ          0X08	 /* ������ */

#define CH_0                0		 /* ͨ��0 */
#define CH_1                1		 /* ͨ��1 */
#define CH_2                2		 /* ͨ��2 */
#define CH_3                3		 /* ͨ��3 */

#define CLKDIS_0            0X00	 /* ʱ�����ʹ�� ������Ӿ���ʱ������ʹ�ܲ����񵴣� */
#define CLKDIS_1            0X10	 /* ʱ�ӽ�ֹ �����ⲿ�ṩʱ��ʱ�����ø�λ���Խ�ֹMCK_OUT�������ʱ����ʡ�� */

/*
		2.4576MHz��CLKDIV=0 ����Ϊ 4.9152MHz ��CLKDIV=1 ����CLK Ӧ�� ��0����
		1MHz ��CLKDIV=0 ���� 2MHz   ��CLKDIV=1 ����CLK ��λӦ��  ��1��
	*/
#define CLK_4_9152M         0X08
#define CLK_2_4576M         0X00
#define CLK_1M              0X04
#define CLK_2M              0X0C

#define FS_50HZ             0X00
#define FS_60HZ             0X01
#define FS_250HZ            0X02
#define FS_500HZ            0X04

#define MD_NORMAL		    (0 << 6)	/* ����ģʽ */
#define	MD_CAL_SELF		    (1 << 6)	/* ��У׼ģʽ */
#define	MD_CAL_ZERO		    (2 << 6)	/* У׼0�̶�ģʽ */
#define	MD_CAL_FULL		    (3 << 6)	/* У׼���̶�ģʽ */

#define	GAIN_1			    (0 << 3)	/* ���� */
#define	GAIN_2			    (1 << 3)	/* ���� */
#define	GAIN_4			    (2 << 3)	/* ���� */
#define	GAIN_8			    (3 << 3)	/* ���� */
#define	GAIN_16			    (4 << 3)	/* ���� */
#define	GAIN_32			    (5 << 3)	/* ���� */
#define	GAIN_64			    (6 << 3)	/* ���� */
#define	GAIN_128		    (7 << 3)	/* ���� */

	/* ����˫���Ի��ǵ����Զ����ı��κ������źŵ�״̬����ֻ�ı�������ݵĴ����ת�������ϵ�У׼�� */
#define	BIPOLAR			    (0 << 2)	/* ˫�������� */
#define	UNIPOLAR		    (1 << 2)	/* ���������� */

#define	BUF_NO			    (0 << 1)	/* �����޻��壨�ڲ�������������) */
#define	BUF_EN			    (1 << 1)	/* �����л��� (�����ڲ�������) */

#define	FSYNC_0			     0		    /* �����˲���ͬ�� */
#define	FSYNC_1			     1		    /* �������˲���ͬ�� */

/* ͨ��1��ͨ��2������,���뻺�壬���� */
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
        //Ӳ����λ
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        bsp_delay_nms(10);
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        bsp_delay_nms(10);
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        bsp_delay_nms(10);
    }
    void AD7705_CalibSelf(uint8_t ch)
    {
        if (ch == 0)                                                                                      //ѡ��ͨ��0
        {                                                                                                 /* ��У׼CH1 */
            spi_read_write(REG_SETUP | WRITE | CH_0); /* дͨ�żĴ�������һ����д���üĴ�����ͨ��0 */ //0X10
            spi_read_write(MD_CAL_SELF | CH1_GAIN_BIPOLAR_BUF | FSYNC_0);                             /* ������У׼ */
            bsp_delay_nms(200);                                                                            /* �ȴ��ڲ��������  --- ʱ��ϳ���Լ180ms */
        }
        else if (ch == 1)
        {                                                                     /* ��У׼CH1 */
            spi_read_write(REG_SETUP | WRITE | CH_1);                     /* дͨ�żĴ�������һ����д���üĴ�����ͨ��0 */
            spi_read_write(MD_CAL_SELF | CH2_GAIN_BIPOLAR_BUF | FSYNC_0); /* ������У׼ */
            bsp_delay_nms(200);                                                /* �ȴ��ڲ��������  --- ʱ��ϳ���Լ180ms */
        }
    }
    void Init_AD7705(void)
    {
        /*
		�ڽӿ����ж�ʧ������£������DIN �ߵ�ƽ��д�����������㹻����ʱ�䣨���� 32������ʱ�����ڣ���
		TM7705 ����ص�Ĭ��״̬��
	    */
        /* ����ʱ�ӼĴ��� */
        spi_read_write(REG_CLOCK | WRITE | CH_0); /* ��дͨ�żĴ�������һ����дʱ�ӼĴ��� */ //0X00
        spi_read_write(CLKDIS_0 | CLK_4_9152M | FS_60HZ); /*4.9152MHZ���� ˢ������50Hz */    //0X08

        /* ÿ���ϵ����һ����У׼ */
        AD7705_CalibSelf(0); /* �ڲ���У׼ CH0 */
        bsp_delay_nms(5);  //��ʱ10MS

        /* ����ʱ�ӼĴ��� */
        spi_read_write(REG_CLOCK | WRITE | CH_1); /* ��дͨ�żĴ�������һ����дʱ�ӼĴ��� */ //0X00
        spi_read_write(CLKDIS_0 | CLK_4_9152M | FS_60HZ); /*4.9152MHZ���� ˢ������50Hz */    //0X08

        AD7705_CalibSelf(1); /* �ڲ���У׼ CH0 */
        bsp_delay_nms(5);  //��ʱ10MS
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
        BSP_ADD_TIMER(bsp_tm7705_test, 1000); //����5s�����˯��
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

bool check_tm7705(void)//У��оƬ�Ƿ�����
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