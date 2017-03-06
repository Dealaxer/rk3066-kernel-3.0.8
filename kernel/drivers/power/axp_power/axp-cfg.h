#ifndef __LINUX_AXP_CFG_H_
#define __LINUX_AXP_CFG_H_

/*�豸��ַ*/
/*
	һ�㲻�ı䣺
	AXP20:0x34
	AXP19:0x34
*/
#define	AXP_DEVICES_ADDR	(0x68 >> 1)
/*i2c���������豸��:���忴��ʹ��ƽ̨Ӳ�������ӣ�����ʹ�����Զ�̽�⣬�˴����Բ����ã�����2��3��Ҫ����*/
#define	AXP_I2CBUS			1
/*��ԴоƬ��Ӧ���жϺţ����忴��ʹ�õ�ƽ̨Ӳ�������ӣ��ж���nmi����cpu����·irq����gpio*/
#define AXP_IRQNO			RK30_PIN6_PA4

/*��ʼ����·�������λmV��0��Ϊ�ر�*/
/*
	ldo1��
		��Ӳ�����������ѹ������Ĳ��ˣ�ֻ���������ʾ��ѹ����������
		LDO1SET�ӵ�:	1300
		LDO1SET��VINT:	3300
*/
#define AXP_LDO1_VALUE		1300
/*
	ldo2��
		AXP20:1800~3300��100/step
		AXP19:1800~3300��100/step
*/
#define AXP_LDO2_VALUE		3000
/*
	ldo3��
		AXP20:700~3500��25/step
		AXP19:700~3500��25/step
*/
#define AXP_LDO3_VALUE		2800
/*
	ldo4��
		AXP20:1250/1300/1400/1500/1600/1700/1800/1900/2000/2500/2700/2800/3000/3100/3200/3300
		AXP19:1800~3300,100/step
*/
#define AXP_LDO4_VALUE		2800
/*
	DCDC1:
		AXP20:��
		AXP19:700~3500��25/step
*/
#define AXP_DCDC1_VALUE		1400
/*
	DCDC2��
		AXP20:700~2275��25/step
		AXP19:700~2275��25/step
*/
#define AXP_DCDC2_VALUE		1500
/*
	DCDC3��
		AXP20:700~3500��25/step
		AXP19:700~3500��25/step
*/
#define AXP_DCDC3_VALUE		3000

/*���������mAh������ʵ�ʵ�����������壬�Կ��ؼƷ�����˵�����������Ҫ����������*/
#define BATCAP				2500

/*��ʼ��������裬m����һ����100~200֮�䣬������ø���ʵ�ʲ��Գ�����ȷ���������Ǵ򿪴�ӡ��Ϣ�����ӵ���պù̼����ϵ�أ����ӳ����������������1���Ӻ󣬽��ϳ��������1~2���ӣ�����ӡ��Ϣ�е�rdcֵ����������*/
#define BATRDC				40
/*��·��ѹ�����еĵ�ص�ѹ�Ļ���*/
#define AXP_VOL_MAX			12

/*
	���������ã�mA��0Ϊ�رգ�
		AXP20:300~1800,100/step
		AXP19:100/190/280/360/450/550/630/700/780/880/960/1000/1080/1160/1240/1320
*/
/*������������mA*/
#define STACHGCUR			1200
/*������������mA*/
#define EARCHGCUR			1200
/*���߳�������mA*/
#define SUSCHGCUR			EARCHGCUR
/*�ػ���������mA*/
#define CLSCHGCUR			EARCHGCUR

/*Ŀ�����ѹ��mV*/
/*
	AXP20:4100/4150/4200/4360
	AXP19:4100/4150/4200/4360
*/
#define CHGVOL				4200
/*������С�����õ�����ENDCHGRATE%ʱ��ֹͣ��磬%*/
/*
	AXP20:10\15
	AXP19:10\15
*/ 
#define ENDCHGRATE			10
/*adc���������ã�Hz*/
/*
	AXP20:25\50\100\200
	AXP19:25\50\100\200
*/
#define ADCFREQ				25
/*Ԥ��糬ʱʱ�䣬min*/
/*
	AXP20:40\50\60\70
	AXP19:30\40\50\60
*/
#define CHGPRETIME			50
/*������糬ʱʱ�䣬min*/
/*
	AXP20:360\480\600\720
	AXP19:420\480\540\600
*/
#define CHGCSTTIME			480


/*pek����ʱ�䣬ms*/
/*
	��power��Ӳ������ʱ�䣺
		AXP20:128/1000/2000/3000
		AXP19:128/512/1000/2000
*/
#define PEKOPEN				1000
/*pek����ʱ�䣬ms*/
/*
	��power���������жϵ�ʱ�䣬���ڴ�ʱ���Ƕ̰������̰���irq�����ڴ�ʱ���ǳ�������������irq��
		AXP20:1000/1500/2000/2500
		AXP19:1000/1500/2000/2500
*/
#define PEKLONG				1500
/*pek�����ػ�ʹ��*/
/*
	��power�������ػ�ʱ��Ӳ���ػ�����ʹ�ܣ�
		AXP20:0-���أ�1-�ػ�
		AXP19:0-���أ�1-�ػ�
*/
#define PEKOFFEN			1
/*pekpwr�ӳ�ʱ�䣬ms*/
/*
	������powerok�źŵ��ӳ�ʱ�䣺
		AXP20:8/64
		AXP19:8/64
*/
#define PEKDELAY			64
/*pek�����ػ�ʱ�䣬ms*/
/*
	��power���Ĺػ�ʱ����
		AXP20:4000/6000/8000/10000
		AXP19:4000/6000/8000/10000
*/
#define PEKOFF				6000

