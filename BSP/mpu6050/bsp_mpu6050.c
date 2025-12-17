#include "bsp_mpu6050.h"
#include "stdio.h"


//void DMP_Init(void)
//{
//	uint8_t i=1,dmp_error;//��¼dmp����ԭ��
//	OLED_ShowString(0,8,"DMP ing...",8,1);
//	OLED_ShowString(0,16,"Attempts:",8,1); //���Դ���
//	OLED_ShowString(0,24,"Error:",8,1);    //����ԭ��
//	OLED_Refresh();
//	while(dmp_error == mpu_dmp_init())//DMP��ʼ��
//	{
//		
//#if DEBUG
//		printf("dmp_error: %d  \r\n",dmp_error);
//		i++;
//		printf("Attempts: %d   \r\n",i);
//#endif
//		
//		OLED_ShowNum(60,20,i++,2,8,1);
//		OLED_ShowNum(42,30,dmp_error,3,8,1);
//		OLED_Refresh();
//		
//		delay_ms(200);
//	}  
//	OLED_ShowString(60,10,"OK!",8,1);
//	OLED_Refresh();
//	delay_ms(1000);
//	OLED_Clear(); //����
//}

void DMP_Init(void)
{
	uint8_t dmp_error;
	do  // DMP初始化重试
	{
		dmp_error = mpu_dmp_init();
		delay_ms(100);
	}while(dmp_error);
}



/******************************************************************
 * �� �� �� �ƣ�IIC_Start
 * �� �� ˵ ����IIC��ʼʱ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: IIC_Start
 * Function description: IIC start timing
 * Function parameter: None
 * Function return: None
 * Author: LC
 * Remarks: None
******************************************************************/
void MPU_Start(void)
{
        SDA_OUT();
        SCL(1); 
        SDA(0);
        
        SDA(1);
        delay_us(5);
        SDA(0);
        delay_us(5);
                       
        SCL(0);
}
/******************************************************************
 * �� �� �� �ƣ�IIC_Stop
 * �� �� ˵ ����IICֹͣ�ź�
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: IIC_Stop
 * Function description: IIC stop signal
 * Function parameters: None
 * Function return: None
 * Author: LC
 * Notes: None
******************************************************************/
void MPU_Stop(void)
{
        SDA_OUT();
        SCL(0);
        SDA(0);
        
        SCL(1);
        delay_us(5);
        SDA(1);
        delay_us(5);
        
}

/******************************************************************
 * �� �� �� �ƣ�IIC_Send_Ack
 * �� �� ˵ ������������Ӧ����߷�Ӧ���ź�
 * �� �� �� �Σ�0����Ӧ��  1���ͷ�Ӧ��
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: IIC_Send_Ack
 * Function description: The host sends a response or non-response signal
 * Function parameter: 0 sends a response 1 sends a non-response
 * Function return: None
 * Author: LC
 * Notes: None
******************************************************************/
void MPU_Send_Ack(unsigned char ack)
{
        SDA_OUT();
        SCL(0);
        SDA(0);
        delay_us(5);
        if(!ack) SDA(0);
        else         SDA(1);
        SCL(1);
        delay_us(5);
        SCL(0);
        SDA(1);
}


/******************************************************************
 * �� �� �� �ƣ�I2C_WaitAck
 * �� �� ˵ �����ȴ��ӻ�Ӧ��
 * �� �� �� �Σ���
 * �� �� �� �أ�0��Ӧ��  1��ʱ��Ӧ��
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: I2C_WaitAck
 * Function description: Wait for slave response
 * Function parameter: None
 * Function return: 0 for response 1 for timeout and no response
 * Author: LC
 * Note: None
******************************************************************/
unsigned char MPU6050_WaitAck(void)
{
        
        char ack = 0;
        unsigned char ack_flag = 10;
        SCL(0);
        SDA(1);
        SDA_IN();
        
        SCL(1);
        while( (SDA_GET()==1) && ( ack_flag ) )
        {
                ack_flag--;
                delay_us(5);
        }
        
        if( ack_flag <= 0 )
        {
                MPU_Stop();
                return 1;
        }
        else
        {
                SCL(0);
                SDA_OUT();
        }
        return ack;
}

