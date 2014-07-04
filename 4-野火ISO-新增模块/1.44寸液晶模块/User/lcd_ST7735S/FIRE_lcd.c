/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_LCD.c
 * @brief      LCD 函数库
 * @author     野火科技
 * @version    v5.0
 * @date       2013-06-26
 */


#include "FIRE_lcd.h"

#include "FIRE_ASCII.h"

#include "bsp_lcd_st7735s.h"
#include "bsp_lcd_st7735R.h"

/*!
 *  @brief      LCD初始化
 *  @since      v5.0
 */
void LCD_init(void)
{
    Site_t site = {0, 0};
    Size_t size ;

    LCD_INIT();                             //初始化LCD

    //LCD_SET_DIR(LCD_DIR+1);

    //由于初始化的时候进行 屏幕方向 选择，因而初始化完成后，才获取宽高
    size.W = LCD_W;
    size.H = LCD_H;

    LCD_rectangle(site, size, BCOLOUR);     //初始化背景
}

/*!
 *  @brief      显示实心矩形
 *  @param      site    左上角坐标
 *  @param      size    矩形大小
 *  @param      rgb565  颜色
 *  @since      v5.0
*  Sample usage:        Site_t site = {10,20};   //x = 10 ,y = 20
                        Size_t size = {50,60};  // W = 50 ,H = 60
                        LCD_rectangle(site, size, RED);
 */
void LCD_rectangle(Site_t site, Size_t size, uint16_t rgb565)
{
    uint32_t n, temp;

    LCD_PTLON(site, size);              //开窗

    temp = (uint32_t)size.W * size.H;
    LCD_RAMWR();                        //写内存
    for(n = 0; n < temp; n++)
    {
        LCD_WR_DATA( rgb565 );          //写数据
    }
}

/*!
 *  @brief      画点
 *  @param      site    左上角坐标
 *  @param      rgb565  颜色
 *  @since      v5.0
*  Sample usage:        Site_t site = {10,20};   //x = 10 ,y = 20
                        LCD_point(site, RED);
 */
void LCD_point(Site_t site, uint16_t rgb565)
{
    Size_t size = {1, 1};
    LCD_PTLON(site, size);
    LCD_RAMWR();                        //写内存
    LCD_WR_DATA(rgb565);                //写数据
}

/*!
 *  @brief      显示字符
 *  @param      site    左上角坐标
 *  @param      ascii   字符
 *  @param      Color   字体颜色
 *  @param      bkColor 背景颜色
 *  @since      v5.0
*  Sample usage:        Site_t site = {10,20};   //x = 10 ,y = 20
                        LCD_char(site,'0', BLUE,RED);
 */
void LCD_char(Site_t site, uint8_t ascii, uint16_t Color, uint16_t bkColor)
{
#define MAX_CHAR_POSX (LCD_W-8)
#define MAX_CHAR_POSY (LCD_H-16)

    uint8_t temp, t, pos;
    Size_t size = {8, 16};

    if(site.x > MAX_CHAR_POSX || site.y > MAX_CHAR_POSY)
    {
        return;
    }

    LCD_PTLON(site, size);

    LCD_RAMWR();                    //写内存

    for (pos = 0; pos < 16; pos++)
    {
        temp = ascii_8x16[((ascii-0x20)*16)+pos];

        for(t = 0; t < 8; t++)
        {

            if(temp & 0x80)
            {
                LCD_WR_DATA(Color);
            }
            else
            {
                LCD_WR_DATA(bkColor);
            }
            temp <<= 1;
        }
    }
    return;
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY
}

/*!
 *  @brief      显示字符串
 *  @param      site    左上角坐标
 *  @param      Str     字符串地址
 *  @param      Color   字体颜色
 *  @param      bkColor 背景颜色
 *  @since      v5.0
*  Sample usage:        Site_t site = {10,20};   //x = 10 ,y = 20
                        LCD_str(site,"www.chuxue123.com", BLUE,RED);
 */
void LCD_str(Site_t site, uint8_t *Str, uint16_t Color, uint16_t bkColor)
{
#define MAX_CHAR_POSX (LCD_W-8)
#define MAX_CHAR_POSY (LCD_H-16)
    while(*Str != '\0')
    {
        if(site.x > MAX_CHAR_POSX )
        {
            //换行
            site.x = 0;
            site.y += 16;
        }
        if(site.y > MAX_CHAR_POSY )
        {
            //一屏
            site.y = 0;
            site.x = 0;
        }

        LCD_char(site, *Str, Color, bkColor);
        site.x += 8;
        Str ++ ;
    }
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY
}

/*!
 *  @brief      显示数字
 *  @param      site    左上角坐标
 *  @param      num     数字
 *  @param      Color   字体颜色
 *  @param      bkColor 背景颜色
 *  @since      v5.0
*  Sample usage:        Site_t site = {10,20};   //x = 10 ,y = 20
                        LCD_num(site,123, BLUE,RED);
 */
void LCD_num(Site_t site, uint32_t num, uint16_t Color, uint16_t bkColor)
{
    uint32_t res = num;
    uint8_t t = 0;
    Site_t sitetemp;
    sitetemp.y = site.y;

    if( num == 0 )
    {
        LCD_char(site, '0', Color, bkColor);
        return;
    }
    while( res )  /*得到数字长度t*/
    {
        res /= 10;
        t++;
    }

    while(num)
    {
        sitetemp.x = site.x + (8 * (t--) - 8);
        LCD_char(sitetemp, (num % 10) + '0', Color, bkColor);
        num /= 10 ;
    }
}

