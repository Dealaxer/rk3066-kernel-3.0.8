#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <asm/types.h>
#include <mach/gpio.h>
#include <mach/iomux.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <linux/wait.h>

#if 0
#define DBG(x...)	printk(KERN_INFO x)
#else
#define DBG(x...)
#endif

#define TRUE 1
#define FALSE 0


//////////////////////////////////////////////////////////////////////////////////////////
#define ASC_AUTHENTICATION		10
#define ASC_READ_USERDATA		11
#define ASC_WRITE_USERDATA		21
#define ASC_CHANGE_PWD			22
#define ASC_READ_PWD			72
#define ASC_READ_CFG			73

#define ASC_SDA_IO  RK30_PIN0_PC7
#define ASC_SCL_IO  RK30_PIN0_PC6

#define SDA_HIGH	gpio_direction_output(ASC_SDA_IO, GPIO_HIGH)
#define SDA_LOW		gpio_direction_output(ASC_SDA_IO, GPIO_LOW)
#define SCL_HIGH	gpio_direction_output(ASC_SCL_IO, GPIO_HIGH)
#define SCL_LOW		gpio_direction_output(ASC_SCL_IO, GPIO_LOW)
#define SDA_IN		gpio_direction_input(ASC_SDA_IO)
#define SDA			gpio_get_value(ASC_SDA_IO)


struct ASC_DATA
{
	unsigned char  zone;//ָ����д�û�����(0-3)
	unsigned char  gc_index;//��������ʹ������GC
	unsigned char  gc[8];//ָ��GC��ֵ
	unsigned char  pw_index;//��������ʹ������PWD
	unsigned char  pw[3];//ָ����PWDֵ
	unsigned char  data[32];//ÿ�������д���û�����:32�ֽ�
};

unsigned char GPA[20];      //GPA ��Ԫʹ��Ƶ�ʸߣ�Ӧֱ��Ѱַ
unsigned char rwdata[0x14];
unsigned char reboot = 0;
//******************����дGC************************************
//GC_TABLE:
unsigned char GC[8] = {0};
unsigned char const  GC0[]={0x34,0x56,0x69,0x62,0x75,0xd6,0x4c,0x29};//GC0
unsigned char const  GC1[]={0x88,0x69,0xc5,0x1d,0xa8,0x99,0xc8,0x46};//GC1
unsigned char const  GC2[]={0x1a,0x07,0x29,0x9d,0x22,0x62,0x49,0x58};//GC2
unsigned char const  GC3[]={0x81,0x9d,0xcf,0x4c,0xb7,0x60,0x0d,0xe4};//GC3

//******************����дPASSWORD************************************
//PASSWORD_TABLE:
unsigned char PW[3] = {0};
unsigned char const  PW_WRITE0[]={0xc7,0x09,0x64};//WRITE PASSWORD 0
unsigned char const  PW_READ0[]= {0x2d,0x81,0x01};//READ  PASSWORD 0
unsigned char const  PW_WRITE1[]={0x4a,0x18,0xab};//WRITE PASSWORD 1
unsigned char const  PW_READ1[]= {0xf6,0x3c,0x4f};//READ  PASSWORD 1
unsigned char const  PW_WRITE2[]={0x6b,0xd1,0x57};//WRITE PASSWORD 2
unsigned char const  PW_READ2[]= {0x2d,0x2a,0xeb};//READ  PASSWORD 2
unsigned char const  PW_WRITE3[]={0x40,0x28,0xd2};//WRITE PASSWORD 3
unsigned char const  PW_READ3[]= {0x97,0x24,0x7e};//READ  PASSWORD 3
unsigned char const  PW_WRITE4[]={0xb3,0x0f,0xbb};//WRITE PASSWORD 4
unsigned char const  PW_READ4[]= {0x01,0x29,0x8b};//READ  PASSWORD 4
unsigned char const  PW_WRITE5[]={0xd7,0x4f,0xfd};//WRITE PASSWORD 5
unsigned char const  PW_READ5[]= {0x75,0x7f,0xbd};//READ  PASSWORD 5
unsigned char const  PW_WRITE6[]={0xe5,0x58,0x42};//WRITE PASSWORD 6
unsigned char const  PW_READ6[]= {0x32,0x69,0x91};//READ  PASSWORD 6
unsigned char const  PW_WRITE7[]={0x3a,0x16,0x38};//WRITE PASSWORD 7
unsigned char const  PW_READ7[]= {0x18,0x64,0xfc};//READ  PASSWORD 7

void SMSTART(void);//Function:Start I2C
void SMSTOP(void); //Function:Stop  I2C
void IIC_Initial(void);//Function:Initial IIC
void delay(int us);

bool verify_auth(unsigned char GCx,unsigned char PWx);

void Wait_For_AckPolling(void);
                   //����:��������͵������Ƿ��ѱ�ִ��
                   //ֻ�е�AckPolling_return=1��bytecounter=0ʱ���ó�����˳�

void GPA_CLOCK(unsigned char Datain,unsigned char times);
                   //GPA����
                   //���:Data_in
                   //����:GPA[0]:GPA_byte
                   //����:times  GPA��������Ĵ���

void read(unsigned char* lpData);
                   //����:��at88scxx I2C����������
                   //����������read(rwdata)����;
                   //���:rwdata����
                   //����:rwdata����
                   //rd���鳤�ȿɸ�
                   //cmd_send_counter�����������,����8��,�������˳�,����ΪI2C��·��������

void write(unsigned char* lpData);
                   //����:��at88scxx I2Cд��������
                   //����������write(rwdata)����;
                   //���:rwdata����
                   //����:rwdata����
                   //rd���鳤�ȿɸ�
                   //cmd_send_counter�����������,����8��,�������˳�,����ΪI2C��·��������

unsigned char AUTHENTICATION(unsigned char GC_select);
                  //˫����֤����
                  //���:GC_select��ѡ����Կ����
                  //����:AAC��      ��֤���
                  //���̣�
                  //1.����������CIx,���������Q0,������Կ;�����Q1,��Q0��Q1���͸�����,������µ�CIx,SKx
                  //2.�ö��ص�CIx��������CIx�Ƚ�
                  //3.�ü������SKx���GCx,�ظ�1����
                  //AAC:��֤���������.AAC!=0xff��ʾ˫����֤δͨ����������
                  //������������Ӧ�ڳ������޸�


