#ifndef __RC522REG_H
#define	__RC522REG_H

//============== MF RC522�Ĵ������� ===============================================================
// ҳ�㣻�����״̬�Ĵ���
#define     JREG_RFU00           	0x00		// ����
#define     JREG_COMMAND         	0x01		// ����Ĵ���
#define     JREG_COMMIEN         	0x02		// �ж�ʹ�ܼĴ��� IRQ��TX��RX��IDLE��HI��LO��ERR��TIMER
#define     JREG_DIVIEN          	0x03		// �ж�ʹ�ܼĴ��� IRQ��SIGIN��MODE��CRC��RFON��RFOFF
#define     JREG_COMMIRQ         	0x04		// �жϱ�־
#define     JREG_DIVIRQ          	0x05		// �жϱ�־
#define     JREG_ERROR           	0x06        // ����״̬�Ĵ���
#define     JREG_STATUS1         	0x07		// ״̬�Ĵ���1
#define     JREG_STATUS2         	0x08		// ״̬�Ĵ���2
#define     JREG_FIFODATA        	0x09		// FIFO
#define     JREG_FIFOLEVEL       	0x0A		// FIFO�洢�ֽ����Ĵ���
#define     JREG_WATERLEVEL      	0x0B		// FIFO���硢���籨�����
#define     JREG_CONTROL         	0x0C		// ���ƼĴ�������ʱ�����ƣ����յ�λ��
#define     JREG_BITFRAMING      	0x0D		// Bit֡���ƼĴ���
#define     JREG_COLL            	0x0E		// ��ͻλ�Ĵ���
#define     JREG_RFU0F           	0x0F        // ����
// ҳһ��ͨ�żĴ���
#define     JREG_RFU10           	0x10		// ����
#define     JREG_MODE            	0x11		// ͨ��ģʽ�Ĵ���
#define     JREG_TXMODE          	0x12		// ���Ϳ��ƼĴ�����CRC�����ʿ��ƣ�
#define     JREG_RXMODE          	0x13 		// ���տ��ƼĴ�����CRC�����ʿ��ƣ�
#define     JREG_TXCONTROL       	0x14		// TX������ƼĴ���
#define     JREG_TXASK           	0x15		// ������ASK����ϵ�����ƼĴ���
#define     JREG_TXSEL           	0x16		// ���Ϳ��ƼĴ������ź�����/���ѡ��
#define     JREG_RXSEL           	0x17		// ���տ��ƼĴ������ź�����ѡ��
#define     JREG_RXTHRESHOLD     	0x18		// ���տ��ƼĴ�������������ֵ���ã�
#define     JREG_DEMOD           	0x19  		// ���տ��ƼĴ�������������ã�
#define     JREG_FELNFC1           	0x1A		// ���� FelCa ͬ���ֽ�
#define     JREG_FELNFC2           	0x1B		// ������յ�������FelCa��
#define     JREG_MIFNFC          	0x1C		// ����ISO14443A/Mifare/NFC�Ĺ���ģʽ
#define     JREG_MANUALRCV          0x1D		// ������΢���Ĵ���
#define     JREG_TYPEB           	0x1E		// TypeB
#define     JREG_SERIALSPEED     	0x1F		// ����UART���ʿ��ƼĴ���
// ҳ�������üĴ���
#define     JREG_RFU20           	0x20 		// ����
#define     JREG_CRCRESULT1      	0x21		// CRC�������Ĵ�����MSB��
#define     JREG_CRCRESULT2      	0x22		// CRC�������Ĵ�����LSB��
#define     JREG_GSNOFF          	0x23		// ����
#define     JREG_MODWIDTH        	0x24		// ���ƿ�ȿ��ƼĴ���
#define     JREG_TXBITPHASE      	0x25		// ����
#define     JREG_RFCFG           	0x26		// ������������ƼĴ���
#define     JREG_GSN             	0x27		// ������������ŵ絼���ƼĴ�����N,���ƹ��ʣ�
#define     JREG_CWGSP           	0x28		// ������������ŵ絼���ƼĴ�����P,���ƹ��ʣ�
#define     JREG_MODGSP          	0x29		// ������������ŵ絼���ƼĴ��������ڵ���ϵ����
#define     JREG_TMODE           	0x2A		// ��ʱ�����üĴ���
#define     JREG_TPRESCALER      	0x2B		// ��ʱ����Ƶϵ���Ĵ���
#define     JREG_TRELOADHI       	0x2C		// 16λ��ʱ����װֵ��Hi��
#define     JREG_TRELOADLO       	0x2D		// 16λ��ʱ����װֵ��Lo��
#define     JREG_TCOUNTERVALHI   	0x2E		// 16λ��ʱ����ֵ��Hi��
#define     JREG_TCOUNTERVALLO   	0x2F		// 16λ��ʱ����ֵ��Lo��
// ҳ�������ԼĴ���
#define     JREG_RFU30           	0x30		// ����
#define     JREG_TESTSEL1        	0x31		// ���ԼĴ���
#define     JREG_TESTSEL2        	0x32		// ���ԼĴ���
#define     JREG_TESTPINEN       	0x33		// ���ԼĴ���
#define     JREG_TESTPINVALUE    	0x34		// ���ԼĴ���
#define     JREG_TESTBUS         	0x35		// �ڲ���������״̬
#define     JREG_AUTOTEST        	0x36		// �Բ��Կ��ƼĴ���
#define     JREG_VERSION         	0x37		// �汾
#define     JREG_ANALOGTEST      	0x38		// AUX�ܽſ��ƼĴ���
#define     JREG_TESTDAC1        	0x39		// DAC1����ֵ
#define     JREG_TESTDAC2        	0x3A		// DAC1����ֵ
#define     JREG_TESTADC         	0x3B		// ADC_I��ADC_Q��ֵ
#define     JREG_ANALOGUETEST1   	0x3C		// ���ԼĴ���
#define     JREG_RFT3D           	0x3D		// ���ԼĴ���
#define     JREG_RFT3E           	0x3E		// ���ԼĴ���
#define     JREG_RFT3F           	0x3F		// ���ԼĴ���

//============== PN512 �����붨�� ===============================================================
#define     JCMD_IDLE            	0x00	    // ��������
#define 	JCMD_CONFIG		    	0x01		// �������PN512��
#define 	JCMD_MEM		    	0x01		// �洢25�ֽڵ��ڲ���������RC52x��
#define 	JCMD_GENRANID    		0x02		// ��ȡ10�ֽڵ������
#define     JCMD_CALCCRC         	0x03		// ����CRC
#define     JCMD_TRANSMIT        	0x04		// �������� 
#define     JCMD_NOCMDCHANGE     	0x07		// �޸�����Ĵ���������λ�����ı䵱ǰ���
#define     JCMD_RECEIVE         	0x08		// ���������
#define     JCMD_TRANSCEIVE			0x0C		// ����FIFO�е����ݵ����ߣ�����󼤻���յ�·
#define 	JCMD_AUTOCOLL	    	0x0D
#define     JCMD_AUTHENT         	0x0E		// MIFARE��֤
#define     JCMD_SOFTRESET       	0x0F		// �����λ


//============= λ���� ============================================================================
// JREG_COMMAND                     0x01        // ����Ĵ���           	
#define     JBIT_RCVOFF          	0x20        // ģ�ⲿ�ֿ�/�ؿ���λ
#define     JBIT_POWERDOWN       	0x10        // PN511��Դ����(�������)

// JREG_COMMIEN                     0x02        // �ж�ʹ�ܼĴ���       
#define     JBIT_IRQINV          	0x80        // �����жϽ����ģʽ

// JREG_DIVIEN                      0x03        // �ж�ʹ�ܼĴ���       
#define     JBIT_IRQPUSHPULL     	0x80        // �����жϽ����ģʽ

// JREG_COMMIEN/JREG_COMMIRQ         0x02, 0x04	// �ж�ʹ��/����        
#define     JBIT_TXI             	0x40        // �����ж�ʹ��/����
#define     JBIT_RXI             	0x20        // �����ж�ʹ��/����
#define     JBIT_IDLEI           	0x10        // �����ж�ʹ��/����
#define     JBIT_HIALERTI        	0x08        // FIFO���ж�ʹ��/����
#define     JBIT_LOALERTI        	0x04        // FIFO���ж�ʹ��/����
#define     JBIT_ERRI            	0x02        // �����ж�ʹ��/����
#define     JBIT_TIMERI          	0x01        // ��ʱ���ж�ʹ��/����

// JREG_DIVIEN/JREG_DIVIRQ           0x03, 0x05	// �ж�ʹ��/����        
#define     JBIT_SIGINACTI       	0x10        // Sigin�ź��ж�ʹ��/����
#define     JBIT_MODE            	0x08        // ģʽ�ж�ʹ��/����PN512��
#define     JBIT_CRCI            	0x04        // CRC�ж�ʹ��/����
#define     JBIT_RFON            	0x02        // RFON�ж�ʹ��/����PN512��
#define     JBIT_RFOFF           	0x01        // RFOFF�ж�ʹ��/����PN512��

