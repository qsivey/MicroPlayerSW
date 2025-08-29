/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef __ST7789_H
#define __ST7789_H

#include "fonts.h"
#include "periph.h"

#define	displayAssert(x, y)			if ((x < 0) || (x >= ST7789_WIDTH) ||\
										(y < 0) || (y >= ST7789_HEIGHT))\
										return

#define	displayCheckTxMode(mode)	if (SPI_TxDMA_Mode != mode)\
									{\
										if (mode == TXDMAM_STATIC_16)\
										{\
											HAL_SPI_DeInit(&LCD_SPI_HANDLE);\
											__HAL_DMA_DISABLE(&LCD_SPI_TX_DMA_HANDLE);\
											LCD_SPI_HANDLE.Init.DataSize = SPI_DATASIZE_16BIT;\
											LCD_SPI_TX_DMA_HANDLE.Init.SrcInc = DMA_SINC_FIXED;\
											LCD_SPI_TX_DMA_HANDLE.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_HALFWORD;\
											LCD_SPI_TX_DMA_HANDLE.Init.DestDataWidth = DMA_DEST_DATAWIDTH_HALFWORD;\
											HAL_DMA_Init(&LCD_SPI_TX_DMA_HANDLE);\
											HAL_SPI_Init(&LCD_SPI_HANDLE);\
											SPI_TxDMA_Mode = TXDMAM_STATIC_16;\
										}\
										\
										else\
										{\
											HAL_SPI_DeInit(&LCD_SPI_HANDLE);\
											__HAL_DMA_DISABLE(&LCD_SPI_TX_DMA_HANDLE);\
											LCD_SPI_HANDLE.Init.DataSize = SPI_DATASIZE_8BIT;\
											LCD_SPI_TX_DMA_HANDLE.Init.SrcInc = DMA_SINC_INCREMENTED;\
											LCD_SPI_TX_DMA_HANDLE.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;\
											LCD_SPI_TX_DMA_HANDLE.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;\
											HAL_DMA_Init(&LCD_SPI_TX_DMA_HANDLE);\
											HAL_SPI_Init(&LCD_SPI_HANDLE);\
											SPI_TxDMA_Mode = TXDMAM_DYNAMIC_8;\
										}\
									}

#define		displayWriteSwap(c)		ui16 colorSwap = __builtin_bswap16(c)

#define		displayDrawMosaic()		for (int i = 0; i < 60; i++)\
										for (int k = 0; k < 60; k++)\
											ST7789_DrawFilledRectangle(i * 4, k * 4, 4, 4, rand() % 65536)

/*
 * Comment one to use another.
 * 3 parameters can be choosed
 * 135x240(0.96 inch) & 240x240(1.3inch) & 170x320(1.9inch)
 * X_SHIFT & Y_SHIFT are used to adapt different display's resolution
 */

/* Choose a type you are using */
//#define USING_135X240
#define USING_240X240
//#define USING_170X320

/* Choose a display rotation you want to use: (0-3) */
//#define ST7789_ROTATION 0
//#define ST7789_ROTATION 1
#define ST7789_ROTATION 2				//  use Normally on 240x240
//#define ST7789_ROTATION 3

#ifdef USING_135X240

    #if ST7789_ROTATION == 0
        #define ST7789_WIDTH 135
        #define ST7789_HEIGHT 240
        #define X_SHIFT 53
        #define Y_SHIFT 40
    #endif

    #if ST7789_ROTATION == 1
        #define ST7789_WIDTH 240
        #define ST7789_HEIGHT 135
        #define X_SHIFT 40
        #define Y_SHIFT 52
    #endif

    #if ST7789_ROTATION == 2
        #define ST7789_WIDTH 135
        #define ST7789_HEIGHT 240
        #define X_SHIFT 52
        #define Y_SHIFT 40
    #endif

    #if ST7789_ROTATION == 3
        #define ST7789_WIDTH 240
        #define ST7789_HEIGHT 135
        #define X_SHIFT 40
        #define Y_SHIFT 53
    #endif

#endif

#ifdef USING_240X240

    #define ST7789_WIDTH 240
    #define ST7789_HEIGHT 240

		#if ST7789_ROTATION == 0
			#define X_SHIFT 0
			#define Y_SHIFT 80
		#elif ST7789_ROTATION == 1
			#define X_SHIFT 80
			#define Y_SHIFT 0
		#elif ST7789_ROTATION == 2
			#define X_SHIFT 0
			#define Y_SHIFT 0
		#elif ST7789_ROTATION == 3
			#define X_SHIFT 0
			#define Y_SHIFT 0
		#endif

#endif

#ifdef USING_170X320

	#if ST7789_ROTATION == 0
        #define ST7789_WIDTH 170
        #define ST7789_HEIGHT 320
        #define X_SHIFT 35
        #define Y_SHIFT 0
    #endif

    #if ST7789_ROTATION == 1
        #define ST7789_WIDTH 320
        #define ST7789_HEIGHT 170
        #define X_SHIFT 0
        #define Y_SHIFT 35
    #endif

    #if ST7789_ROTATION == 2
        #define ST7789_WIDTH 170
        #define ST7789_HEIGHT 320
        #define X_SHIFT 35
        #define Y_SHIFT 0
    #endif

    #if ST7789_ROTATION == 3
        #define ST7789_WIDTH 320
        #define ST7789_HEIGHT 170
        #define X_SHIFT 0
        #define Y_SHIFT 35
    #endif

#endif

/**
 *Color of pen
 *If you want to use another color, you can choose one in RGB565 format.
 */