unsigned char verify_write_password(unsigned char pw_select);
                  //У��д����������������ȷУ����д����󿪷Ŷ�д
                  //pw_select����������ѡ��
                  //PAC:����У����������.PAC!=0xff��ʾ��֤δͨ����������

void select_user_zone(unsigned char zone);
                  //ѡ���û���
                  //zone���û������

void read_user_zone(unsigned char rd_high_addr,unsigned char rd_low_addr,unsigned char rd_number);
                 //�����û����������ݣ��������������
                 //rd_high_addr �û������ֽڵ�ַ
                 //rd_low_addr  �û������ֽڵ�ַ
                 //rd_number    ��ȡ�������ݼ�����������ݵĸ���
                 //���ܳ����������ݴ����rwdata[0x04]=>rwdata[0x13]

void read_config_zone(unsigned char rd_high_addr,unsigned char rd_low_addr,unsigned char rd_number);
                 //��rd_low_addr����Ӧ������0xb0
                 //����� 0xb0<rd_low_addr<0xef ��Ԫ��Ӧ���µ���

void encrypto_data(unsigned char encryptodatanumber);
                 //���������ݽ��м���
                 //encryptodatanumber�������ݵĸ���
                 //����ܵ��������ݴ���� rwdata[4]��rwdata[0x13]

void write_user_zone(unsigned char wr_high_addr,unsigned char wr_low_addr,unsigned char wr_number);
                 //����:���Ѽ����˵���������д������
                 //wr_high_addr �û������ֽڵ�ַ
                 //wr_low_addr  �û������ֽڵ�ַ
                 //wr_number    д�����ݵĸ���
                 //������� rwdata[4]��rwdata[0x13]��������д������

void send_checksum(void);
                 //����:����У��͵�����(��������ʽд�����ݺ�,�跢��У���)

void write_config_zone(unsigned char wr_high_addr,unsigned char wr_low_addr,unsigned char wr_number);
                 //��rd_low_addr����Ӧ������0xb0
                 //����� 0xb0<rd_low_addr<0xef ��Ԫ��Ӧ���µ���
                 //��˿�Ϻ󲻿�д

//****************���Ķ�д����******************************
unsigned char verify_sc_plaintext(unsigned char sc_first_byte,unsigned char sc_second_byte,unsigned char sc_third_byte);
                 //����:У�鰲ȫ���루�������룩,������λ����write7_password
                 //��������˿δ��ǰ,У�鰲ȫ����ɹ���,���Զ�����������,�û����������д
                 //sc_firsr_byte  ����ĵ�1���ֽ�����
                 //sc_second_byte ����ĵ�2���ֽ�����
                 //sc_third_byte  ����ĵ�3���ֽ�����
                 //��verify_sc_plaintext(0xDD,0x42,0x97)
                 //0xDD,0x42,0x97�ֱ�Ϊ����ĵ�123���ֽ�����

void set_user_zone_plaintext(unsigned char zonep);
                 //ѡ���û��������ģ�
                 //zonep:�û������

void read_paintext(unsigned char rd_cmd,unsigned char A1,unsigned char A2,unsigned char N);
                 //������(����)
                 //rd_cmd:0xb2 ���û��� /0xb6 ��������  /0xb6 0x01 0x00 0x01 ����˿
                 //A1    :���ֽڵ�ַ
                 //A2    :���ֽڵ�ַ
                 //N     :��ȡ���ݸ���

void write_paintext(unsigned char wr_cmd,unsigned char A1,unsigned char A2,unsigned char N);
                 //д����(����)
                 //rd_cmd:0xb0 д�û��� /0xb4 д������  /0xb4 0x01 ID 0x00  д��˿
                 //A1    :���ֽڵ�ַ
                 //A2    :���ֽڵ�ַ
                 //N     :д���ݸ���
                 //ע�⣺�ն���˿ʱֻ�ܴ�ID=0x06=>0x04=>0x00

//************************AckPolling*******************************
//����:��������͵������Ƿ��ѱ�ִ��
//ֻ�е�AckPolling_return=1��bytecounter=0ʱ���ó�����˳�
//*****************************************************************
void Wait_For_AckPolling(void)
{
	unsigned char  AckPolling_return;
	unsigned char  temp, bitcounter;
	unsigned char  bytecounter;

	temp=0xb6;
	bytecounter=100;//note:if CPU clk fast,inc bytecounter.
	AckPolling_return=0;
	SMSTART();

	do{
		for(bitcounter=0;bitcounter<8;bitcounter++)
		{
			if ((temp&0x80)==0x80)
			{
				SDA_HIGH;
			}
			else
			{
				SDA_LOW;
			}

			SCL_LOW;delay(1); //SCL���� _|~~|_ �� _|~~|_
			SCL_HIGH;delay(2);//SDA����_|~~~~|_�� ______(�߻���������)
			SCL_LOW;delay(1);
			temp=temp<<1;
		}

		SDA_IN;
		SCL_LOW;delay(1);
		SCL_HIGH;delay(1);
		bytecounter--;

		if(SDA!=GPIO_HIGH)
		{
			delay(1);
			AckPolling_return=1;
			SCL_LOW;delay(1);
		}
		else
		{
			delay(1);
			printk("Wait_For_AckPolling retry!\n");
			SCL_LOW;delay(1);
			AckPolling_return=0;
			temp=0xb6;
			SMSTART();
		}

		if(AckPolling_return==1)
		{
			break;
		}

	}while(bytecounter>0);

	SMSTOP();
}