// JREG_COMMIRQ/JREG_DIVIRQ         0x04, 0x05  // �ж�����             
#define     JBIT_SET             	0x80        // ��������жϱ�־λ

// Error Register 					0x06		// ����Ĵ���
#define     JBIT_WRERR           	0x80		// дFIFO��ͻ����
#define     JBIT_TEMPERR         	0x40		// оƬ�¶ȹ���
#define     JBIT_BUFFEROVFL      	0x10		// FIFO�������
#define     JBIT_COLLERR         	0x08		// ��ͻλ����
#define     JBIT_CRCERR          	0x04		// CRCУ�����
#define     JBIT_PARITYERR       	0x02		// ��żУ�����
#define     JBIT_PROTERR         	0x01		// Э�����

// JREG_STATUS1                     0x07		// ״̬�Ĵ���1      		
#define     JBIT_RFFREQOK           0x80        // ָʾRF��Ƶ�ʣ����ⲿRF����Ƶ����12MHz~15MHz,
                                                // ��λ��1��������0
#define     JBIT_CRCOK              0x40        // CRC������ָʾ����CRC����Ϊ0����λ��λ
#define     JBIT_CRCREADY           0x20        // CRC������ϣ���λ��λ
#define     JBIT_IRQ                0x10        // �ж������������жϣ���λ��1
#define     JBIT_TRUNNUNG           0x08        //
#define     JBIT_EXRFON             0x04        // �ⲿRF��״̬�Ĵ���        
#define     JBIT_HIALERT            0x02        // FIFO��
#define     JBIT_LOALERT            0x01        // FIFO��

// Status 2 Register				0x08)
#define     JBIT_TEMPSENSOFF     	0x80		// �¶ȴ���������
#define     JBIT_I2CFORCEHS      	0x40		// IIC����ģʽ
#define 	JBIT_TARGETACTIVATED	0x10
#define     JBIT_CRYPTO1ON       	0x08		// Crypto ����ģʽ

// FIFOLevel Register				0X0A
#define     JBIT_FLUSHBUFFER     	0x80		// 1 ���FIFO

// ControlReg						0x0C		// ������ͬ�Ŀ���λ
#define     JBIT_TSTOPNOW        	0x80		// 1 ��ʱ������ֹͣ
#define     JBIT_TSTARTNOW       	0x40		// 1 ��ʱ����������
#define		JBIT_WRNFCIDTOFIFO		0x20		// 1 ����10�ֽ�NFCID	PN51x
#define		JBIT_INITIATOR			0X10		// 1 ��ģʽ				PN51x

// BitFramiReg						0x0D		// ����λ֡
#define     JBIT_STARTSEND       	0x80		// 1 �������ݷ���

// CollReg							0x0E		// RF�ӿڼ�⵽�ĳ�ͻλ
#define     JBIT_VALUESAFTERCOLL 	0x80		// 0 ��ͻλ��������յ����ݽ������

// Page1
// ModeReg							0x11		// ���巢�ͺͽ���ģʽ������CRC��ֵ���壩
#define     JBIT_MSBFIRST        	0x80		// CRC��λ��ǰ RC523 
#define     JBIT_TXWAITRF        	0x20   
#define     JBIT_POLSIGIN        	0x08		// SIGIN ����


// TxModeReg						0x12		// ���巢���������ʺ�ģʽ
#define     JBIT_INVMOD          	0x08		// 1 �������ݵ��Ʒ���

// RxModeReg						0x13		// ��������������ʺ�ģʽ
#define     JBIT_RXNOERR         	0x08		// 1 ���յ�λ����4λ����������

// ���� TxModeReg��RxModeReg���е�
#define     JBIT_106KBPS         	0x00
#define     JBIT_212KBPS         	0x10
#define     JBIT_424KBPS         	0x20
#define     JBIT_848KBPS         	0x30		// RC523

#define     JBIT_CRCEN           	0x80		// CRCʹ��
#define     JBIT_ISO14443A       	0x00		// ���ݴ���ģʽTypeA	RC523
#define     JBIT_ISO14443B       	0x03		// ���ݴ���ģʽTypeB	RC523

// TxControlReg						0x14		// ���������ſ����߼�
#define     JBIT_INVTX2ON        	0x80		// 1 ��TX2ʹ�ܣ���TX2�ź��������
#define     JBIT_INVTX1ON        	0x40		// 1 ��TX1ʹ�ܣ���TX1�ź��������
#define     JBIT_INVTX2OFF       	0x20 		// 1 ��TX2��ֹ����TX2�ź��������  
#define     JBIT_INVTX1OFF       	0x10  		// 1 ��TX1��ֹ����TX1�ź�������� 
#define     JBIT_TX2CW           	0x08		// 1 TX2����δ���Ƶ��ز��źţ�
#define     JBIT_TX2RFEN         	0x02		// 1 TX2������Ƶ��ز��ź�
#define     JBIT_TX1RFEN         	0x01		// 1 TX1������Ƶ��ز��ź�

// JREG_TXAUTO                                  // ���ƺ��������������� 0x15		
#define     JBIT_AUTORFOFF          0x80        // RF���Զ��ر�ģʽ(NFCIP-1ģʽ)
#define     JBIT_FORCE100ASK        0x40        // 100%ASK����ģʽ����(������ModGsPReg)
#define     JBIT_AUTOWAKEUP         0x20        // �������ģʽ���Զ�����
#define     JBIT_CAON               0x08        // 
#define     JBIT_INITIALRFON        0x04        // 
#define     JBIT_TX2RFAUTOEN        0x02        // TX2���ⲿRF���رպ��Զ���
#define     JBIT_TX1RFAUTOEN        0x01        // TX1���ⲿRF���رպ��Զ���

// DemodReg							0x19		// ���������
#define     JBIT_FIXIQ           	0x20		// �����ͨ��ѡ��

// TModeReg							0x2A		// ��ʱ��ģʽ����
#define     JBIT_TAUTO           	0x80		// 1 ��ʱ���Զ�������ֹͣ
#define     JBIT_TAUTORESTART    	0x10		// 1 ��ʱ�����¼�����
												// 0 ��������0���������ж��ź�


//============= λ������ ========================================================================
// Command register             	0x01
#define     JMASK_COMMAND        	0x0F		// JREG_COMMAND ����������

// Waterlevel register          	0x0B
#define     JMASK_WATERLEVEL		0x3F		// JREG_WATERLEVEL FIFOˮƽ��������

// Control register             	0x0C
#define     JMASK_RXBITS         	0x07		// JREG_CONTROL �����յ����ֽڵ���Чλ����Ŀ������

// Mode register                	0x11
#define     JMASK_CRCPRESET      	0x03		// JREG_MODE CRCԤ��ֵ��ѡ��������

// TxMode register              	0x12, 0x13
#define     JMASK_SPEED          	0x70		// JREG_RXMODE ����/��������λ����������

// TxSel register               	0x16
#define     JMASK_DRIVERSEL      	0x30		// JREG_TXSEL TX1��TX2���ѡ��������
#define     JMASK_SIGOUTSEL      	0x0F		// JREG_TXSEL MFOUT�ź�Դѡ��������

// RxSel register               	0x17
#define     JMASK_UARTSEL        	0xC0		// JREG_RXSEL �ǽӴ�ʽUART�����ź�ѡ��������
#define     JMASK_RXWAIT         	0x3F		// JREG_RXSEL ���յȴ�ʱ������������

// RxThreshold register         	0x18
#define     JMASK_MINLEVEL       	0xF0		// JREG_RXTHRESHOLD ��������������������С�ź�ǿ��
#define     JMASK_COLLEVEL       	0x07		// JREG_RXTHRESHOLD �����ͻλ�źŵ�ǿ��������

// Demod register               	0x19
#define     JMASK_ADDIQ          	0xC0		// JREG_DEMOD ���չ�����I/Qͨ����ѡ��������
#define     JMASK_TAURCV         	0x0C		// JREG_DEMOD ���չ������ڲ�PLL��ʱ�䳣��������
#define     JMASK_TAUSYNC        	0x03		// JREG_DEMOD ͻ�������У��ڲ�PLL��ʱ�䳣��������

// RFCfg register               	0x26
#define     JMASK_RXGAIN         	0x70		// JREG_RFCFG �����źŵ�ѹ��������������

// GsN register                 	0x27
#define     JMASK_CWGSN          	0xF0		// JREG_GSN N�������ĵ絼������
#define     JMASK_MODGSN         	0x0F		// JREG_GSN N�������ĵ���ϵ��������

// CWGsP register               	0x28
#define     JMASK_CWGSP          	0x3F		// JREG_CWGSP P�������ĵ絼������

// ModGsP register              	0x29
#define     JMASK_MODGSP         	0x3F		// JREG_MODGSP P�������ĵ���ϵ��������

// TMode register               	0x2A
#define     JMASK_TGATED         	0x60		// JREG_TMODE ��ʱ���Ź���ģʽ������
#define     JMASK_TPRESCALER_HI  	0x0F		// JREG_TMODE ��ʱ�����ֽ�������




#endif