/******************************************************************
 * �� �� �� �ƣ�Send_Byte
 * �� �� ˵ ����д��һ���ֽ�
 * �� �� �� �Σ�datҪд�˵�����
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: Send_Byte
 * Function description: Write a byte
 * Function parameter: dat data to be written
 * Function return: None
 * Author: LC
 * Notes: None
******************************************************************/
void MPU_Send_Byte(uint8_t dat)
{
        int i = 0;
        SDA_OUT();
        SCL(0);//����ʱ�ӿ�ʼ���ݴ��� Pull the clock low to start data transmission
        
        for( i = 0; i < 8; i++ )
        {
                SDA( (dat & 0x80) >> 7 );
                delay_us(1);
                SCL(1);
                delay_us(5);
                SCL(0);
                delay_us(5);
                dat<<=1;
        }        
}

/******************************************************************
 * �� �� �� �ƣ�Read_Byte
 * �� �� ˵ ����IIC��ʱ��
 * �� �� �� �Σ���
 * �� �� �� �أ�����������
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: Read_Byte
 * Function description: IIC read timing
 * Function parameters: None
 * Function returns: Read data
 * Author: LC
 * Notes: None
******************************************************************/
unsigned char Read_Byte(void)
{
        unsigned char i,receive=0;
        SDA_IN();//SDA����Ϊ���� SDA is set as input
    for(i=0;i<8;i++ )
        {
        SCL(0);
        delay_us(5);
        SCL(1);
        delay_us(5);
        receive<<=1;
        if( SDA_GET() )
        {        
            receive|=1;   
        }
        delay_us(5); 
    }                                         
        SCL(0); 
  return receive;
}


uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t res;
    MPU_Start(); 
	MPU_Send_Byte((MPU_ADDR<<1)|0);//����������ַ+д����	
	MPU6050_WaitAck();		//�ȴ�Ӧ�� 
    MPU_Send_Byte(reg);	//д�Ĵ�����ַ
    MPU6050_WaitAck();		//�ȴ�Ӧ��
    MPU_Start();
	MPU_Send_Byte((MPU_ADDR<<1)|1);//����������ַ+������	
    MPU6050_WaitAck();		//�ȴ�Ӧ�� 
	res=Read_Byte();//��ȡ����,����nACK 
    MPU_Stop();			//����һ��ֹͣ���� 
	return res;		
}






/******************************************************************
 * �� �� �� �ƣ�MPU6050_WriteReg
 * �� �� ˵ ����IIC����д������
 * �� �� �� �Σ�addr������ַ regaddr�Ĵ�����ַ numҪд��ĳ��� regdataд������ݵ�ַ
 * �� �� �� �أ�0=��ȡ�ɹ�   ����=��ȡʧ��
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU6050_WriteReg
 * Function description: IIC writes data continuously
 * Function parameters: addr device address regaddr register address num length to be written regdata data address to be written
 * Function return: 0 = read successfully Other = read failed
 * Author: LC
 * Notes: None
******************************************************************/
char MPU6050_WriteReg(uint8_t addr,uint8_t regaddr,uint8_t num,uint8_t *regdata)
{
    uint16_t i = 0;
        MPU_Start();
//        Send_Byte((addr<<1)|0);
				MPU_Send_Byte((addr<<1)|0);
        if( MPU6050_WaitAck() == 1 ) {MPU_Stop();return 1;}
//        Send_Byte(regaddr);
				MPU_Send_Byte(regaddr);
        if( MPU6050_WaitAck() == 1 ) {MPU_Stop();return 2;}
    
        for(i=0;i<num;i++)
    {
//        Send_Byte(regdata[i]);
			MPU_Send_Byte(regdata[i]);
        if( MPU6050_WaitAck() == 1 ) {MPU_Stop();return (3+i);}
    }        
        MPU_Stop();
    return 0;
}


/******************************************************************
 * �� �� �� �ƣ�MPU6050_ReadData
 * �� �� ˵ ����IIC������ȡ����
 * �� �� �� �Σ�addr������ַ regaddr�Ĵ�����ַ numҪ��ȡ�ĳ��� Read��ȡ��������Ҫ�洢�ĵ�ַ
 * �� �� �� �أ�0=��ȡ�ɹ�   ����=��ȡʧ�� 
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU6050_ReadData
 * Function description: IIC reads data continuously
 * Function parameters: addr device address regaddr register address num length to read Read address where the read data is to be stored
 * Function return: 0 = read successfully Other = read failed
 * Author: LC
 * Notes: None
******************************************************************/
char MPU6050_ReadData(uint8_t addr, uint8_t regaddr,uint8_t num,uint8_t* Read)
{
        uint8_t i;
        MPU_Start();
//        Send_Byte((addr<<1)|0);
				MPU_Send_Byte((addr<<1)|0);
        if( MPU6050_WaitAck() == 1 ) {MPU_Stop();return 1;}
//        Send_Byte(regaddr);
				MPU_Send_Byte(regaddr);
        if( MPU6050_WaitAck() == 1 ) {MPU_Stop();return 2;}
        
        MPU_Start();
//        Send_Byte((addr<<1)|1);
			  MPU_Send_Byte((addr<<1)|1);
        if( MPU6050_WaitAck() == 1 ) {MPU_Stop();return 3;}
        
        for(i=0;i<(num-1);i++){
                Read[i]=Read_Byte();
                MPU_Send_Ack(0);
        }
        Read[i]=Read_Byte();
        MPU_Send_Ack(1);         
        MPU_Stop();
        return 0;
}