//***********************IIC_Initial***********************************
//Initial IIC
//*****************************************************************
void IIC_Initial()
{
	gpio_direction_output(ASC_SCL_IO, GPIO_HIGH);
	gpio_direction_output(ASC_SDA_IO, GPIO_HIGH);
	SDA_HIGH;delay(2);
	SCL_HIGH;delay(2);
	SCL_LOW; delay(2);
	SCL_HIGH;delay(2);
	SCL_LOW; delay(2);
	SCL_HIGH;delay(2);
	SCL_LOW; delay(2);
	SCL_HIGH;delay(2);
	SCL_LOW; delay(2);
	SCL_HIGH;delay(2);
	SCL_LOW; delay(2);
	SCL_HIGH;delay(2);
	SCL_LOW; delay(2);
	SCL_HIGH;delay(2);//�ȴ��5��CLK
}

void delay(int us)
{
	udelay(us*10);
}

void SMSTART()
{
	SDA_HIGH;
	SCL_HIGH;
	delay(4);
	SDA_LOW;delay(1);
	SCL_LOW;delay(1);
}

void SMSTOP()
{
	SDA_LOW;
	SCL_LOW;delay(1);
	SCL_HIGH;delay(1);
	SDA_HIGH;delay(10);
}

void write(unsigned char* lpData)
{
	unsigned char bitcounter;
	unsigned char temp;
	unsigned char bytecounter;
	unsigned char cmd_send_counter;
	temp=*lpData;
	bytecounter=0;
	cmd_send_counter=0;
	SMSTART();

	do{
		for(bitcounter=0;bitcounter<8;bitcounter++)
		{
			if ((temp&0x80)==0x80)
			{
				SDA_HIGH;
			}
			else
			{
				SDA_LOW;
			}

			SCL_LOW;delay(1); //SCL���� _|~~|_ �� _|~~|_
			SCL_HIGH;delay(2);//SDA����_|~~~~|_�� ______(�߻���������)
			SCL_LOW;delay(1);
			temp=temp<<1;
		}

		SDA_IN;delay(1);
		SCL_LOW;delay(1);
		SCL_HIGH;delay(1);

		if(SDA!=GPIO_HIGH)
		{
			delay(1);
			bytecounter++;
			temp=*(lpData+bytecounter);
			SCL_LOW;delay(1);
		}
		else
		{
			delay(1);
			printk("write retry!\r\n");
			temp=*lpData;
			bytecounter=0;
			cmd_send_counter++;
			SCL_LOW;delay(1);
			SMSTART();
		}

		if(cmd_send_counter>8)
		{
		    printk("cmd_send_counter: %d\r\n", cmd_send_counter);
			bytecounter=*(lpData+3)+4;
		}
	}while(bytecounter< *(lpData+3)+4);

	SMSTOP();

}

void read(unsigned char* lpData)
{
	unsigned char bitcounter;
	unsigned char temp;
	unsigned char cmd_send_counter;
	unsigned char bytecounter;
	temp=*lpData;
	bytecounter=0;
	cmd_send_counter=0;
	SMSTART();

	do{
		for(bitcounter=0;bitcounter<8;bitcounter++)
		{
			if ((temp&0x80)==0x80)
			{
				SDA_HIGH;
			}
			else
			{
				SDA_LOW;
			}
			SCL_LOW;delay(1); //SCL���� _|~~|_ �� _|~~|_
			SCL_HIGH;delay(2);//SDA����_|~~~~|_�� ______(�߻���������)
			SCL_LOW;delay(1);
			temp=temp<<1;
		}

		SDA_IN;
		SCL_LOW;delay(1);
		SCL_HIGH;delay(1);

		if(SDA!=GPIO_HIGH)
		{
			delay(1);
			bytecounter++;
			temp=*(lpData+bytecounter);
			SCL_LOW;delay(1);
		}
		else
		{
			delay(1);
			printk("readwrite retry!\n");
			temp=*lpData;
			bytecounter=0;
			cmd_send_counter++;
			SCL_LOW;delay(1);
			SMSTART();
		}

		if(cmd_send_counter>8)
		{
			bytecounter=4;
			goto read_out;
		}

	}while(bytecounter<4);

	//rev byte
	SDA_HIGH;
	SDA_IN;

	for(bytecounter=0;bytecounter< *(lpData+3);bytecounter++)
	{
		for(bitcounter=0;bitcounter<8;bitcounter++)
		{
			SCL_LOW;delay(1);
			SCL_HIGH;delay(1);

			if (SDA)
			{
				temp=temp|0x01;
			}
			else
			{
				temp=(temp&0xfe);
			}

			if(bitcounter<7)
			{
				temp=temp<<1;
			}

			SCL_HIGH;delay(1);
			SCL_LOW;delay(1);
		}

		SDA_LOW;delay(1);
		SCL_HIGH;delay(2);
		SCL_LOW;delay(1);
		SDA_IN;

		*(lpData+bytecounter+4)=temp;
	}
read_out:
	SMSTOP();
}