#define	WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define GRAY        0X8430
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define BROWN       0XBC40
#define BRRED       0XFC07
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458
#define	ORANGE 		0XE480
#define VIOLET		0X601F
#define LIGHTGREEN  0X841F
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

/* Control Registers and constant codes */
#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

/**
 * Memory Data Access Control Register (0x36H)
 * MAP:     D7  D6  D5  D4  D3  D2  D1  D0
 * param:   MY  MX  MV  ML  RGB MH  -   -
 *
 */

/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MY  0x80
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MX  0x40
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_MV  0x20
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_ML  0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1   0xDA
#define ST7789_RDID2   0xDB
#define ST7789_RDID3   0xDC
#define ST7789_RDID4   0xDD

/* Advanced options */
#define ST7789_COLOR_MODE_16bit 0x55    //  RGB565 (16bit)
#define ST7789_COLOR_MODE_18bit 0x66    //  RGB666 (18bit)

#define ABS(x) ((x) > 0 ? (x) : -(x))


typedef enum
{
	TXDMAM_STATIC_16			= 0,
	TXDMAM_DYNAMIC_8

}	qeTxDMA_Mode_t;


typedef struct
{
    ui16		x_start,
				x_end,
				y;

    const char	*str;

    tFont		*font;

    ui16		color,
				bgColor;

    ui16		total_width;
    ui16		scroll;
    ui8			str_len;

    ui16		char_widths [256];

}	ScrollTextState;


typedef struct
{
	ui16	*buffer;
    ui16	buf_width;
    ui16	height;

}	RenderedText;


class qcST7789 : virtual public qcPeripheral
{
	private :

		qeTxDMA_Mode_t		SPI_TxDMA_Mode;

		void				Reset (void) { HAL_GPIO_WritePin(LCD_SPI_RES_GPIO_PORT, LCD_SPI_RES_PIN, GPIO_PIN_RESET); }
		void				Unreset (void) { HAL_GPIO_WritePin(LCD_SPI_RES_GPIO_PORT, LCD_SPI_RES_PIN, GPIO_PIN_SET); }
		void				Command (void) { HAL_GPIO_WritePin(LCD_SPI_DC_GPIO_PORT, LCD_SPI_DC_PIN, GPIO_PIN_RESET); }
		void				Data (void) { HAL_GPIO_WritePin(LCD_SPI_DC_GPIO_PORT, LCD_SPI_DC_PIN, GPIO_PIN_SET); }
		void				Select (void) { HAL_GPIO_WritePin(LCD_SPI_CS_GPIO_PORT, LCD_SPI_CS_PIN, GPIO_PIN_RESET); }
		void				Unselect (void) { HAL_GPIO_WritePin(LCD_SPI_CS_GPIO_PORT, LCD_SPI_CS_PIN, GPIO_PIN_SET); }

		void				DriverInit (void);

		void				SPI_Transmit (ui8 *pData, ui16 size);
		void				SPI_TransmitDMA (ui8 *pData, ui32 size);

		void				WriteCommand (ui8 cmd);
		void				WriteData (ui8 *buff, ui32 size);
		void				WriteData16Bit (ui16 buff, ui32 size);
		void				WriteSmallData (ui8 data);

		void				SetAddressWindow (ui16 x0, ui16 y0, ui16 x1, ui16 y1);

		void				SetRotation (ui8 m);

		void				Translation (char *s, char *output, ui8 length);
		ui16				Paint (ui16 color, ui16 bgColor, ui8 alpha);

		void				DrawPixel (ui16 x, ui16 y, ui16 color);

	public :

		volatile bool		SPI_TxDMA_EOT;

		void				ST7789_Init (void);

		void				ST7789_InvertColors (ui8 invert);

		void				ST7789_DrawPixel (ui16 x, ui16 y, ui16 color);
		void				ST7789_FillColor (ui16 color);
		void				ST7789_DrawLine (ui16 x1, ui16 y1, ui16 x2, ui16 y2, ui16 color);
		void				ST7789_DrawRectangle(ui16 x1, ui16 y1, ui16 x2, ui16 y2, ui16 thick, ui16 color);
		void				ST7789_DrawFilledRectangle(ui16 x, ui16 y, ui16 w, ui16 h, ui16 color);
		void				ST7789_DrawCircle (ui16 x0, ui16 y0, ui8 r, ui16 color);
		void				ST7789_DrawFilledCircle (i16 x0, i16 y0, i16 r, ui16 color);
		void				ST7789_DrawTriangle (ui16 x1, ui16 y1, ui16 x2, ui16 y2, ui16 x3, ui16 y3, ui16 color);
		void				ST7789_DrawFilledTriangle (ui16 x1, ui16 y1, ui16 x2, ui16 y2, ui16 x3, ui16 y3, ui16 color);

		void				ST7789_WriteChar (ui16 x, ui16 y, const tImage* img, ui16 color, ui16 bgColor);
		void				ST7789_WriteString (ui16 x, ui16 y, const char *str, tFont *font, ui16 color, ui16 bgColor);


		void				ST7789_DrawImage (ui16 x, ui16 y, ui16 w, ui16 h, ui16 *data);
		void				ST7789_DrawPartofImage (ui16 x, ui16 y, ui16 w, ui16 h, ui16 *data);

		ui32				ST7789_TestFPS (void);
		void				ST7789_Rainbow (void);

		/* auxiliary functions for char */
		uint32_t			utf8_merge_code (const char **ptr);
		const tChar*		find_char (const tFont *font, int32_t code);

};


#ifndef ST7789_ROTATION
    #error You should at least choose a display rotation!
#endif

#endif	/* _ST7789_H_ */