/******************************************************************
 * �� �� �� �ƣ�MPU_Set_Gyro_Fsr
 * �� �� ˵ ��������MPU6050�����Ǵ����������̷�Χ
 * �� �� �� �Σ�fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
 * �� �� �� �أ�0,���óɹ�  ����,����ʧ��
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU_Set_Gyro_Fsr
 * Function description: Set the full scale range of the MPU6050 gyroscope sensor
 * Function parameters: fsr: 0, ��250dps; 1, ��500dps; 2, ��1000dps; 3, ��2000dps
 * Function return: 0, setting successful Others, setting failed
 * Author: LC
 * Notes: None
******************************************************************/
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
        return MPU6050_WriteReg(0x68,MPU_GYRO_CFG_REG,1,(uint8_t*)(fsr<<3)); //���������������̷�Χ Set the gyroscope full-scale range
}    

/******************************************************************
 * �� �� �� �ƣ�MPU_Set_Accel_Fsr
 * �� �� ˵ ��������MPU6050���ٶȴ����������̷�Χ
 * �� �� �� �Σ�fsr:0,��2g;1,��4g;2,��8g;3,��16g
 * �� �� �� �أ�0,���óɹ�  ����,����ʧ��
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU_Set_Accel_Fsr
 * Function description: Set the full scale range of the MPU6050 acceleration sensor
 * Function parameters: fsr: 0, ��2g; 1, ��4g; 2, ��8g; 3, ��16g
 * Function return: 0, setting successful Others, setting failed
 * Author: LC
 * Notes: None
******************************************************************/
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
        return MPU6050_WriteReg(0x68,MPU_ACCEL_CFG_REG,1,(uint8_t*)(fsr<<3)); //���ü��ٶȴ����������̷�Χ   Set the full-scale range of the accelerometer
}

/******************************************************************
 * �� �� �� �ƣ�MPU_Set_LPF
 * �� �� ˵ ��������MPU6050�����ֵ�ͨ�˲���
 * �� �� �� �Σ�lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
 * �� �� �� �أ�0,���óɹ�  ����,����ʧ��
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU_Set_LPF
 * Function description: Set the digital low-pass filter of MPU6050
 * Function parameter: lpf: digital low-pass filter frequency (Hz)
 * Function return: 0, setting successful Others, setting failed
 * Author: LC
 * Note: None
******************************************************************/
uint8_t MPU_Set_LPF(uint16_t lpf)
{
        uint8_t data=0;
        
        if(lpf>=188)data=1;
        else if(lpf>=98)data=2;
        else if(lpf>=42)data=3;
        else if(lpf>=20)data=4;
        else if(lpf>=10)data=5;
        else data=6; 
    return data=MPU6050_WriteReg(0x68,MPU_CFG_REG,1,&data);//�������ֵ�ͨ�˲���   Setting the digital low-pass filter
}
/******************************************************************
 * �� �� �� �ƣ�MPU_Set_Rate
 * �� �� ˵ ��������MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
 * �� �� �� �Σ�rate:4~1000(Hz)  ��ʼ����rateȡ50
 * �� �� �� �أ�0,���óɹ�  ����,����ʧ��
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU_Set_Rate
 * Function description: Set the sampling rate of MPU6050 (assuming Fs=1KHz)
 * Function parameter: rate: 4~1000 (Hz) Initialization rate is 50
 * Function return: 0, setting successful Others, setting failed
 * Author: LC
 * Note: None
******************************************************************/
uint8_t MPU_Set_Rate(uint16_t rate)
{
        uint8_t data;
        if(rate>1000)rate=1000;
        if(rate<4)rate=4;
        data=1000/rate-1;
        data=MPU6050_WriteReg(0x68,MPU_SAMPLE_RATE_REG,1,&data);        //�������ֵ�ͨ�˲��� Setting the digital low-pass filter
         return MPU_Set_LPF(rate/2);            //�Զ�����LPFΪ�����ʵ�һ�� Automatically set LPF to half the sampling rate
}