//*****************************AUTHENTICATION*****************************
//˫����֤����
//���:GC_select��ѡ����Կ����
//����:AAC��      ��֤���
//���̣�
//1.����������CIx,���������Q0,������Կ;�����Q1,��Q0��Q1���͸�����,������µ�CIx,SKx
//2.�ö��ص�CIx��������CIx�Ƚ�
//3.�ü������SKx���GCx,�ظ�1����
//AAC:��֤���������.AAC!=0xff��ʾ˫����֤δͨ����������
//������������Ӧ�ڳ������޸�
//************************************************************************
unsigned char AUTHENTICATION(unsigned char GC_select)
{
	unsigned char SK[8];     //�Ự��Կ
	unsigned char Q_CH[0x14];//����14H�ֽ�����
	unsigned char j;

	for(j=0;j<=19;j++)
	{
		GPA[j]=0x00;
	}//��ʼ��GPA��Ԫ

	Q_CH[0]=0xb6;
	Q_CH[1]=0x00;
	Q_CH[2]=GC_select<<4;
	Q_CH[2]=Q_CH[2]+0x50;
	Q_CH[3]=0x08;
	read(Q_CH);

	for(j=0;j<8;j++)
	{
		Q_CH[j+12]=Q_CH[j+4];
	}//CI

	for(j=0;j<8;j++)
	{
		Q_CH[j+4]=0xAA;
	}      //�˴��ɸ������

	for(j=0;j<4;j++)
	{
		GPA_CLOCK(Q_CH[12+2*j],0x03);
		GPA_CLOCK(Q_CH[12+2*j+1],0x03);
		GPA_CLOCK(Q_CH[4+j],0x01);
	}

	if ((GC_select&0x0f)==0x00)
	{
		for(j=0;j<8;j++)
		{
			Q_CH[12+j]=GC[j];//GC0[j];
		}
	}
	else if ((GC_select&0x0f)==0x01)
	{
		for(j=0;j<8;j++)
		{
			Q_CH[12+j]=GC[j];//GC1[j];
		}
	}
	else if ((GC_select&0x0f)==0x02)
	{
		for(j=0;j<8;j++)
		{
			Q_CH[12+j]=GC[j];//GC2[j];
		}
	}
	else
	{
		for(j=0;j<8;j++)
		{
			Q_CH[12+j]=GC[j];//GC3[j];
		}
	}

	for(j=0;j<4;j++)
	{
		GPA_CLOCK(Q_CH[12+2*j],0x03);
		GPA_CLOCK(Q_CH[12+2*j+1],0x03);
		GPA_CLOCK(Q_CH[8+j],0x01);
	}

	GPA_CLOCK(0x00,0x06);
	Q_CH[12]=GPA[0];
	for(j=1;j<8;j++)
	{
		GPA_CLOCK(0x00,0x07);
		Q_CH[12+j]=GPA[0];
	}

	Q_CH[0]=0xb8;//send ch
	Q_CH[2]=0x00;
	Q_CH[3]=0x10;
	Q_CH[1]=GC_select;
	write(Q_CH);
	Wait_For_AckPolling();

	//new ci
	Q_CH[12]=0xFF;
	for(j=1;j<8;j++)
	{
		GPA_CLOCK(0x00,0x02);
		Q_CH[12+j]=GPA[0];
	}
	//new sk
	for(j=0;j<8;j++)
	{
		GPA_CLOCK(0x00,0x02);
		SK[j]=GPA[0];
	}
	GPA_CLOCK(0x00,0x03);

	//�Ƚ�CIֵ�Ƿ�һ��
	Q_CH[0]=0XB6;
	Q_CH[1]=0X00;
	Q_CH[2]=GC_select<<4;
	Q_CH[2]=Q_CH[2]+0x50;
	Q_CH[3]=0x08;
	read(Q_CH);
	Wait_For_AckPolling();
	if (Q_CH[4]!=0xff)
	{
		printk("CI Error 1\r\n");
		goto Aut_Report;
	}

	for(j=0;j<8;j++)
	{
		if(Q_CH[4+j]!= Q_CH[12+j])
		{
			printk("CI Error 2\r\n");
			goto Aut_Report;
		}
	}

	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(Q_CH[2],0x01);
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(0x08,0x01);
	for(j=0;j<8;j++)
	{
		GPA_CLOCK(Q_CH[4+j],0x01);
		GPA_CLOCK(0x00,0x05);
	}
	//******************crypto_Authentication*************************
	//�ԻỰ��Կ����֤һ��
	//*****************************************************************
	for(j=0;j<=19;j++)
	{
		GPA[j]=0x00;
	}

	for(j=0;j<8;j++)
	{
		Q_CH[j+4]=0xAA;
	}      //�˴��ɸ������

	for(j=0;j<4;j++)
	{
		GPA_CLOCK(Q_CH[12+2*j],0x03);
		GPA_CLOCK(Q_CH[12+2*j+1],0x03);
		GPA_CLOCK(Q_CH[4+j],0x01);
	}

	for(j=0;j<4;j++)
	{
		GPA_CLOCK(SK[2*j],0x03);
		GPA_CLOCK(SK[2*j+1],0x03);
		GPA_CLOCK(Q_CH[8+j],0x01);
	}

	GPA_CLOCK(0x00,0x06);
	Q_CH[12]=GPA[0];
	for(j=1;j<8;j++)
	{
		GPA_CLOCK(0x00,0x07);
		Q_CH[12+j]=GPA[0];
	}

	Q_CH[0]=0xb8;
	Q_CH[1]=GC_select+0x10;
	Q_CH[2]=0x00;
	Q_CH[3]=0x10;
	write(Q_CH);
	Wait_For_AckPolling();

	//new ci
	Q_CH[12]=0xFF;
	for(j=1;j<8;j++)
	{
		GPA_CLOCK(0x00,0x02);
		Q_CH[12+j]=GPA[0];
	}
	//new sk
	for(j=0;j<8;j++)
	{
		GPA_CLOCK(0x00,0x02);
		SK[j]=GPA[0];
	}
	GPA_CLOCK(0x00,0x03);

	//�Ƚ�CIֵ�Ƿ�һ��
	Q_CH[0]=0XB6;
	Q_CH[1]=0X00;
	Q_CH[2]=GC_select<<4;
	Q_CH[2]=Q_CH[2]+0x50;
	Q_CH[3]=0x08;
	read(Q_CH);
	Wait_For_AckPolling();
	if (Q_CH[4]!=0xff)
	{
		printk("CI Error 3\r\n");
		goto Aut_Report;
	}

	for(j=0;j<8;j++)
	{
		if(Q_CH[4+j]!= Q_CH[12+j])
		{
			printk("CI Error 4\r\n");
			goto Aut_Report;
		}
	}

	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(Q_CH[2],0x01);
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(0x08,0x01);
	for(j=0;j<8;j++)
	{
		GPA_CLOCK(Q_CH[4+j],0x01);
		GPA_CLOCK(0x00,0x05);
	}

	//printk("AUTHENTICATION Success!\n");
Aut_Report:
		return Q_CH[4];
}



