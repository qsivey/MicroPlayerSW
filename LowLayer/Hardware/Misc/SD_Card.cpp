/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#include "SD_Card.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qcSD_Card::SD_Init (void)
{
	Stat = STA_NOINIT;
	PowerFlag = 0;

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	SD_SPI_GPIO_CLOCK_ENABLE();
	SD_GPIO_CLOCK_ENABLE();

	GPIO_InitStruct.Pin = SD_SPI_MOSI_PIN | SD_SPI_MISO_PIN | SD_SPI_SCK_PIN ;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SD_CS_PIN ;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStruct);

	SD_SPI_CLOCK_ENABLE();

	SD_SPI_HANDLE.Instance = SPI3;
	SD_SPI_HANDLE.Init.Mode = SPI_MODE_MASTER;
	SD_SPI_HANDLE.Init.Direction = SPI_DIRECTION_2LINES;
	SD_SPI_HANDLE.Init.DataSize = SPI_DATASIZE_8BIT;
	SD_SPI_HANDLE.Init.CLKPolarity = SPI_POLARITY_LOW;
	SD_SPI_HANDLE.Init.CLKPhase = SPI_PHASE_1EDGE;
	SD_SPI_HANDLE.Init.NSS = SPI_NSS_SOFT;
	SD_SPI_HANDLE.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	SD_SPI_HANDLE.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SD_SPI_HANDLE.Init.TIMode = SPI_TIMODE_DISABLE;
	SD_SPI_HANDLE.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;

	if (HAL_SPI_Init(&SD_SPI_HANDLE) != HAL_OK)
		HardwareErrorHandler();
}


void qcSD_Card::Select (void)
{
	HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET);
	qmCustomDelay(100);
}


void qcSD_Card::Deselect (void)
{
	HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET);
	qmCustomDelay(100);
}


void qcSD_Card::TxByte (ui8 data)
{
	while(!__HAL_SPI_GET_FLAG(&SD_SPI_HANDLE, SPI_FLAG_TXE));
	HAL_SPI_Transmit(&SD_SPI_HANDLE, &data, 1, qcfgHAL_TIMEOUT_DELAY);
}


void qcSD_Card::TxBuffer (ui8 *buffer, ui16 len)
{
	while(!__HAL_SPI_GET_FLAG(&SD_SPI_HANDLE, SPI_FLAG_TXE));
	HAL_SPI_Transmit(&SD_SPI_HANDLE, buffer, len, qcfgHAL_TIMEOUT_DELAY);
}


ui8 qcSD_Card::RxByte (void)
{
	ui8 dummy, data;
	dummy = 0xFF;

	while(!__HAL_SPI_GET_FLAG(&SD_SPI_HANDLE, SPI_FLAG_TXE));
	HAL_SPI_TransmitReceive(&SD_SPI_HANDLE, &dummy, &data, 1, qcfgHAL_TIMEOUT_DELAY);

	return data;
}


ui8 qcSD_Card::ReadyWait (void)
{
	uint8_t res;

	/* Timeout 500 ms */
	Timer2 = 500;

	/* If SD goes ready, receives 0xFF */
	do {
		res = RxByte();
	} while ((res != 0xFF) && Timer2);

	return res;
}


void qcSD_Card::PowerOn (void)
{
	ui8 args [6];
	ui32 cnt = 0x1FFF;

	/* Transmit bytes to wake up */
	Deselect();

	for (int i = 0; i < 10; i++)
		TxByte(0xFF);

	/* Slave select */
	Select();

	/* Make idle state */
	args[0] = CMD0;		/* CMD0:GO_IDLE_STATE */
	args[1] = 0;
	args[2] = 0;
	args[3] = 0;
	args[4] = 0;
	args[5] = 0x95;		/* CRC */

	TxBuffer(args, sizeof(args));

	/* Wait response */
	while ((RxByte() != 0x01) && cnt)
		cnt--;

	Deselect();
	TxByte(0XFF);

	PowerFlag = 1;
}


