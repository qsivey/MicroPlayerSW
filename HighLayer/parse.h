/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		HIGH_LAYER_PARSE_H_
#define		HIGH_LAYER_PARSE_H_

/*  = = = = = = = = = = = = = = = = = = = = = = = */


#include    "ST7789.h"

#include	"ff.h"
#include	"ff_gen_drv.h"

#include	"printf.h"
#include	"tjpgd.h"

#define		MAX_FOLDERS				20
#define		MAX_NAME_LEN			128
#define 	MAX_BLOCK_SIZE			8 * 1024 * 1024
#define 	MAX_DIMENSION			3000

#pragma pack(push, 1)

typedef struct
{
	ui16			bfType;							// 'BM'
	ui32			bfSize;
	ui16			bfReserved1;
	ui16			bfReserved2;
	ui32			bfOffBits;

}	qsBMP_Header_t;


typedef struct
{
	ui32			biSize;
    i32				biWidth;
    i32				biHeight;
    ui16			biPlanes;
    ui16			biBitCount;
    ui32			biCompression;
    ui32			biSizeImage;
    i32				biXPelsPerMeter;
    i32				biYPelsPerMeter;
    ui32			biClrUsed;
    ui32			biClrImportant;

}	qsBMP_InfoHeader_t;


typedef enum
{
    uAF_UNKNOWN			= 0,
    uAF_FLAC,
    uAF_MP3,
    uAF_WAV

}	qeAudioFormat_t;


typedef struct
{
	ui32			offset;
	ui32			size;
	ui32			width;
	ui32			height;
	char			mime [20];

}	qcPicture_t;


typedef struct
{
    bool 			existsFlag;
    bool 			pictureFlag;

    qeAudioFormat_t	format;

    ui32			sampleRate;
    ui64 			totalSamples;
    ui8 			channels;
    ui8 			bitsPerSample;
    ui32			bitRate;

    char			title [64];
    char			artist [64];
    char			version [8]; 		// "ID3v1", "ID3v2.3", "ID3v2.4", "None"

    float			durationSec;

    qcPicture_t		picture;
    ui32			dataSize;
    ui32			byteRate;

}	qsMetadata_t;

#pragma pack(pop)

static const uint16_t bitrate_table[2][16] = {
    // MPEG Version 1
    { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 },
    // MPEG Version 2/2.5
    { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 }
};


static const uint16_t samplerate_table[3][4] = {
		{ 44100, 48000, 32000, 0 }, // MPEG Version 1
		{ 22050, 24000, 16000, 0 }, // MPEG Version 2
		{ 11025, 12000, 8000,  0 }  // MPEG Version 2.5
};


typedef struct
{
	JDEC	jd;
	ui8		tjdBuff [qcfgTJD_BUFF_SIZE];

}	qsJPEG_t;


class qcParse
{
	public :

		/* JPEG */
		qsJPEG_t			*JPEG_Temp;

		static size_t		InputJPEG (JDEC* jd, ui8* buff, size_t len);
		static int			OutputJPEG (JDEC *jd, void *bitmap, JRECT *rect);
		int					ScaleJPEG (void *bitmap, JRECT *rect);
		bool				RenderJPEG (void);
		static int			ReadSignatureJPEG (FIL* f, ui32 offset, ui32 max_size, ui16 *w, ui16 *h);

		/* PNG */
		static int			ReadSignaturePNG (FIL* f, ui32 offset, ui32 max_size, ui16 *w, ui16 *h);

		/* Cache */
		FIL					file;
		char				currentPath [256];

		ui16				BMP_Buffer [qcfgDISPLAY_WIDTH * qcfgDISPLAY_HEIGHT];

		char				*imgCacheTable;
		ui16				imgCacheNumber;

		void				ExtractMetaPicture (char *path);
		void				CreateImgCacheTable (void);
		void				CachePicture (char *filename);

		/* Meta */
		qsMetadata_t		Metadata;
		qeAudioFormat_t		DetectFormat(const char* filename);
		bool				ParseAudio(const char *filename);

		FRESULT				ParseFLAC_Meta (FIL *file);
		FRESULT  			ParseMP3_Meta (FIL *file);
		FRESULT				ParseWAV_Meta (FIL *file);

		void				CopyID3TextField (FIL* file, ui32 dataPosition, ui32 frameSize, char *string, size_t strinLen);
		int					ParseFrameHeaderSizeID(char *outID, ui32 *outSize, ui32 versionMajor, const ui8 *header, UINT headerRead);

		/* Folders */
		char				folders [MAX_FOLDERS][MAX_NAME_LEN];
		ui8					folderCount;
		void				ScanFolders (const char* rootPath = "/mymusic");

};

/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* HIGH_LAYER_PARSE_H_ */