//*********************GPA_CLOCK************************************
//GPA����
//���:Data_in
//����:GPA[0]:GPA_byte
//����:times  GPA��������Ĵ���
//*****************************************************************
void GPA_CLOCK(unsigned char Datain,unsigned char times)
{
	unsigned char t;
	unsigned char d;
	unsigned char din_gpa;
	unsigned char Ri;
	unsigned char R_Sum;
	unsigned char Si;
	unsigned char S_Sum;
	unsigned char Ti;
	unsigned char T_Sum;

	for(t=0x00;t<times;t++)
	{
		din_gpa=Datain ^ GPA[0];
		Ri= din_gpa&0x1f;
		Si= ((din_gpa&0x0f)<<3)+((din_gpa&0xe0)>>5);
		Ti=(din_gpa&0xf8)>>3;
		//rԪ��
		if(((GPA[4])+((GPA[1]&0x0f)<<1)+((GPA[1]&0x10)>>4))>31)
		{
			R_Sum=((GPA[4])+((GPA[1]&0x0f)<<1)+((GPA[1]&0x10)>>4))-31;
		}
		else
		{
			R_Sum=((GPA[4])+((GPA[1]&0x0f)<<1)+((GPA[1]&0x10)>>4));
		}

		GPA[1]=GPA[2];
		GPA[2]=GPA[3];
		GPA[3]=GPA[4];
		GPA[4]=GPA[5]^Ri;
		GPA[5]=GPA[6];
		GPA[6]=GPA[7];
		GPA[7]=R_Sum;
		//sԪ��
		if ((GPA[9]+((GPA[8]&0x3f)<<1)+((GPA[8]&0x40)>>6) )>127)
		{
			S_Sum=( (GPA[9]) + ((GPA[8]&0x3f)<<1)+((GPA[8]&0x40)>>6) )-127;
		}
		else
		{
			S_Sum= (GPA[9]) + ((GPA[8]&0x3f)<<1)+((GPA[8]&0x40)>>6) ;
		}

		GPA[8]=GPA[9];
		GPA[9]=Si^GPA[10];
		GPA[10]=GPA[11];
		GPA[11]=GPA[12];
		GPA[12]=GPA[13];
		GPA[13]=GPA[14];
		GPA[14]=S_Sum;
		//tԪ��
		if ((GPA[15]+GPA[17])> 31)
		{
			T_Sum=GPA[15]+GPA[17]-31;
		}
		else
		{
			T_Sum=GPA[15]+GPA[17];
		}

		GPA[15]=GPA[16];
		GPA[16]=GPA[17];
		GPA[17]=GPA[18]^Ti;
		GPA[18]=GPA[19];
		GPA[19]=T_Sum;
		///Output
		if((GPA[14]&0x01)==0)
		{
			d=((GPA[7]^GPA[3])&0x01);
		}
		else
		{
			d=((GPA[19]^GPA[16])&0x01);
		}

		if((GPA[14]&0x02)==0)
		{
			d=d+((GPA[7]^GPA[3])&0x02);
		}
		else
		{
			d=d+((GPA[19]^GPA[16])&0x02);
		}

		if((GPA[14]&0x04)==0)
		{
			d=d+((GPA[7]^GPA[3])&0x04);
		}
		else
		{
			d=d+((GPA[19]^GPA[16])&0x04);
		}

		if((GPA[14]&0x08)==0)
		{
			d=d+((GPA[7]^GPA[3])&0x08);
		}
		else
		{
			d=d+((GPA[19]^GPA[16])&0x08);
		}

		GPA[0]= ( (((GPA[0])&0x0f)<<4) +d); //GPA����
	}
}

//******************verify_wr_pw********************************
//У��д����������������ȷУ����д����󿪷Ŷ�д
//pw_select����������ѡ��
//PAC:����У����������.PAC!=0xff��ʾ��֤δͨ����������
//****************************************************************
unsigned char verify_write_password(unsigned char pw_select)
{
	unsigned char j;
	unsigned char pw[7];

	if ((pw_select&0x0f)==0x00)
	{
		for(j=0;j<3;j++)
		{
			pw[4+j]=PW[j];//PW_WRITE0[j];
		}
	}
	else if ((pw_select&0x0f)==0x01)
	{
		for(j=0;j<3;j++)
		{
			pw[4+j]=PW[j];//PW_WRITE1[j];
		}
	}
	else if ((pw_select&0x0f)==0x02)
	{
		for(j=0;j<3;j++)
		{
			pw[4+j]=PW[j];//PW_WRITE2[j];
		}
	}
	else if ((pw_select&0x0f)==0x03)
	{
		for(j=0;j<3;j++)
		{
			pw[4+j]=PW[j];//PW_WRITE3[j];
		}
	}
	else if ((pw_select&0x0f)==0x04)
	{
		for(j=0;j<3;j++)
		{
			pw[4+j]=PW[j];//PW_WRITE4[j];
		}
	}
	else if ((pw_select&0x0f)==0x05)
	{
		for(j=0;j<3;j++)
		{
			pw[4+j]=PW[j];//PW_WRITE5[j];
		}
	}
	else if ((pw_select&0x0f)==0x06)
	{
		for(j=0;j<3;j++)
		{
			pw[4+j]=PW[j];//PW_WRITE6[j];
		}
	}
	else
	{
		for(j=0;j<3;j++)
		{
			pw[4+j]=PW[j];//PW_WRITE7[j];
		}
	}

	for(j=0;j<3;j++)
	{
		GPA_CLOCK(pw[4+j],0x05);
		pw[4+j]=GPA[0];
	}
	pw[0]=0xba;
	pw[1]=pw_select&0x0f;
	pw[2]=0x00;
	pw[3]=0x03;
	write(pw);
	Wait_For_AckPolling();

	pw[0]=0XB6;
	pw[1]=0X00;
	pw[2]=pw_select&0x0f;
	pw[2]=pw[2]*0x08;
	pw[2]=pw[2]+0xb0;
	pw[3]=0x01;
	read(pw);
	Wait_For_AckPolling();

	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(pw[2],0x01);
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(0x01,0x01);
	pw[4]=pw[4]^GPA[0];
	GPA_CLOCK(pw[4],0x01);
	GPA_CLOCK(0x00,0x05);

	return pw[4];
}