bool qcSD_Card::RxDataBlock (BYTE *buff, UINT len)
{
	ui8 token;

	/* Timeout 200 ms */
	Timer1 = 200;

	/* Loop until receive a response or timeout */
	do {
		token = RxByte();
	} while ((token == 0xFF) && Timer1);

	/* Invalid response */
	if (token != 0xFE) return false;

	ui8 *dummy = (ui8*)qmMalloc(len);
	memset(dummy, 0xFF, len);

	/* Receive data */
	while(!__HAL_SPI_GET_FLAG(&SD_SPI_HANDLE, SPI_FLAG_TXE));
	HAL_SPI_TransmitReceive(&SD_SPI_HANDLE, dummy, buff, len, qcfgHAL_TIMEOUT_DELAY);

	qmFree(dummy);

	/* Discard CRC */
	RxByte();
	RxByte();

	return true;
}


bool qcSD_Card::TxDataBlock (const ui8 *buff, BYTE token)
{
	ui8 resp = 0;
	ui8 i = 0;

	/* Wait SD ready */
	if (ReadyWait() != 0xFF) return false;

	/* Transmit token */
	TxByte(token);

	/* If it's not STOP token, transmit data */
	if (token != 0xFD)
	{
		TxBuffer((uint8_t*)buff, 512);

		/* Discard CRC */
		RxByte();
		RxByte();

		/* Receive response */
		while (i <= 64)
		{
			resp = RxByte();

			/* transmit 0x05 accepted */
			if ((resp & 0x1F) == 0x05) break;
			i++;
		}

		/* Receive buffer clear */
		while (RxByte() == 0);
	}

	/* transmit 0x05 accepted */
	if ((resp & 0x1F) == 0x05) return true;

	return false;
}


BYTE qcSD_Card::SendCmd (BYTE cmd, ui32 arg)
{
	ui8 crc, res;

	/* Wait SD ready */
	if (ReadyWait() != 0xFF) return 0xFF;

	/* Transmit command */
	TxByte(cmd); 				/* Command */
	TxByte((ui8)(arg >> 24)); 	/* Argument[31..24] */
	TxByte((ui8)(arg >> 16)); 	/* Argument[23..16] */
	TxByte((ui8)(arg >> 8)); 	/* Argument[15..8] */
	TxByte((ui8)arg); 			/* Argument[7..0] */

	/* Prepare CRC */
	if (cmd == CMD0) crc = 0x95;		/* CRC for CMD0(0) */
	else if (cmd == CMD8) crc = 0x87;	/* CRC for CMD8(0x1AA) */
	else crc = 1;

	/* Transmit CRC */
	TxByte(crc);

	/* Skip a stuff byte when STOP_TRANSMISSION */
	if (cmd == CMD12)
		RxByte();

	/* Receive response */
	uint8_t n = 10;
	do {
		res = RxByte();
	} while ((res & 0x80) && --n);

	return res;
}


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *												   Glue DiskIO Functions
 */
static qcSD_Card SD_Card;


