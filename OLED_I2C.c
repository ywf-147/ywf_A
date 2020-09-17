#include "main.h"
#include "OLED_I2C.h"

#include "codetab.h"
unsigned int LunarCalendarDay;

void OLED_WriteByte(u8 Addr, u8 Data)
{
    IIC_Start();

    IIC_Send_Byte(OLED_ADDRESS);
    IIC_Wait_Ack();

    IIC_Send_Byte(Addr);
    IIC_Wait_Ack();

    IIC_Send_Byte(Data);
    IIC_Wait_Ack();

    IIC_Stop();
}
void WriteCmd(u8 I2C_Command)//写命令
{
    OLED_WriteByte(0x00, I2C_Command);
}

void WriteDat(u8 I2C_Data)//写数据
{
    OLED_WriteByte(0x40, I2C_Data);
}

void OLED_Init(void)
{
    delay_ms(100); //这里的延时很重要

    WriteCmd(0xAE); //display off
    WriteCmd(0x20);	//Set Memory Addressing Mode
    WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
    WriteCmd(0xc8);	//Set COM Output Scan Direction
    WriteCmd(0x00); //---set low column address
    WriteCmd(0x10); //---set high column address
    WriteCmd(0x40); //--set start line address
    WriteCmd(0x81); //--set contrast control register
    WriteCmd(0xff); //亮度调节 0x00~0xff
    WriteCmd(0xa1); //--set segment re-map 0 to 127
    WriteCmd(0xa6); //--set normal display
    WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    WriteCmd(0x3F); //
    WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    WriteCmd(0xd3); //-set display offset
    WriteCmd(0x00); //-not offset
    WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    WriteCmd(0xf0); //--set divide ratio
    WriteCmd(0xd9); //--set pre-charge period
    WriteCmd(0x22); //
    WriteCmd(0xda); //--set com pins hardware configuration
    WriteCmd(0x12);
    WriteCmd(0xdb); //--set vcomh
    WriteCmd(0x20); //0x20,0.77xVcc
    WriteCmd(0x8d); //--set DC-DC enable
    WriteCmd(0x14); //
    WriteCmd(0xaf); //--turn on oled panel
}

void OLED_SetPos(u8 x, u8 y) //设置起始点坐标
{
    WriteCmd(0xb0 + y);
    WriteCmd(((x & 0xf0) >> 4) | 0x10);
    WriteCmd((x & 0x0f) | 0x01);
}

void OLED_Fill(u8 fill_Data)//全屏填充
{
    u8 m, n;

    for(m = 0; m < 8; m++)
    {
        WriteCmd(0xb0 + m);		//page0-page1
        WriteCmd(0x00);		//low column start address
        WriteCmd(0x10);		//high column start address

        for(n = 0; n < 128; n++)
        {
            WriteDat(fill_Data);
        }
    }
}