//*******************select_user_zone************************
//ѡ���û���
//zone���û������
//**********************************************************
void select_user_zone(unsigned char zone)
{
	unsigned char zon[4];

	GPA_CLOCK(zone,0x01);
	zon[0]=0xb4;
	zon[1]=0x03;
	zon[2]=zone;
	zon[3]=0x00;
	write(zon);
	Wait_For_AckPolling();
}
//**************************read_user_zone**************************
//�����û����������ݣ��������������
//rd_high_addr �û������ֽڵ�ַ
//rd_low_addr  �û������ֽڵ�ַ
//rd_number    ��ȡ�������ݼ�����������ݵĸ���
//���ܳ����������ݴ����rwdata[0x04]=>rwdata[0x13]
//******************************************************************
void read_user_zone(unsigned char rd_high_addr,unsigned char rd_low_addr,unsigned char rd_number)
{
	unsigned char j;

	rwdata[0]=0xb2;
	rwdata[1]=rd_high_addr;
	rwdata[2]=rd_low_addr;
	rwdata[3]=rd_number;
	read(rwdata);
	Wait_For_AckPolling();

	//��ʼ������
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(rd_low_addr,0x01);
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(rd_number,0x01);

	for(j=0;j<rd_number;j++)
	{
		rwdata[4+j]=rwdata[4+j]^GPA[0];
		GPA_CLOCK(rwdata[4+j],0x01);
		GPA_CLOCK(0x00,0x05);
	}
}

//**************************read_config_zone**************************
//�����û����������ݣ��������������
//rd_high_addr �û������ֽڵ�ַ
//rd_low_addr  �û������ֽڵ�ַ
//rd_number    ��ȡ�������ݼ�����������ݵĸ���
//���ܳ����������ݴ����rwdata[0x04]=>rwdata[0x13]
//******************************************************************
void read_config_zone(unsigned char rd_high_addr,unsigned char rd_low_addr,unsigned char rd_number)
{
	unsigned char j;

	rwdata[0]=0xb6;
	rwdata[1]=rd_high_addr;
	rwdata[2]=rd_low_addr;
	rwdata[3]=rd_number;
	read(rwdata);
	Wait_For_AckPolling();

	//��ʼ������
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(rd_low_addr,0x01);
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(rd_number,0x01);

	for(j=0;j<rd_number;j++)
	{
		if(rd_low_addr>=0xb0)
		{
			rwdata[4+j]=rwdata[4+j]^GPA[0];
		}
		GPA_CLOCK(rwdata[4+j],0x01);
		GPA_CLOCK(0x00,0x05);
	}
}

//*************************encrypto_data*******************************
//���������ݽ��м���
//encryptodatanumber�������ݵĸ���
//����ܵ��������ݴ���� rwdata[4]��rwdata[0x13]
//*********************************************************************
void encrypto_data(unsigned char encryptodatanumber)
{

	unsigned char j;
	unsigned char temp;
	for(j=0;j<encryptodatanumber;j++)
	{
		GPA_CLOCK(0x00,0x05);
		temp=rwdata[4+j];
		rwdata[4+j]=rwdata[4+j]^GPA[0];
		GPA_CLOCK(temp,0x01);
	}
}

//***************send checksum********************
//����:����У��͵�����(��������ʽд�����ݺ�,�跢��У���)
//**************************************************
void send_checksum(void)
{
	unsigned char dat6[6];

	dat6[0]=0xb4;
	dat6[1]=0x02;
	dat6[2]=0x00;
	dat6[3]=0x02;

	GPA_CLOCK(0x00,0x0f);
	dat6[4]=GPA[0];
	GPA_CLOCK(0x00,0x05);
	dat6[5]=GPA[0];

	write(dat6);
}
//************************write_user_zone*********************************
//����:���Ѽ����˵���������д������
//wr_high_addr �û������ֽڵ�ַ
//wr_low_addr  �û������ֽڵ�ַ
//wr_number    д�����ݵĸ���
//������� rwdata[4]��rwdata[0x13]��������д������
////////////////////////////////////////////
void write_user_zone(unsigned char wr_high_addr,unsigned char wr_low_addr,unsigned char wr_number)
{
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(wr_low_addr,0x01);
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(wr_number,0x01);

	rwdata[0]=0xb0;
	rwdata[1]=wr_high_addr;
	rwdata[2]=wr_low_addr;
	rwdata[3]=wr_number;
	encrypto_data(wr_number);
	write(rwdata);
	mdelay(7);
	send_checksum();
	Wait_For_AckPolling();
}

//************************write_user_zone*********************************
//����:���Ѽ����˵���������д������
//wr_high_addr �û������ֽڵ�ַ
//wr_low_addr  �û������ֽڵ�ַ
//wr_number    д�����ݵĸ���
//������� rwdata[4]��rwdata[0x13]��������д������
////////////////////////////////////////////
void write_config_zone(unsigned char wr_high_addr,unsigned char wr_low_addr,unsigned char wr_number)
{
	unsigned char j;
	unsigned char temp;
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(wr_low_addr,0x01);
	GPA_CLOCK(0x00,0x05);
	GPA_CLOCK(wr_number,0x01);

	rwdata[0]=0xb4;
	rwdata[1]=wr_high_addr;
	rwdata[2]=wr_low_addr;
	rwdata[3]=wr_number;
	//encrypto_data(wr_number);
	for(j=0;j<wr_number;j++)
	{
		GPA_CLOCK(0x00,0x05);
		temp=rwdata[4+j];
		if(wr_low_addr>=0xb0)
		{
			rwdata[4+j]=rwdata[4+j]^GPA[0];
		}
		GPA_CLOCK(temp,0x01);
	}

	write(rwdata);
	Wait_For_AckPolling();
	mdelay(7);
	send_checksum();
	Wait_For_AckPolling();
}