/******************************************************************
 * �� �� �� �ƣ�MPU6050ReadGyro
 * �� �� ˵ ������ȡ����������
 * �� �� �� �Σ����������ݴ洢��ַ 
 * �� �� �� �أ�reg�ж��Ƿ��ȡ������ 0:��ȡ�� ����;û�ж�ȡ��
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU6050ReadGyro
 * Function description: Read gyroscope data
 * Function parameter: Gyroscope data storage address
 * Function return: None
 * Author: LC
 * Notes: reg determines whether data is read 0: read, others; not read
******************************************************************/


uint8_t MPU6050ReadGyro(short *gyroDatax,short *gyroDatay,short *gyroDataz)
{
        uint8_t buf[6];
        uint8_t reg ;
        //MPU6050_GYRO_OUT = MPU6050���������ݼĴ�����ַ
        //��������������Ĵ����ܹ���6���Ĵ�����ɣ�
        //���X/Y/Z������������Ǵ��������ݣ����ֽ���ǰ�����ֽ��ں�
        //ÿһ����16λ����˳��Ϊxyz
				//MPU6050_GYRO_OUT = MPU6050 gyroscope data register address
				//The gyroscope data output register consists of 6 registers in total,
				//Output the gyroscope sensor data of the three axes X/Y/Z, with the high byte in front and the low byte in the back.
				//Each axis is 16 bits, in order of xyz
        reg = MPU6050_ReadData(0x68,MPU6050_GYRO_OUT,6,buf);
        if( reg == 0 )
        {
					
					*gyroDatax=(buf[0]<<8)|buf[1];  
					*gyroDatay=(buf[2]<<8)|buf[3];  
					*gyroDataz=(buf[4]<<8)|buf[5];
//                gyroData[0] = (buf[0] << 8) | buf[1];
//                gyroData[1] = (buf[2] << 8) | buf[3];
//                gyroData[2] = (buf[4] << 8) | buf[5];
        }
				  return reg;
}
/******************************************************************
 * �� �� �� �ƣ�MPU6050ReadAcc
 * �� �� ˵ ������ȡ���ٶ�����
 * �� �� �� �Σ����ٶ����ݴ洢��ַ 
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU6050ReadAcc
 * Function description: Read acceleration data
 * Function parameter: Acceleration data storage address
 * Function return: None
 * Author: LC
 * Notes: None
******************************************************************/
//void MPU6050ReadAcc(short *accData)
//{
//        uint8_t buf[6];
//        uint8_t reg = 0;
//        //MPU6050_ACC_OUT = MPU6050���ٶ����ݼĴ�����ַ
//        //���ٶȴ�������������Ĵ����ܹ���6���Ĵ�����ɣ�
//        //���X/Y/Z������ļ��ٶȴ�����ֵ�����ֽ���ǰ�����ֽ��ں�
//				//MPU6050_ACC_OUT = MPU6050 acceleration data register address
//				//The acceleration sensor data output register consists of 6 registers in total,
//				//Output the acceleration sensor values ??of the three axes X/Y/Z, with the high byte in front and the low byte in the back.
//        reg = MPU6050_ReadData(0x68, MPU6050_ACC_OUT, 6, buf);
//        if( reg == 0)
//        {
//                accData[0] = (buf[0] << 8) | buf[1];
//                accData[1] = (buf[2] << 8) | buf[3];
//                accData[2] = (buf[4] << 8) | buf[5];
//        }
//}


uint8_t MPU6050ReadAcc(short *accData_x,short *accData_y,short *accData_z)
{
        uint8_t buf[6];
        uint8_t reg ;
        //MPU6050_ACC_OUT = MPU6050���ٶ����ݼĴ�����ַ
        //���ٶȴ�������������Ĵ����ܹ���6���Ĵ�����ɣ�
        //���X/Y/Z������ļ��ٶȴ�����ֵ�����ֽ���ǰ�����ֽ��ں�
				//MPU6050_ACC_OUT = MPU6050 acceleration data register address
				//The acceleration sensor data output register consists of 6 registers in total,
				//Output the acceleration sensor values ??of the three axes X/Y/Z, with the high byte in front and the low byte in the back.
        reg = MPU6050_ReadData(0x68, MPU6050_ACC_OUT, 6, buf);
        if( reg == 0)
        {
                *accData_x = (buf[0] << 8) | buf[1];
                *accData_y = (buf[2] << 8) | buf[3];
                *accData_z = (buf[4] << 8) | buf[5];
        }
				return reg;
}