/*n_oe�ػ�ʱ�䣬ms*/
/*
	N_OE�ɵ͵���Ӳ���ػ��ӳ�ʱ��
		AXP20:128/1000/2000/3000
		AXP19:128/1000/2000/3000
*/
#define NOEOFF				2000

/*���¹ػ�ʹ��*/
/*
	AXP�ڲ��¶ȹ���Ӳ���ػ�����ʹ�ܣ�
		AXP20:0-���أ�1-�ػ�
		AXP19:0-���أ�1-�ػ�
*/
#define OTPOFFEN			1

/* usb ��ѹ��mV��0Ϊ������*/
/*
	AXP20:4100~4700��100/step
	AXP19:4100~4700��100/step
*/
#define USBVOLLIM			4000

/* usb ������mA��0Ϊ������*/
/*
	AXP20:100/500/900
	AXP19:100/500
*/
#define USBCURLIM			0

/*�����Ϣʣ�������ٷֱȵĹ������и���ʱ�䣬s*/
#define TIMER1				10
/*�����Ϣ���״̬�ı䣬���������Ϣ�ȶ�ʱ�䣬Timer1XTimer2 s*/
#define TIMER2				3
/*����������ʱ�䣬Timer1XTimer3 s*/
#define TIMER3				2
/*�ŵ��ѹ���ŵ��������ʱ�䣬Timer1XTimer4 s,ע��Timer4 > Timer3*/
#define TIMER4				4
/*�Կ��ؼƷ�����У׼ʱ�����ʣ�����������ʱ�䣬Timer1XTimer5 s*/
/*�Կ�·��ѹ�����ǵ��ʣ�����������ʱ�䣬Timer1XTimer5 s*/
#define TIMER5				3
/*δʹ��irqʱ�������Ϣ״̬�������и���ʱ�䣬Timer6X0.1 s*/
#define TIMER6				1

/*���ؼ��㷨�У����������ʼУ׼ʣ�������ٷֱȣ���Ҫ�ڷŵ羭����·��ѹУ׼������*/
/*
	��Ҫʵ�ʲ�����أ������⼶�Ŀ�·��ѹ��Ӧʣ������Ƚ�׼
*/
#define BATCAPCORRATE		10
/*�ػ�ʣ������ٷֱ�*/
#define SHUTDOWNRATE		0
/*�ػ���ѹ��mV*/
/*
	ϵͳ��ƵĹػ�����ĵ�ض˵�ѹ����Ҫ��ػ��ٷֱȡ���·��ѹ��Ӧ�ٷֱȱ��͵羯���ѹ�໥��ϲŻ�������
*/
#define SHUTDOWNVOL			3500
/*�͵羯���ѹ1��mV*/
/*
	����ϵͳ���������Ӳ�����Զ���
		AXP20:2867~4295��6/step
		AXP19:2867~4295��6/step
*/
#define WARNVOL1			(SHUTDOWNVOL * 1000)
#define WARNVOL2			((SHUTDOWNVOL - 50) * 1000)

/*��ʼ����·��ѹ��Ӧ�ٷֱȱ�*/
/*
	����ʹ��Ĭ��ֵ��������ø���ʵ�ʲ��Եĵ����ȷ��ÿ����Ӧ��ʣ��ٷֱȣ��ر���Ҫע�⣬�ػ���ѹSHUTDOWNVOL�͵��������ʼУ׼ʣ�������ٷֱ�BATCAPCORRATE��������׼ȷ��
	AXP20��19������
*/
#if 0
#define OCVREG0				0x00		//3.1328
#define OCVREG1				0x01		//3.2736
#define OCVREG2				0x02		//3.4144
#define OCVREG3				0x05		//3.5552
#define OCVREG4				0x07		//3.6256
#define OCVREG5				0x0D		//3.6608
#define OCVREG6				0x10		//3.6960
#define OCVREG7				0x1A		//3.7312
#define OCVREG8				0x24		//3.7664
#define OCVREG9				0x2E		//3.8016
#define OCVREGA				0x35		//3.8368
#define OCVREGB				0x3D		//3.8720
#define OCVREGC				0x49		//3.9424
#define OCVREGD				0x54		//4.0128
#define OCVREGE				0x5C		//4.0832
#define OCVREGF				0x64		//4.1536
#else
#define OCVREG0				0		  //3132
#define OCVREG1				0		  //3273
#define OCVREG2				0		  //3414
#define OCVREG3				0		//3555
#define OCVREG4				0//10		//3625
#define OCVREG5				5//17		//3660
#define OCVREG6				14//28		//3696
#define OCVREG7				27//38		//3731
#define OCVREG8				40//47		//3766
#define OCVREG9				49//53		//3801
#define OCVREGA				56//60		//3836
#define OCVREGB				63//67		//3872
#define OCVREGC				74//75		//3942
#define OCVREGD				82//81		//4012
#define OCVREGE				90//89		//4083
#define OCVREGF				100		//4153
#endif