//****************���Ķ�д����******************************
//****************verify_sc_plaintext****************************
//����:У�鰲ȫ���루�������룩,������λ����write7_password
//��������˿δ��ǰ,У�鰲ȫ����ɹ���,���Զ�����������,�û����������д
//sc_firsr_byte  ����ĵ�1���ֽ�����
//sc_second_byte ����ĵ�2���ֽ�����
//sc_third_byte  ����ĵ�3���ֽ�����
//��verify_sc_plaintext(0xDD,0x42,0x97)
//0xDD,0x42,0x97�ֱ�Ϊ����ĵ�123���ֽ�����
//*****************************************************************
unsigned char verify_sc_plaintext(unsigned char sc_first_byte,unsigned char sc_second_byte,unsigned char sc_third_byte)
{
	unsigned char scdata[7];
	scdata[0]=0xBA;
	scdata[1]=0x07;
	scdata[2]=0x00;
	scdata[3]=0x03;
	scdata[4]=sc_first_byte;
	scdata[5]=sc_second_byte;
	scdata[6]=sc_third_byte;
	write(scdata);

	scdata[0]=0xb6;///����PAC_WR7
	scdata[1]=0x00;
	scdata[2]=0xE8;
	scdata[3]=0x01;
	read(scdata);
	return scdata[4];
}

//************************set_user_zone_plaintext***********************
//ѡ���û��������ģ�
//zonep:�û������
//*********************************************************************
void set_user_zone_plaintext(unsigned char zonep)
{
	unsigned char zon[4];
	zon[0]=0xb4;
	zon[1]=0x03;
	zon[2]=zonep;
	zon[3]=0x00;
	write(zon);
}

//**************************read_paintext************************************
//������(����)
//rd_cmd:0xb2 ���û��� /0xb6 ��������  /0xb6 0x01 0x00 0x01 ����˿
//A1    :���ֽڵ�ַ
//A2    :���ֽڵ�ַ
//N     :��ȡ���ݸ���
//*******************************************************************************
void read_paintext(unsigned char rd_cmd,unsigned char A1,unsigned char A2,unsigned char N)
{
	rwdata[0]=rd_cmd;//0xb2 �� 0xb6
	rwdata[1]=A1;    //A1
	rwdata[2]=A2;    //A2
	rwdata[3]=N;     //N
	read(rwdata);
}

//**************************write_paintext************************************
//д����(����)
//rd_cmd:0xb0 д�û��� /0xb4 д������  /0xb4 0x01 ID 0x00  д��˿
//A1    :���ֽڵ�ַ
//A2    :���ֽڵ�ַ
//N     :д���ݸ���
//ע�⣺�ն���˿ʱֻ�ܴ�ID=0x06=>0x04=>0x00
//*******************************************************************************
void write_paintext(unsigned char wr_cmd,unsigned char A1,unsigned char A2,unsigned char N)
{
	rwdata[0]=wr_cmd;//0xb0 �� 0xb4
	rwdata[1]=A1;    //A1
	rwdata[2]=A2;    //A2
	rwdata[3]=N;     //N
	write(rwdata);
}

bool verify_auth(unsigned char GCx,unsigned char PWx)
{
	unsigned char AAC;//AAC=0xFF�����У��GCx��ȷ
	unsigned char PAC;//PAC=0xFF�����У��PWx��ȷ
	AAC = AUTHENTICATION(GCx);
	if (AAC != 0xFF)
	{
		printk("22222222222222222222\n");
		return FALSE;
	}
	PAC = verify_write_password(PWx);
	if (PAC != 0xFF)
	{
		printk("33333333333333333333\n");
		return FALSE;
	}
	printk("authentication success!\n");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////




#define AT88SCXX_DEV_NAME      "at88scxx"
#define AT88SCXX_DEV_MAJOR     0

static int      at88scxx_major = AT88SCXX_DEV_MAJOR;
static dev_t    dev;
static struct   cdev at88scxx_cdev;
static struct   class *at88scxx_class;



static int at88scxx_to_gpio(void)
{
	//rk30_mux_api_set(GPIO0C6_TRACECLK_SMCADDR2_NAME, GPIO0C_GPIO0C6); 			
	//rk30_mux_api_set(GPIO0C7_TRACECTL_SMCADDR3_NAME, GPIO0C_GPIO0C7);

	gpio_request(ASC_SDA_IO, "at88sc sda");
	gpio_request(ASC_SCL_IO, "at88sc scl");

	gpio_pull_updown(ASC_SCL_IO, PullDisable);
	gpio_pull_updown(ASC_SDA_IO, PullDisable);

	gpio_direction_output(ASC_SCL_IO, GPIO_HIGH);
	gpio_direction_output(ASC_SDA_IO, GPIO_HIGH);

	return 0;
}

static int at88scxx_open(struct inode *inode, struct file *filp)
{
    DBG("at88scxx_open\n");

	return 0;
}

static int at88scxx_release(struct inode *inode, struct file *filp)
{
    DBG("at88scxx_release\n");

	return 0;
}

static long at88scxx_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct ASC_DATA* ascdata;
	long ret = 0;

	DBG("at88scxx_ioctl: cmd = %d arg = 0x%08x\n",cmd, arg);
	ascdata = (struct ASC_DATA*)arg;
	if (ascdata == 0)
	{
		return -EINVAL;
	}
	switch (cmd){
		case 0://������֤
			memcpy(GC,ascdata->gc,8);
			memcpy(PW,ascdata->pw,3);
			//printk("GC value:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",GC[0],GC[0],GC[0],GC[0],GC[0],GC[0],GC[0],GC[0]);
			//printk("PW value:0x%02x,0x%02x,0x%02x\n",PW[0],PW[1],PW[2]);
			//printk("GC Index:%d,PW Index:%d\n",ascdata->gc_index,ascdata->pw_index);
			if (reboot > 4)//����5�κ�����֤����ֹоƬ����
			{
				return -EINVAL;
			}
			if (!verify_auth(ascdata->gc_index,ascdata->pw_index))
			{
				reboot++;
				return -EINVAL;
			}
			reboot = 0;
			return 0;
			break;
		case 1://���û���
			memcpy(GC,ascdata->gc,8);
			memcpy(PW,ascdata->pw,3);
			if (reboot > 4)//����5�κ�����֤����ֹоƬ����
			{
				return -EINVAL;
			}
			if (!verify_auth(ascdata->gc_index,ascdata->pw_index))
			{
				reboot++;
				return -EINVAL;
			}
			select_user_zone(ascdata->zone);
			read_user_zone(0x00,0x00,0x10);
			memcpy(ascdata->data,rwdata+4,0x10);
			read_user_zone(0x00,0x10,0x10);
			memcpy(ascdata->data+0x10,rwdata+4,0x10);
			reboot = 0;
			return 0;
			break;
		case 2://д�û���
			memcpy(GC,ascdata->gc,8);
			memcpy(PW,ascdata->pw,3);
			if (reboot > 4)//����5�κ�����֤����ֹоƬ����
			{
				return -EINVAL;
			}
			if (!verify_auth(ascdata->gc_index,ascdata->pw_index))
			{
				reboot++;
				return -EINVAL;
			}
			select_user_zone(ascdata->zone);
			memcpy(rwdata+4,ascdata->data,0x10);
			write_user_zone(0x00,0x00,0x10);
			memcpy(rwdata+4,ascdata->data+0x10,0x10);
			write_user_zone(0x00,0x10,0x10);
			select_user_zone(ascdata->zone);
			reboot = 0;
			return 0;
			break;
		case 3://�޸�����
			memcpy(GC,ascdata->gc,8);
			memcpy(PW,ascdata->pw,3);
			//printk("GC value:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",GC[0],GC[0],GC[0],GC[0],GC[0],GC[0],GC[0],GC[0]);
			//printk("PW value:0x%02x,0x%02x,0x%02x\n",PW[0],PW[1],PW[2]);
			//printk("GC Index:%d,PW Index:%d\n",ascdata->gc_index,ascdata->pw_index);
			if (reboot > 4)//����5�κ�����֤����ֹоƬ����
			{
				return -EINVAL;
			}
			if (!verify_auth(ascdata->gc_index,ascdata->pw_index))
			{
				reboot++;
				return -EINVAL;
			}
			//printk("new PW value:0x%02x,0x%02x,0x%02x\n",ascdata->data[0],ascdata->data[1],ascdata->data[2]);
			memcpy(rwdata+4,ascdata->data,3);
			write_config_zone(0,ascdata->pw_index*8+0xb1,3);
			write_config_zone(0,ascdata->pw_index*8+0xb5,3);
			reboot = 0;
			return 0;
			break;
		case 4://��ȡ����
			memcpy(GC,ascdata->gc,8);
			memcpy(PW,ascdata->pw,3);
			if (reboot > 4)//����5�κ�����֤����ֹоƬ����
			{
				return -EINVAL;
			}
			if (!verify_auth(ascdata->gc_index,ascdata->pw_index))
			{
				reboot++;
				return -EINVAL;
			}
			read_config_zone(0,ascdata->data[0]*8+0xb0,4);
			memcpy(ascdata->data,rwdata+4,4);
			reboot = 0;
			return 0;
			break;
		case 5://��ȡ������
			if (reboot > 4)//����5�κ�����֤����ֹоƬ����
			{
				return -EINVAL;
			}
			if (!verify_auth(ascdata->gc_index,ascdata->pw_index))
			{
				reboot++;
				return -EINVAL;
			}
			read_config_zone(0,ascdata->data[0],8);
			memcpy(ascdata->data,rwdata+4,8);
			return 0;
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		default:
			ret = -EINVAL;
			break;
	}
	return ret;
}