/* initialize SD */
DSTATUS SD_disk_initialize (BYTE drv)
{
	uint8_t n, type, ocr[4];

	SD_Card.SD_Init();

	/* single drive, drv should be 0 */
	if(drv) return STA_NOINIT;

	/* no disk */
	if(SD_Card.Stat & STA_NODISK) return SD_Card.Stat;

	/* power on */
	SD_Card.PowerOn();

	/* slave select */
	SD_Card.Select();

	/* check disk type */
	type = 0;

	/* send GO_IDLE_STATE command */
	if (SD_Card.SendCmd(CMD0, 0) == 1)
	{
		/* timeout 1 sec */
		SD_Card.Timer1 = 1000;

		/* SDC V2+ accept CMD8 command, http://elm-chan.org/docs/mmc/mmc_e.html */
		if (SD_Card.SendCmd(CMD8, 0x1AA) == 1)
		{
			/* operation condition register */
			for (n = 0; n < 4; n++)
			{
				ocr[n] = SD_Card.RxByte();
			}

			/* voltage range 2.7-3.6V */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA)
			{
				/* ACMD41 with HCS bit */
				do {
					if (SD_Card.SendCmd(CMD55, 0) <= 1 && SD_Card.SendCmd(CMD41, 1UL << 30) == 0) break;
				} while (SD_Card.Timer1);

				/* READ_OCR */
				if (SD_Card.Timer1 && SD_Card.SendCmd(CMD58, 0) == 0)
				{
					/* Check CCS bit */
					for (n = 0; n < 4; n++)
					{
						ocr[n] = SD_Card.RxByte();
					}

					/* SDv2 (HC or SC) */
					type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				}
			}
		}
		else
		{
			/* SDC V1 or MMC */
			type = (SD_Card.SendCmd(CMD55, 0) <= 1 && SD_Card.SendCmd(CMD41, 0) <= 1) ? CT_SD1 : CT_MMC;

			do
			{
				if (type == CT_SD1)
				{
					if (SD_Card.SendCmd(CMD55, 0) <= 1 && SD_Card.SendCmd(CMD41, 0) == 0) break; /* ACMD41 */
				}
				else
				{
					if (SD_Card.SendCmd(CMD1, 0) == 0) break; /* CMD1 */
				}

			} while (SD_Card.Timer1);

			/* SET_BLOCKLEN */
			if (!SD_Card.Timer1 || SD_Card.SendCmd(CMD16, 512) != 0) type = 0;
		}
	}

	SD_Card.CardType = type;

	/* Idle */
	SD_Card.Deselect();
	SD_Card.RxByte();

	/* Clear STA_NOINIT */
	if (type)
	{
		SD_Card.Stat &= ~STA_NOINIT;
	}
	else
	{
		/* Initialization failed */
		SD_Card.PowerOff();
	}

	return SD_Card.Stat;
}


/* return disk status */
DSTATUS SD_disk_status (BYTE drv)
{
	if (drv) return STA_NOINIT;
	return SD_Card.Stat;
}


/* read sector */
DRESULT SD_disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
	/* pdrv should be 0 */
	if (pdrv || !count) return RES_PARERR;

	/* no disk */
	if (SD_Card.Stat & STA_NOINIT) return RES_NOTRDY;

	/* convert to byte address */
	if (!(SD_Card.CardType & CT_SD2)) sector *= 512;

	SD_Card.Select();

	if (count == 1)
	{
		/* READ_SINGLE_BLOCK */
		if ((SD_Card.SendCmd(CMD17, sector) == 0) && SD_Card.RxDataBlock(buff, 512)) count = 0;
	}
	else
	{
		/* READ_MULTIPLE_BLOCK */
		if (SD_Card.SendCmd(CMD18, sector) == 0)
		{
			do {
				if (!SD_Card.RxDataBlock(buff, 512)) break;
				buff += 512;
			} while (--count);

			/* STOP_TRANSMISSION */
			SD_Card.SendCmd(CMD12, 0);
		}
	}

	/* Idle */
	SD_Card.Deselect();
	SD_Card.RxByte();

	return count ? RES_ERROR : RES_OK;
}


/* write sector */
#if _USE_WRITE == 1

	DRESULT SD_disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
	{
		/* pdrv should be 0 */
		if (pdrv || !count) return RES_PARERR;

		/* no disk */
		if (SD_Card.Stat & STA_NOINIT) return RES_NOTRDY;

		/* write protection */
		if (SD_Card.Stat & STA_PROTECT) return RES_WRPRT;

		/* convert to byte address */
		if (!(SD_Card.CardType & CT_SD2)) sector *= 512;

		SD_Card.Select();

		if (count == 1)
		{
			/* WRITE_BLOCK */
			if ((SD_Card.SendCmd(CMD24, sector) == 0) && SD_Card.TxDataBlock(buff, 0xFE))
				count = 0;
		}
		else
		{
			/* WRITE_MULTIPLE_BLOCK */
			if (SD_Card.CardType & CT_SD1)
			{
				SD_Card.SendCmd(CMD55, 0);
				SD_Card.SendCmd(CMD23, count); /* ACMD23 */
			}

			if (SD_Card.SendCmd(CMD25, sector) == 0)
			{
				do {
					if(!SD_Card.TxDataBlock(buff, 0xFC)) break;
					buff += 512;
				} while (--count);

				/* STOP_TRAN token */
				if(!SD_Card.TxDataBlock(0, 0xFD))
				{
					count = 1;
				}
			}
		}

		/* Idle */
		SD_Card.Deselect();
		SD_Card.RxByte();

		return count ? RES_ERROR : RES_OK;
	}