/******************************************************************
 * �� �� �� �ƣ�MPU6050_GetTemp
 * �� �� ˵ ������ȡMPU6050�ϵ��¶�
 * �� �� �� �Σ���
 * �� �� �� �أ��¶�ֵ��λΪ��
 * ��       �ߣ�LC
 * ��       ע���¶Ȼ��㹫ʽΪ��Temperature = 36.53 + regval/340
 * Function name: MPU6050_GetTemp
 * Function description: Read the temperature on MPU6050
 * Function parameters: None
 * Function return: Temperature value in ��C
 * Author: LC
 * Note: Temperature conversion formula: Temperature = 36.53 + regval/340
******************************************************************/
float MPU6050_GetTemp(void)
{
        short temp3;
        uint8_t buf[2];
        float Temperature = 0;
        MPU6050_ReadData(0x68,MPU6050_RA_TEMP_OUT_H,2,buf); 
    temp3= (buf[0] << 8) | buf[1];        
        Temperature=((double) temp3/340.0)+36.53;
    return Temperature;
}

/******************************************************************
 * �� �� �� �ƣ�MPU6050ReadID
 * �� �� ˵ ������ȡMPU6050��������ַ
 * �� �� �� �Σ���
 * �� �� �� �أ�0=��ⲻ��MPU6050   1=�ܼ�⵽MPU6050
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU6050ReadID
 * Function description: Read the device address of MPU6050
 * Function parameter: None
 * Function return: 0 = MPU6050 cannot be detected 1 = MPU6050 can be detected
 * Author: LC
 * Note: None
******************************************************************/
uint8_t MPU6050ReadID(void)
{
        unsigned char Re[2] = {0};
        MPU6050_ReadData(0x68,0X75,1,Re);
        if (Re[0] != 0x68) {
                return 1;
        }
        return 0;
}

/******************************************************************
 * �� �� �� �ƣ�MPU6050_Init
 * �� �� ˵ ����MPU6050��ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ�0�ɹ�  1û�м�⵽MPU6050
 * ��       �ߣ�LC
 * ��       ע����
 * Function name: MPU6050_Init
 * Function description: MPU6050 initialization
 * Function parameters: None
 * Function return: 0 success 1 MPU6050 not detected
 * Author: LC
 * Notes: None
******************************************************************/
char MPU6050_Init(void)
{	
	
		uint8_t res;
     SDA_OUT();
    delay_ms(10);
    //��λ6050 Reset 6050
    MPU6050_WriteReg(0x68,MPU6050_RA_PWR_MGMT_1, 1,(uint8_t*)(0x80));
    delay_ms(100);
    //��Դ�����Ĵ��� Power Management Registers
    //ѡ��X��������Ϊ�ο�PLL��ʱ��Դ������CLKSEL=001
		//Select the X-axis gyro as the clock source for the reference PLL, set CLKSEL=001
    MPU6050_WriteReg(0x68,MPU6050_RA_PWR_MGMT_1,1, (uint8_t*)(0x00));
    
    MPU_Set_Gyro_Fsr(3);    //�����Ǵ�����,��2000dps Gyroscope sensor, ��2000dps
    MPU_Set_Accel_Fsr(0);   //���ٶȴ�����,��2g Accelerometer, ��2g
    MPU_Set_Rate(50);                

    MPU6050_WriteReg(0x68,MPU_INT_EN_REG , 1,(uint8_t*)0x00);        //�ر������ж� Disable all interrupts
    MPU6050_WriteReg(0x68,MPU_USER_CTRL_REG,1,(uint8_t*)0x00);        //I2C��ģʽ�ر� I2C Master Mode Off
    MPU6050_WriteReg(0x68,MPU_FIFO_EN_REG,1,(uint8_t*)0x00);                //�ر�FIFO Close FIFO
    MPU6050_WriteReg(0x68,MPU_INTBP_CFG_REG,1,(uint8_t*)0X80);        //INT���ŵ͵�ƽ��Ч INT pin low level is effective
    res=MPU_Read_Byte(MPU6050_WHO_AM_I); 
    if( MPU6050ReadID() == 0 )//����Ƿ���6050 Check if there is 6050
    {       
            MPU6050_WriteReg(0x68,MPU6050_RA_PWR_MGMT_1, 1,(uint8_t*)0x01);//����CLKSEL,PLL X��Ϊ�ο� Set CLKSEL, PLL X axis as reference
            MPU6050_WriteReg(0x68,MPU_PWR_MGMT2_REG, 1,(uint8_t*)0x00);//���ٶ��������Ƕ����� Both the accelerometer and the gyroscope work
            MPU_Set_Rate(50);        
            return 1;
    }
			else 
	{
		return 1;
	}
    return 0;
}