static struct file_operations at88scxx_fops = {
	.owner   = THIS_MODULE,
	.open    = at88scxx_open,
	.release    = at88scxx_release,
	.unlocked_ioctl   = at88scxx_ioctl,
};

static int at88scxx_probe(struct platform_device *pdev)
{
	printk("%s:at88scxx initialized\n",__FUNCTION__);

	return 0;
}

static int at88scxx_remove(struct platform_device *pdev)
{
	gpio_free(ASC_SCL_IO);
	gpio_free(ASC_SDA_IO);

	return 0;
}

static struct platform_driver at88scxx_driver = {
	.probe	= at88scxx_probe,
	.remove = at88scxx_remove,
	.driver	= {
		.name	= "at88scxx",
		.owner	= THIS_MODULE,
	},
};

static int __init at88scxx_init(void)
{
  int result;  
  //struct ASC_DATA data={0,2,{0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22},4,{0x44,0x44,0x44},{0}};
	platform_driver_register(&at88scxx_driver);
	at88scxx_to_gpio();
	IIC_Initial();
	//at88scxx_ioctl(0,0,(unsigned long)&data);
	//while(1);
  
	if (0 == at88scxx_major)
	{
		/* auto select a major */
		result = alloc_chrdev_region(&dev, 0, 1, AT88SCXX_DEV_NAME);
		at88scxx_major = MAJOR(dev);
	}
	else
	{
		/* use load time defined major number */
		dev = MKDEV(at88scxx_major, 0);
		result = register_chrdev_region(dev, 1, AT88SCXX_DEV_NAME);
	}


	/* initialize our char dev data */
	cdev_init(&at88scxx_cdev, &at88scxx_fops);

	/* register char dev with the kernel */
	result = cdev_add(&at88scxx_cdev, dev, 1);
    
	if (0 != result)
	{
		unregister_chrdev_region(dev, 1);
		printk("Error registrating mali device object with the kernel\n");
		return result;
	}

    at88scxx_class = class_create(THIS_MODULE, AT88SCXX_DEV_NAME);
    device_create(at88scxx_class, NULL, MKDEV(at88scxx_major, MINOR(dev)), NULL,
                  AT88SCXX_DEV_NAME);

	printk("at88scxx device driver module init\n");
	return 0;
}

static void __exit at88scxx_exit(void)
{
    device_destroy(at88scxx_class, MKDEV(at88scxx_major, 0));
    class_destroy(at88scxx_class);

    cdev_del(&at88scxx_cdev);
    unregister_chrdev_region(dev, 1);
    platform_driver_unregister(&at88scxx_driver);
}

module_init(at88scxx_init);
module_exit(at88scxx_exit);
MODULE_DESCRIPTION ("at88scxx driver");
MODULE_AUTHOR("kelvin@techvision.com.cn");
MODULE_LICENSE("GPL");
