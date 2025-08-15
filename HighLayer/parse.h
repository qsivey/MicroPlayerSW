/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		Nik125Y
 *	Telegram:	@Nik125Y
 *	Email:		topnikm@gmail.com
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

#define MAX_FOLDERS 20
#define MAX_NAME_LEN 32

typedef struct {
	bool flag;
	bool picture_flag;
    // STREAMINFO
    uint32_t sampleRate;
    uint64_t totalSamples;
    uint8_t channels;
    uint8_t bitsPerSample;

    // VORBIS_COMMENT
    char title[64];
    char artist[64];

    // PICTURE
    ui32 	pictureOffset;
    ui32	pictureSize;
    ui32 	pictureWidth;
    ui32	pictureHeight;
} flac_metadata_t;

typedef struct {
	bool flag;
	bool picture_flag;

    char title[64];
    char artist[64];

    //TIME
    ui32 sampleRate;
    ui32 bitrate;
    uint64_t totalSamples;
    float duration_sec;

    char version[8]; // "ID3v1", "ID3v2.3", "ID3v2.4", "None"
} mp3_metadata_t;

typedef struct {
	bool flag;
	bool picture_flag;

    char title[64];
    char artist[64];

    ui32 data_size;
    ui32 sample_rate;
    ui32 byte_rate;
} wav_metadata_t;

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

class qcParse
{
	public :

		/* Meta */
		flac_metadata_t		flac_meta;
		mp3_metadata_t		mp3_meta;
		wav_metadata_t		wav_meta;
		FRESULT				parse_flac_metadata(FIL* file, flac_metadata_t* meta);
		FRESULT  			parse_mp3_metadata(FIL* file, mp3_metadata_t* meta);
		FRESULT				parse_wav_metadata(FIL *file, wav_metadata_t *meta);

		/* Folders */
		char				folders[MAX_FOLDERS][MAX_NAME_LEN];
		ui8					folderCount;
		void				ScanFolders(const char* rootPath = "/mymusic");
		ui32				syncsafe_to_size(uint8_t* bytes);

};

/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* HIGH_LAYER_PARSE_H_ */