#endif /* _USE_WRITE */

/* ioctl */
DRESULT SD_disk_ioctl (BYTE drv, BYTE ctrl, void *buff)
{
	DRESULT res;
	ui8 n, csd[16], *ptr = (ui8*)buff;
	WORD csize;

	/* pdrv should be 0 */
	if (drv) return RES_PARERR;
	res = RES_ERROR;

	if (ctrl == CTRL_POWER)
	{
		switch (*ptr)
		{
		case 0:
			SD_Card.PowerOff();		/* Power Off */
			res = RES_OK;
			break;
		case 1:
			SD_Card.PowerOn();		/* Power On */
			res = RES_OK;
			break;
		case 2:
			*(ptr + 1) = SD_Card.CheckPower();
			res = RES_OK;		/* Power Check */
			break;
		default:
			res = RES_PARERR;
		}
	}
	else
	{
		/* no disk */
		if (SD_Card.Stat & STA_NOINIT) return RES_NOTRDY;

		SD_Card.Select();

		switch (ctrl)
		{
		case GET_SECTOR_COUNT:
			/* SEND_CSD */
			if ((SD_Card.SendCmd(CMD9, 0) == 0) && SD_Card.RxDataBlock(csd, 16))
			{
				if ((csd[0] >> 6) == 1)
				{
					/* SDC V2 */
					csize = csd[9] + ((WORD) csd[8] << 8) + 1;
					*(DWORD*) buff = (DWORD) csize << 10;
				}
				else
				{
					/* MMC or SDC V1 */
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD) csd[7] << 2) + ((WORD) (csd[6] & 3) << 10) + 1;
					*(DWORD*) buff = (DWORD) csize << (n - 9);
				}
				res = RES_OK;
			}
			break;
		case GET_SECTOR_SIZE:
			*(WORD*) buff = 512;
			res = RES_OK;
			break;
		case CTRL_SYNC:
			if (SD_Card.ReadyWait() == 0xFF) res = RES_OK;
			break;
		case MMC_GET_CSD:
			/* SEND_CSD */
			if (SD_Card.SendCmd(CMD9, 0) == 0 && SD_Card.RxDataBlock(ptr, 16)) res = RES_OK;
			break;
		case MMC_GET_CID:
			/* SEND_CID */
			if (SD_Card.SendCmd(CMD10, 0) == 0 && SD_Card.RxDataBlock(ptr, 16)) res = RES_OK;
			break;
		case MMC_GET_OCR:
			/* READ_OCR */
			if (SD_Card.SendCmd(CMD58, 0) == 0)
			{
				for (n = 0; n < 4; n++)
				{
					*ptr++ = SD_Card.RxByte();
				}
				res = RES_OK;
			}
		default:
			res = RES_PARERR;
		}

		SD_Card.Deselect();
		SD_Card.RxByte();
	}

	return res;
}

#ifdef		__cplusplus
extern		"C" {
#endif
/*  = = = = = = = = = = = = = = = = = = = = = = = */

SD_disk_initialize_wrapper_t SD_disk_initialize_wrapper = &SD_disk_initialize;
SD_disk_status_wrapper_t SD_disk_status_wrapper = &SD_disk_status;
SD_disk_read_wrapper_t SD_disk_read_wrapper = &SD_disk_read;
SD_disk_write_wrapper_t SD_disk_write_wrapper = &SD_disk_write;
SD_disk_ioctl_wrapper_t SD_disk_ioctl_wrapper = &SD_disk_ioctl;

/*  = = = = = = = = = = = = = = = = = = = = = = = */
#ifdef		__cplusplus
			}
#endif