/*AXP19 ��ʼ����·��ѹ*/
/*
	ֻ���AXP19�����Ըı䣬ע����ϱ��ʣ������ٷֱ�һһ��Ӧ
*/
#define OCVVOL0				3132
#define OCVVOL1				3273
#define OCVVOL2				3414
#define OCVVOL3				3555
#define OCVVOL4				3625
#define OCVVOL5				3660
#define OCVVOL6				3696
#define OCVVOL7				3731
#define OCVVOL8				3766
#define OCVVOL9				3801
#define OCVVOLA				3836
#define OCVVOLB				3872
#define OCVVOLC				3942
#define OCVVOLD				4012
#define OCVVOLE				4083
#define OCVVOLF				4153

/*  AXP�ж�ʹ��  */
#define	AXP_IRQ_USBLO		(1<< 1)	//usb �͵�
#define	AXP_IRQ_USBRE		(1<< 2)	//usb �γ�
#define	AXP_IRQ_USBIN		(1<< 3)	//usb ����
#define	AXP_IRQ_USBOV		(1<< 4)	//usb ��ѹ
#define	AXP_IRQ_ACRE		(1<< 5)	//ac  �γ�
#define	AXP_IRQ_ACIN		(1<< 6)	//ac  ����
#define	AXP_IRQ_ACOV		(1<< 7) //ac  ��ѹ
#define	AXP_IRQ_TEMLO		(1<< 8) //��ص���
#define	AXP_IRQ_TEMOV		(1<< 9) //��ع���
#define	AXP_IRQ_CHAOV		(1<<10) //��س�翪ʼ
#define	AXP_IRQ_CHAST		(1<<11) //��س�����
#define	AXP_IRQ_BATATOU		(1<<12) //����˳�����ģʽ
#define	AXP_IRQ_BATATIN		(1<<13) //��ؽ��뼤��ģʽ
#define AXP_IRQ_BATRE		(1<<14) //��ذγ�
#define AXP_IRQ_BATIN		(1<<15) //��ز���
#define	AXP_IRQ_PEKLO		(1<<16) //power���̰�
#define	AXP_IRQ_PEKSH		(1<<17) //power������

#define AXP_IRQ_DCDC3LO		(1<<19) //DCDC3С������ֵ
#define AXP_IRQ_DCDC2LO		(1<<20) //DCDC2С������ֵ
#define AXP_IRQ_DCDC1LO		(1<<21) //DCDC1С������ֵ
#define AXP_IRQ_CHACURLO	(1<<22) //������С������ֵ
#define AXP_IRQ_ICTEMOV		(1<<23) //AXPоƬ�ڲ�����
#define AXP_IRQ_EXTLOWARN2	(1<<24) //APS��ѹ�����ѹ1
#define AXP_IRQ_EXTLOWARN1	(1<<25) //APS��ѹ�����ѹ2
#define AXP_IRQ_USBSESUN	(1<<26) //USB Session End
#define AXP_IRQ_USBSESVA	(1<<27) //USB Session A/B
#define AXP_IRQ_USBUN		(1<<28) //USB ��Ч
#define AXP_IRQ_USBVA		(1<<29) //USB ��Ч
#define AXP_IRQ_NOECLO		(1<<30) //N_OE�ػ�
#define AXP_IRQ_NOEOPE		(1<<31) //N_OE����
#define AXP_IRQ_GPIO0TG		(1<<32) //GPIO0������ش���
#define AXP_IRQ_GPIO1TG		(1<<33) //GPIO1������ش�����ADC����IRQ
#define AXP_IRQ_GPIO2TG		(1<<34) //GPIO2������ش���
#define AXP_IRQ_GPIO3TG		(1<<35) //GPIO3������ش���

#define AXP_IRQ_PEKFE		(1<<37) //power���½��ش���
#define AXP_IRQ_PEKRE		(1<<38) //power�������ش���
#define AXP_IRQ_TIMER		(1<<39) //��ʱ����ʱ

/*ѡ����Ҫ�򿪵��ж�ʹ��*/
#define AXP_NOTIFIER_ON	   (AXP_IRQ_USBIN |\
				       		AXP_IRQ_USBRE |\
				       		AXP_IRQ_ACIN |\
				       		AXP_IRQ_ACRE |\
				       		AXP_IRQ_BATIN |\
				       		AXP_IRQ_BATRE |\
				       		AXP_IRQ_PEKLO |\
				       		AXP_IRQ_PEKSH |\
				       		AXP_IRQ_CHAST |\
				       		AXP_IRQ_CHAOV)

#define ABS(x)				((x) >0 ? (x) : -(x) )

#endif
