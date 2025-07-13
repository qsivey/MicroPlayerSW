/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#include "st7789.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qcST7789::DriverInit (void)
{
	qmDelayMs(500);

	LCD_SPI_DMA_CLOCK_ENABLE();

	HAL_NVIC_SetPriority(LCD_SPI_TX_DMA_CH_IRQN, 0, 0);
	HAL_NVIC_EnableIRQ(LCD_SPI_TX_DMA_CH_IRQN);

	__HAL_RCC_TIM2_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;

	/* Common configuration for all channels */
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_NOPULL;
	GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate	= LCD_TIM_CH_PIN_AF;
	GPIO_InitStruct.Pin			= LCD_TIM_CH_PIN;
	HAL_GPIO_Init(LCD_TIM_CH_GPIO_PORT, &GPIO_InitStruct);

	LCD_TIM_HANDLE.Instance = LCD_TIM_INSTANCE;

	LCD_TIM_HANDLE.Init.Prescaler         	= (qcfgPROJ_MCU_FREQUENCY / 1000000) - 1;
	LCD_TIM_HANDLE.Init.Period            	= 1000;
	LCD_TIM_HANDLE.Init.ClockDivision     	= TIM_CLOCKDIVISION_DIV1;
	LCD_TIM_HANDLE.Init.CounterMode       	= TIM_COUNTERMODE_UP;
	LCD_TIM_HANDLE.Init.RepetitionCounter 	= 0;
	LCD_TIM_HANDLE.Init.AutoReloadPreload 	= TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_PWM_Init(&LCD_TIM_HANDLE) != HAL_OK)
	{
		/* Initialization Error */
		HardwareErrorHandler();
	}

	TIM_OC_InitTypeDef sConfigOC;

	/* Common configuration for all channels */
	sConfigOC.OCMode       = TIM_OCMODE_PWM1;
	sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;

	/* Set the pulse value for PWM channel */
	sConfigOC.Pulse = 500;

	if (HAL_TIM_PWM_ConfigChannel(&LCD_TIM_HANDLE, &sConfigOC, LCD_TIM_CHANNEL) != HAL_OK)
		/* Configuration Error */
		HardwareErrorHandler();

	/* Start PWM channel */
	if (HAL_TIM_PWM_Start(&LCD_TIM_HANDLE, LCD_TIM_CHANNEL) != HAL_OK)
		/* PWM Generation Error */
		HardwareErrorHandler();

	LCD_SPI_CLOCK_ENABLE();

	/* SPI GPIO Initializing */
	GPIO_InitStruct.Pin			= LCD_SPI_MOSI_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_NOPULL;
	GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= LCD_SPI_MOSI_PIN_AF;
	HAL_GPIO_Init(LCD_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin			= LCD_SPI_SCL_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_NOPULL;
	GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= LCD_SPI_SCL_PIN_AF;
	HAL_GPIO_Init(LCD_SPI_SCL_GPIO_PORT, &GPIO_InitStruct);

	/* Other GPIO Initializing */
	GPIO_InitStruct.Pin			= LCD_SPI_RES_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull		= GPIO_NOPULL;
	GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(LCD_SPI_RES_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_SPI_CS_PIN;
	HAL_GPIO_Init(LCD_SPI_CS_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_SPI_DC_PIN;
	HAL_GPIO_Init(LCD_SPI_DC_GPIO_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(LCD_SPI_RES_GPIO_PORT, LCD_SPI_RES_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_SPI_CS_GPIO_PORT, LCD_SPI_CS_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_SPI_DC_GPIO_PORT, LCD_SPI_DC_PIN, GPIO_PIN_RESET);

	/* DMA Init */
	LCD_SPI_TX_DMA_HANDLE.Instance						= LCD_SPI_TX_DMA_INSTANCE;
	LCD_SPI_TX_DMA_HANDLE.Init.Request					= LCD_SPI_TX_DMA_REQUEST;
	LCD_SPI_TX_DMA_HANDLE.Init.BlkHWRequest				= DMA_BREQ_SINGLE_BURST;
	LCD_SPI_TX_DMA_HANDLE.Init.Direction				= DMA_PERIPH_TO_MEMORY;
	LCD_SPI_TX_DMA_HANDLE.Init.SrcInc					= DMA_SINC_INCREMENTED;
	LCD_SPI_TX_DMA_HANDLE.Init.DestInc					= DMA_DINC_FIXED;
	LCD_SPI_TX_DMA_HANDLE.Init.SrcDataWidth				= DMA_SRC_DATAWIDTH_BYTE;
	LCD_SPI_TX_DMA_HANDLE.Init.DestDataWidth			= DMA_DEST_DATAWIDTH_BYTE;
	LCD_SPI_TX_DMA_HANDLE.Init.Priority					= DMA_LOW_PRIORITY_LOW_WEIGHT;
	LCD_SPI_TX_DMA_HANDLE.Init.SrcBurstLength			= 1;
	LCD_SPI_TX_DMA_HANDLE.Init.DestBurstLength			= 1;
	LCD_SPI_TX_DMA_HANDLE.Init.TransferAllocatedPort	= DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
	LCD_SPI_TX_DMA_HANDLE.Init.TransferEventMode		= DMA_TCEM_BLOCK_TRANSFER;
	LCD_SPI_TX_DMA_HANDLE.Init.Mode						= DMA_NORMAL;

	if (HAL_DMA_Init(&LCD_SPI_TX_DMA_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	__HAL_LINKDMA(&LCD_SPI_HANDLE, hdmatx, LCD_SPI_TX_DMA_HANDLE);

	if (HAL_DMA_ConfigChannelAttributes(&LCD_SPI_TX_DMA_HANDLE, DMA_CHANNEL_NPRIV) != HAL_OK)
		HardwareErrorHandler();

	HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(SPI3_IRQn);

	/* SPI parameter configuration */
	LCD_SPI_HANDLE.Instance						= LCD_SPI_INSTANCE;
	LCD_SPI_HANDLE.Init.Mode					= SPI_MODE_MASTER;
	LCD_SPI_HANDLE.Init.Direction				= SPI_DIRECTION_1LINE;
	LCD_SPI_HANDLE.Init.DataSize				= SPI_DATASIZE_8BIT;
	LCD_SPI_HANDLE.Init.CLKPolarity				= SPI_POLARITY_LOW;
	LCD_SPI_HANDLE.Init.CLKPhase				= SPI_PHASE_1EDGE;
	LCD_SPI_HANDLE.Init.NSS						= SPI_NSS_SOFT;
	LCD_SPI_HANDLE.Init.BaudRatePrescaler		= SPI_BAUDRATEPRESCALER_2;
	LCD_SPI_HANDLE.Init.FirstBit				= SPI_FIRSTBIT_MSB;
	LCD_SPI_HANDLE.Init.TIMode					= SPI_TIMODE_DISABLE;
	LCD_SPI_HANDLE.Init.CRCCalculation			= SPI_CRCCALCULATION_DISABLE;
	LCD_SPI_HANDLE.Init.CRCPolynomial			= 0x7;
	LCD_SPI_HANDLE.Init.NSSPMode				= SPI_NSS_PULSE_ENABLE;
	LCD_SPI_HANDLE.Init.NSSPolarity				= SPI_NSS_POLARITY_LOW;
	LCD_SPI_HANDLE.Init.FifoThreshold			= SPI_FIFO_THRESHOLD_01DATA;
	LCD_SPI_HANDLE.Init.MasterSSIdleness		= SPI_MASTER_SS_IDLENESS_00CYCLE;
	LCD_SPI_HANDLE.Init.MasterInterDataIdleness	= SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	LCD_SPI_HANDLE.Init.MasterReceiverAutoSusp	= SPI_MASTER_RX_AUTOSUSP_DISABLE;
	LCD_SPI_HANDLE.Init.MasterKeepIOState		= SPI_MASTER_KEEP_IO_STATE_DISABLE;
	LCD_SPI_HANDLE.Init.IOSwap					= SPI_IO_SWAP_DISABLE;
	LCD_SPI_HANDLE.Init.ReadyMasterManagement	= SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
	LCD_SPI_HANDLE.Init.ReadyPolarity			= SPI_RDY_POLARITY_HIGH;

	if (HAL_SPI_Init(&LCD_SPI_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	SPI_TxDMA_Mode = TXDMAM_DYNAMIC_8;
}


void qcST7789::SPI_Transmit (ui8 *pData, ui16 size)
{
	Select();

	HAL_SPI_Transmit(&LCD_SPI_HANDLE, pData, size, 100);

	Unselect();
}


void qcST7789::SPI_TransmitDMA (ui8 *pData, ui32 size)
{
	Select();

	ui16 SPI_MaxData = (SPI_TxDMA_Mode == TXDMAM_STATIC_16) ? 0x7FFF : UINT16_MAX;

	SPI_TxDMA_EOT = false;

	if (size <= SPI_MaxData)
		HAL_SPI_Transmit_DMA(&LCD_SPI_HANDLE, pData, size);

	else
	{
		while (1)
		{
			HAL_SPI_Transmit_DMA(&LCD_SPI_HANDLE, pData, (size <= SPI_MaxData) ? size : SPI_MaxData);

			while (!SPI_TxDMA_EOT);

			if (size > SPI_MaxData)
				size -= SPI_MaxData;

			else
				break;

			SPI_TxDMA_EOT = false;

			if (SPI_TxDMA_Mode == TXDMAM_DYNAMIC_8)
				pData += SPI_MaxData;
		}
	}

	while (!SPI_TxDMA_EOT);

	Unselect();
}


void qcST7789::WriteCommand (ui8 cmd)
{
	Command();
	SPI_Transmit(&cmd, sizeof(cmd));
}


void qcST7789::WriteData (ui8 *buff, ui32 size)
{
	Data();
	SPI_TransmitDMA(buff, size);
}


void qcST7789::WriteSmallData (ui8 data)
{
	Data();
	SPI_Transmit(&data, sizeof(data));
}


void qcST7789::SetAddressWindow (ui16 x0, ui16 y0, ui16 x1, ui16 y1)
{
	ui16 x_start = x0 + X_SHIFT, x_end = x1 + X_SHIFT;
	ui16 y_start = y0 + Y_SHIFT, y_end = y1 + Y_SHIFT;

	/* Column Address set */
	WriteCommand(ST7789_CASET);
	{
		if (SPI_TxDMA_Mode == TXDMAM_DYNAMIC_8)
		{
			ui8 data [] = { (ui8)(x_start >> 8), (ui8)(x_start & 0xFF), (ui8)(x_end >> 8), (ui8)(x_end & 0xFF) };
			WriteData(data, sizeof(data));
		}

		else
		{
			ui8 data1 [] = { (ui8)(x_start & 0xFF), (ui8)(x_start >> 8) };
			WriteData(data1, 1);
			ui8 data2 [] = { (ui8)(x_end & 0xFF), (ui8)(x_end >> 8)};
			WriteData(data2, 1);
		}
	}

	/* Row Address set */
	WriteCommand(ST7789_RASET);
	{
		if (SPI_TxDMA_Mode == TXDMAM_DYNAMIC_8)
		{
			ui8 data [] = { (ui8)(y_start >> 8), (ui8)(y_start & 0xFF), (ui8)(y_end >> 8), (ui8)(y_end & 0xFF) };
			WriteData(data, sizeof(data));
		}

		else
		{
			ui8 data1 [] = { (ui8)(y_start & 0xFF), (ui8)(y_start >> 8) };
			WriteData(data1, 1);
			ui8 data2 [] = { (ui8)(y_end & 0xFF), (ui8)(y_end >> 8) };
			WriteData(data2, 1);
		}
	}

	/* Write to RAM */
	WriteCommand(ST7789_RAMWR);
}


void qcST7789::SetRotation (ui8 m)
{
	displayCheckTxMode(TXDMAM_DYNAMIC_8)

	WriteCommand(ST7789_MADCTL);

	switch (m)
	{
		case 0:
			WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
			break;
		case 1:
			WriteSmallData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
			break;
		case 2:
			WriteSmallData(ST7789_MADCTL_RGB);
			break;
		case 3:
			WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
			break;
		default:
			break;
	}
}


void qcST7789::Translation (char *s, char *output, ui8 length)
{
		ui8 x = 160;
		/* Where x - offset to Russian symbols */

		size_t len = strlen(s);

		if (len > length) return;
		if (!length) return;

	 	ui8 len_in=(len/2)+1;
	 	ui8 k=0;

	 	char index[len_in];

		for (ui8 i = 0; i < len; i++)
		{
			/* English */
			if (s[i] < 128){
			  index[k] = s[i];
			  k++;
			}
			/* Russian */
			else if (s[i] == 208)
			{
			  /* Except Ё or other letter with 208 */
			  if (s[i + 1] == 129) index[k] = 127; //Ё
			  else index[k] = x + (s[i + 1] - 176);
			  k++;
			  i++;
			}
			/* Except ё or other letter with 209 */
			else if (s[i] == 209){
				if (s[i + 1] == 145) index[k] = 192; //ё
				else index[k] = x + (s[i + 1] - 112);
				k++;
				i++;
			}
			/* Except № */
			else if (s[i] == 226){
				index[k] = 193; //№
				k++;
				i+=2;
			}
		}
		index[(len/2)] = 0;

		for (int i=0;i<len_in;i++){
			output[i] = index[i];
		}
}


ui16 qcST7789::Paint (ui16 color, ui16 bgColor, ui8 alpha)
{
	ui8 r = (color >> 11) & 0x1F;
	ui8 g = (color >> 5) & 0x3F;
	ui8 b = color & 0x1F;

	ui8 bgr = (bgColor >> 11) & 0x1F;
	ui8 bgg = (bgColor >> 5) & 0x3F;
	ui8 bgb = bgColor & 0x1F;

	r =((r*alpha)+(bgr*(63-alpha)))/63;
	g =((g*alpha)+(bgg*(63-alpha)))/63;
	b =((b*alpha)+(bgb*(63-alpha)))/63;

	return (r << 11) | (g << 5) | b;
}


void qcST7789::ST7789_Init (void)
{
	DriverInit();

	qmDelayMs(20);
    Reset();
    qmDelayMs(20);
    Unreset();
    qmDelayMs(60);

    WriteCommand(ST7789_COLMOD);	//	Set color mode
    WriteSmallData(ST7789_COLOR_MODE_16bit);
  	WriteCommand(0xB2);				//	Porch control
	{
		ui8 data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};

		for (ui32 i = 0; i < sizeof(data); i++)
			WriteSmallData(data[i]);
	}

	SetRotation(ST7789_ROTATION);	//	MADCTL (Display Rotation)

	/* Internal LCD Voltage generator settings */
    WriteCommand(0XB7);				//	Gate Control
    WriteSmallData(0x35);			//	Default value
    WriteCommand(0xBB);				//	VCOM setting
    WriteSmallData(0x19);			//	0.725v (default 0.75v for 0x20)
    WriteCommand(0xC0);				//	LCMCTRL
    WriteSmallData (0x2C);			//	Default value
    WriteCommand (0xC2);				//	VDV and VRH command Enable
    WriteSmallData (0x01);			//	Default value
    WriteCommand (0xC3);				//	VRH set
    WriteSmallData (0x12);			//	+-4.45v (defalut +-4.1v for 0x0B)
    WriteCommand (0xC4);				//	VDV set
    WriteSmallData (0x20);			//	Default value
    WriteCommand (0xC6);				//	Frame rate control in normal mode
    WriteSmallData (0x0F);			//	Default value (60HZ)
    WriteCommand (0xD0);				//	Power control
    WriteSmallData (0xA4);			//	Default value
    WriteSmallData (0xA1);			//	Default value
	/**************** Division line ****************/

	WriteCommand(0xE0);
	{
		ui8 data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};

		for (ui32 i = 0; i < sizeof(data); i++)
			WriteSmallData(data[i]);
	}

    WriteCommand(0xE1);
	{
		ui8 data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};

		for (ui32 i = 0; i < sizeof(data); i++)
			WriteSmallData(data[i]);
	}

    WriteCommand (ST7789_INVON);		//	Inversion ON
	WriteCommand (ST7789_SLPOUT);	//	Out of sleep mode
  	WriteCommand (ST7789_NORON);		//	Normal Display on
  	WriteCommand (ST7789_DISPON);	//	Main screen turned on

	qmDelayMs(50);

	ST7789_FillColor(BLACK);

	ST7789_WriteString(2, 2, (char*)"Hello, qsivey!", &Font20, GREEN, BLACK);
	ST7789_WriteString(2, 30, (char*)"Display is ready...", &Font20, GREEN, BLACK);

//	ui16 FPS = ST7789_TestFPS();
//
//	ST7789_FillColor(BLACK);
//
//	char str [15];
//
//	sprintf(str, "FPS: %d", FPS);
//
//	ST7789_WriteString(2, 2, str, &Font20, GREEN, BLACK);
//	qmDelayMs(3000);
}


void qcST7789::ST7789_FillColor (ui16 color)
{
	SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);

	displayCheckTxMode(TXDMAM_STATIC_16)

	ui8 color8 [] = { (ui8)(color & 0xFF), (ui8)(color >> 8) };

	WriteData(color8, ST7789_WIDTH * ST7789_HEIGHT);
}


void qcST7789::ST7789_DrawPixel (ui16 x, ui16 y, ui16 color)
{
	displayCheckTxMode(TXDMAM_STATIC_16)

	SetAddressWindow(x, y, x, y);

	WriteData((ui8*)&color, 1);
}


void qcST7789::ST7789_DrawLine (ui16 x0, ui16 y0, ui16 x1, ui16 y1, ui16 color)
{
	ui16 swap;
    ui16 steep = ABS(y1 - y0) > ABS(x1 - x0);
    if (steep) {
		swap = x0;
		x0 = y0;
		y0 = swap;

		swap = x1;
		x1 = y1;
		y1 = swap;
    }

    if (x0 > x1) {
		swap = x0;
		x0 = x1;
		x1 = swap;

		swap = y0;
		y0 = y1;
		y1 = swap;
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = ABS(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            ST7789_DrawPixel(y0, x0, color);
        } else {
            ST7789_DrawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}


void qcST7789::ST7789_DrawRectangle (ui16 x1, ui16 y1, ui16 x2, ui16 y2, ui16 thick, ui16 color)
{
	if (x1 == x2 || y1 == y2) return;

	int8_t x_dir = 1, y_dir = 1;

	if (x1 > x2) x_dir = -1;
	if (y1 > y2) y_dir = -1;

	for (int i = 0; i < thick; i++)
	{
		int16_t x_offset = (i * x_dir), y_offset = (i * y_dir);

		ST7789_DrawLine(x1 + x_offset, y1 + y_offset, x2 - x_offset, y1 + y_offset, color);
		ST7789_DrawLine(x1 + x_offset, y1 + y_offset, x1 + x_offset, y2 - y_offset, color);
		ST7789_DrawLine(x1 + x_offset, y2 - y_offset, x2 - x_offset, y2  - y_offset, color);
		ST7789_DrawLine(x2 - x_offset, y1 + y_offset, x2 - x_offset, y2 - y_offset, color);
	}
}


void qcST7789::ST7789_DrawCircle (ui16 x0, ui16 y0, ui8 r, ui16 color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawPixel(x0 + x, y0 + y, color);
		ST7789_DrawPixel(x0 - x, y0 + y, color);
		ST7789_DrawPixel(x0 + x, y0 - y, color);
		ST7789_DrawPixel(x0 - x, y0 - y, color);

		ST7789_DrawPixel(x0 + y, y0 + x, color);
		ST7789_DrawPixel(x0 - y, y0 + x, color);
		ST7789_DrawPixel(x0 + y, y0 - x, color);
		ST7789_DrawPixel(x0 - y, y0 - x, color);
	}
}


void qcST7789::ST7789_DrawImage (ui16 x, ui16 y, ui16 w, ui16 h, ui16 *data)
{
	displayAssert(x, y);

	if ((x + w - 1) >= ST7789_WIDTH)
		return;

	if ((y + h - 1) >= ST7789_HEIGHT)
		return;

	displayCheckTxMode(TXDMAM_DYNAMIC_8)

	SetAddressWindow(x, y, x + w - 1, y + h - 1);

	WriteData((ui8*)data, w * h * 2);
}


void qcST7789::ST7789_InvertColors (ui8 invert)
{
	displayCheckTxMode(TXDMAM_DYNAMIC_8)

	WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
}


void qcST7789::ST7789_WriteChar (ui16 x, ui16 y, char ch, tFont *font, ui16 color, ui16 bgColor)
{
	displayCheckTxMode(TXDMAM_STATIC_16)

	SetAddressWindow(x, y, x + font->chars[(ch-32)].image->width - 1, y + font->chars[(ch-32)].image->height - 1);

	int a = 0;
	bool even = false;
	ui16 chrome, pixel;

	for (int i = 0; i < font->chars[(ch-32)].image->height; i++)
		for (int k = 0; k < font->chars[(ch-32)].image->width; k++)
		{
			if (even)
				chrome = (font->chars[(ch-32)].image->data[a] & 0x0F);

			else
				chrome = (font->chars[(ch-32)].image->data[a] >> 4) & 0x0F;

			ui8 alpha = chrome ? ((chrome + 1) * 4) - 1 : 0;

			if (alpha > 0)
			{
				pixel = Paint(bgColor, color, alpha);
				WriteData((ui8*)&pixel, 1);
			}

			else
				WriteData((ui8*)&color, 1);

			if (even)
				a++;

			even = !even;
		}
}


void qcST7789::ST7789_WriteString (ui16 x, ui16 y, char *str, tFont *font, ui16 color, ui16 bgColor)
{
	displayCheckTxMode(TXDMAM_STATIC_16)

	while (*str) {
		if (x + font->chars[(*str-32)].image->width >= ST7789_WIDTH) {
			x = 0;
			y += font->chars[(*str-32)].image->height;
			if (y + font->chars[(*str-32)].image->height >= ST7789_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}

		ST7789_WriteChar(x, y, *str, font, color, bgColor);
		x += font->chars[(*str-32)].image->width;
		str++;
	}
}


void qcST7789::ST7789_DrawFilledRectangle (ui16 x, ui16 y, ui16 w, ui16 h, ui16 color)
{
	displayAssert(x, y);

	displayCheckTxMode(TXDMAM_STATIC_16)

	SetAddressWindow(x, y, x + w - 1, y + h - 1);

	ui8 color8 [] = { (ui8)(color >> 8), (ui8)(color & 0xFF) };

	WriteData(color8, w * h);
}


void qcST7789::ST7789_DrawTriangle (ui16 x1, ui16 y1, ui16 x2, ui16 y2, ui16 x3, ui16 y3, ui16 color)
{
	/* Draw lines */
	ST7789_DrawLine(x1, y1, x2, y2, color);
	ST7789_DrawLine(x2, y2, x3, y3, color);
	ST7789_DrawLine(x3, y3, x1, y1, color);
}


void qcST7789::ST7789_DrawFilledTriangle (ui16 x1, ui16 y1, ui16 x2, ui16 y2, ui16 x3, ui16 y3, ui16 color)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	}
	else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	}
	else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) {
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	}
	else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ST7789_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}


void qcST7789::ST7789_DrawFilledCircle (i16 x0, i16 y0, i16 r, ui16 color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);
	ST7789_DrawLine(x0 - r, y0, x0 + r, y0, color);

	while (x < y)
	{
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		ST7789_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

		ST7789_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		ST7789_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
}


ui32 qcST7789::ST7789_TestFPS (void)
{
	ui32 tick = qmGetTick();
	ui32 FPS = 0;

	while (qmGetTick() - tick < 1000)
	{
		ST7789_FillColor(rand() % 65536);
		FPS++;
	}

	return FPS;
}