void OLED_CLS(void)//清屏
{
    OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          :
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
    WriteCmd(0X8D);  //设置电荷泵
    WriteCmd(0X14);  //开启电荷泵
    WriteCmd(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          :
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
    WriteCmd(0X8D);  //设置电荷泵
    WriteCmd(0X10);  //关闭电荷泵
    WriteCmd(0XAE);  //OLED休眠
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(u8 x, u8 y, u8 ch[], u8 TextSize)
// Calls          :
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void OLED_ShowStr(u8 x, u8 y, u8 *ch, u8 TextSize)
{
    u8 c = 0, i = 0, j = 0, b = 0;

    switch(TextSize)
    {
        case 1:
        {
            while(*ch != '\0')
            {
                c = *ch - 32;

                if(x > 126)
                {
                    x = 0;
                    y++;
                }

                OLED_SetPos(x, y);

                for(i = 0; i < 6; i++)
                    WriteDat(F6x8[c][i]);

                x += 6;
                j++;
            }
        }
        break;

        case 2:
        {
            if(*ch == 0xff)
            {
                b = '~' - 32;
                OLED_SetPos(x, y);

                for(i = 0; i < 8; i++)
                    WriteDat(F8X16[((b * 16) + 32) + i]);

                OLED_SetPos(x, y + 1);

                for(i = 0; i < 8; i++)
                    WriteDat(F8X16[((b * 16) + 15) + i + 8]);

                x += 8;
            }

            while(*ch != '\0')
            {
                c = *ch - 32;

                if(x > 120)
                {
                    x = 0;
                    y++;
                }

                OLED_SetPos(x, y);

                for(i = 0; i < 8; i++)
                    WriteDat(F8X16[c * 16 + i]);

                OLED_SetPos(x, y + 1);

                for(i = 0; i < 8; i++)
                    WriteDat(F8X16[c * 16 + i + 8]);

                x += 8;
                ch++;
            }
        }
        break;
    }
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(u8 x, u8 y, u8 N)
// Calls          :
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_ShowCN(u8 x, u8 y, u8 N)
{
    u8 wm = 0;
    unsigned int  adder = 32 * N;
    OLED_SetPos(x, y);

    for(wm = 0; wm < 16; wm++)
    {
        WriteDat(F16x161[adder]);
        adder += 1;
    }

    OLED_SetPos(x, y + 1);

    for(wm = 0; wm < 16; wm++)
    {
        WriteDat(F16x161[adder]);
        adder += 1;
    }
}
/*
void OLED_WriteData(u8 x,u8 y,u8 Data)
{
	u8 tmp;




}*/
//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[]);
// Calls          :
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
void OLED_DrawBMP(u8 x0, u8 y0, u8 x1, u8 y1, u8 *BMP)
{
    unsigned int j = 0;
    u8 x, y;

    if(y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;

    for(y = y0; y < y1; y++)
    {
        OLED_SetPos(x0, y);

        for(x = x0; x < x1; x++)
        {
            WriteDat(BMP[j++]);
        }
    }
}
void Show_Time(u8 TmieMdeo, u8 N)
{
    switch(TmieMdeo)
    {
        case 0:
            OLED_DrawBMP(0 * 24, 2,  1 * 24, 6, (u8*)num24x44[N] + 24); //显示小时十位
            break;

        case 1:
            OLED_DrawBMP(1 * 24, 2,  2 * 24, 6, (u8*)num24x44[N] + 24); //显示小时个位
            OLED_DrawBMP(2 * 24, 2,  2 * 24 + 16, 6, (u8*)maohao + 16); //显示时钟的冒号
            break;

        case 2:
            OLED_DrawBMP(2 * 24 + 16, 2,  3 * 24 + 16, 6, (u8*)num24x44[N] + 24); //显示分钟十位
            break;

        case 3:
            OLED_DrawBMP(3 * 24 + 16, 2,  4 * 24 + 16, 6, (u8*)num24x44[N] + 24); //显示分钟个位
            break;

    }
}

u8 getIndex_From_GBST_16(signed char hz[])
{
    u8 i;

    for(i = 0; i < HZMAX; i++)
    {
        if((GBST_16[i].Index[0] == hz[0]) && (GBST_16[i].Index[1] == hz[1]))return i;
    }

    return 255;
    //

}

void OLED_ShowString16(u8 x, u8 y, u8 str[])
{
    u8 hzn, i, j;
    u8 wm, wm1;

    for(i = 0; str[i] != '\0'; i++)
    {
        if(str[i] < 127) //如果是ASCII字符，就按ASCII显示
        {
            OLED_SetPos(x, y);

            for(j = 0; j < 8; j++)
                //WriteDat(Y8X16[0+j]);
                WriteDat(Y8X16[(str[i] - 32) * 16 + j]);

            OLED_SetPos(x, y + 1);

            for(j = 0; j < 8; j++)
                //WriteDat(Y8X16[0+8+j]);
                WriteDat(Y8X16[(str[i] - 32) * 16 + 8 + j]);

            x = x + 8;
        }
        else//如果是汉字，就在字库中找下标并显示
        {
            hzn = getIndex_From_GBST_16((signed char*)str + i); //找下标
            OLED_SetPos(x, y); //上半部分

            for(wm = 0; wm < 16; wm++)
            {
                WriteDat(GBST_16[hzn].Msk[wm]);
            }

            OLED_SetPos(x, y + 1); //下半部分

            for(wm1 = 0; wm1 < 16; wm1++)
            {
                WriteDat( GBST_16[hzn].Msk[16 + wm1]    );
            }

            x = x + 16;
            //======这个不可随便乱动
            i++;
        }
    }

}