/*!
 *  @brief      显示数字（清空多余的位）
 *  @param      site            左上角坐标
 *  @param      num             数字
 *  @param      max_num_bit     最大的位数
 *  @param      Color           字体颜色
 *  @param      bkColor         背景颜色
 *  @since      v5.0
*  Sample usage:        Site_t site = {10,20};   //x = 10 ,y = 20
                        LCD_num_BC(site,123,5, BLUE,RED);
 */
void LCD_num_BC(Site_t site, uint32_t num, uint8_t max_num_bit, uint16_t Color, uint16_t bkColor)
{
    uint32_t res = num;
    uint8_t t = 0;
    Site_t sitetemp;
    Size_t size;

    sitetemp.y = site.y;

    if( num == 0 )
    {
        LCD_char(site, '0', Color, bkColor);

        site.x += 8;
        size.H  = 16;
        size.W  = 8 * (max_num_bit - 1);
        LCD_rectangle(site, size, bkColor);

        return;
    }
    while( res )            /*得到数字长度t*/
    {
        res /= 10;
        t++;
    }
    if(t >= max_num_bit )    //限制最大长度
    {
        t = max_num_bit;
    }

    res = t;

    while( t != 0 )
    {
        sitetemp.x = site.x + (8 * (t--) - 8);
        LCD_char(sitetemp, (num % 10) + '0', Color, bkColor);
        num /= 10 ;
    }

    if(res != max_num_bit )
    {
        size.W = 8 * (max_num_bit - res);
        site.x += (8 * res);
        size.H  = 16;
        LCD_rectangle(site, size, bkColor);
    }
}

/*!
 *  @brief      灰度图像显示
 *  @param      site            左上角坐标
 *  @param      size            显示图像大小
 *  @param      img             图像地址
 *  @since      v5.0
 *  Sample usage:       Site_t site = {10,20};      //x = 10 ,y = 20
                        Size_t size = {320,240};    //W = 320,H = 240
                        LCD_Img_gray(site, size, img);
 */
void LCD_Img_gray(Site_t site, Size_t size, uint8_t *img)
{
    uint32_t     total = (size.H * size.W);
    uint16_t     imgtemp;
    uint8_t     *pimg = (uint8_t *)img;

    LCD_PTLON(site, size);                      //开窗
    LCD_RAMWR();                                //写内存

    while(total--)
    {
        imgtemp     = (uint16_t) * (pimg++);
        imgtemp = GRAY_2_RGB565(imgtemp);
        LCD_WR_DATA( imgtemp );               //写图像数据
    }
}

/*!
 *  @brief      缩放灰度图像显示
 *  @param      site            左上角坐标
 *  @param      size            显示图像大小
 *  @param      img             图像地址
 *  @param      imgsize         图像大小
 *  @since      v5.0
 *  Sample usage:       Site_t site = {10,20};          //x = 10 ,y = 20
                        Size_t size = {80,60};          //W = 80,H = 60
                        Size_t imgsize = {320,240};     //W = 320,H = 240
                        LCD_Img_gray_Z(site, size, img,imgsize);
 */
void LCD_Img_gray_Z(Site_t site, Size_t size, uint8_t *img, Size_t imgsize)
{

    uint32_t temp, tempY;
    uint16_t x, y;
    uint16_t X, Y;
    uint8_t *pimg = (uint8_t *)img;
    uint16_t rgb;

    LCD_PTLON(site, size);                      //开窗

    LCD_RAMWR();                                //写内存

    for(y = 0; y < size.H; y++)
    {
        Y = ( (  y * imgsize.H   ) / size.H) ;
        tempY = Y * imgsize.W ;

        for(x = 0; x < size.W; x++)
        {
            X = ( x * imgsize.W  ) / size.W ;
            temp = tempY + X;
            rgb = GRAY_2_RGB565(pimg[temp]);    //
            LCD_WR_DATA(rgb);
        }
    }
}



void LCD_Img_Binary(Site_t site, Size_t size, uint8_t *img)
{
    uint32_t     total = (size.H * size.W) / 8;
    uint8_t     imgtemp;
    uint8_t       bitindex;
    uint8_t     *pimg = (uint8_t *)img;

    LCD_PTLON(site, size);                      //开窗

    LCD_RAMWR();                                //写内存

    while(total--)
    {
        imgtemp     = *(pimg++);
        bitindex    = 8;
        while(bitindex--)
        {
            if( imgtemp & (0x01 << bitindex) )
            {
                LCD_WR_DATA( BINARY_COLOR );      //写图像数据
            }
            else
            {
                LCD_WR_DATA(  BINARY_BGCOLOR  );      //写图像数据
            }
        }
    }
}

void LCD_Img_Binary_Z(Site_t site, Size_t size, uint8_t *img, Size_t imgsize)
{

    uint32_t temp, tempY;
    uint16_t x, y;
    uint16_t X, Y;
    uint8_t *pimg = (uint8_t *)img;

    LCD_PTLON(site, size);                      //开窗

    LCD_RAMWR();                                //写内存

    for(y = 0; y < size.H; y++)
    {
        Y = ( (  y * imgsize.H  ) / size.H) ;
        tempY = Y * imgsize.W ;

        for(x = 0; x < size.W; x++)
        {
            X = (( x * imgsize.W  ) / size.W) ;
            temp = tempY + X;
            if( (pimg[temp>>3] & (1 << (7 - (temp & 0x07))) ) == 0  )
            {
                LCD_WR_DATA( BINARY_BGCOLOR );        //写图像数据
            }
            else
            {
                LCD_WR_DATA( BINARY_COLOR );      //写图像数据
            }
        }
    }
}


