
/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		Nik125Y
 *	Telegram:	@Nik125Y
 *	Email:		topnikm@gmail.com
 *	____________________________________________________________________
 */

#include    "parse.h"

/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */

FRESULT qcParse::parse_wav_metadata(FIL *file, wav_metadata_t *meta) {
	FRESULT res;
	UINT br;
	memset(meta, 0, sizeof(wav_metadata_t));

	char id[4];
	uint32_t size;
	char format[4];

	// RIFF
	res = f_read(file, id, 4, &br);
	if (res || memcmp(id, "RIFF", 4) != 0) return FR_INVALID_OBJECT;
	res = f_lseek_ext(file, 4, SEEK_CUR);
	res = f_read(file, format, 4, &br);
	if (res || memcmp(format, "WAVE", 4) != 0) return FR_INVALID_OBJECT;

	while (f_tell(file) < f_size(file)) {

		res = f_read(file, id, 4, &br);
		res = f_read(file, &size, 4, &br);

		// "fmt " чанк
		if (memcmp(id, "fmt ", 4) == 0) {
			uint8_t fmt_buf[16];
			if (size < 16) return FR_INVALID_OBJECT;
			res = f_read(file, fmt_buf, 16, &br);
			//4 sample rate, 4 byte rate, 2 block align, 2 bits per sample
			meta->sample_rate = *(uint32_t*)&fmt_buf[4];
			meta->byte_rate = *(uint32_t*)&fmt_buf[8];

			if (size > 16)
				res = f_lseek_ext(file, size - 16, SEEK_CUR);

		} else
			if (memcmp(id, "data", 4) == 0) {
						uint8_t data_buf[8];
						res = f_read(file, data_buf, 8, &br);
						meta->data_size = *(uint32_t*)&data_buf[4];
			} // "LIST" чанк с сабчанком "INFO"
			else
			if (memcmp(id, "LIST", 4) == 0) {
			FSIZE_t list_end = f_tell(file) + size;
			char list_type[4];
			res = f_read(file, list_type, 4, &br);

			if (memcmp(list_type, "INFO", 4) != 0) {
				res = f_lseek_ext(file, size - 4, SEEK_CUR);  // skip rest of LIST
				continue;
			}

			while (f_tell(file) < list_end) {
				char sub_id[4];
				uint32_t sub_size;

				res = f_read(file, sub_id, 4, &br);
				res = f_read(file, &sub_size, 4, &br);

				if (memcmp(sub_id, "INAM", 4) == 0) {
					res = f_read(file, meta->title, (sub_size < 63) ? sub_size : 63, &br);
					meta->title[(br < 63) ? br : 63] = '\0';
				} else if (memcmp(sub_id, "IART", 4) == 0) {
					res = f_read(file, meta->artist, (sub_size < 63) ? sub_size : 63, &br);
					meta->artist[(br < 63) ? br : 63] = '\0';
				} else {
					res = f_lseek_ext(file, sub_size, SEEK_CUR);
				}

				// Выравнивание
				if (sub_size % 2 != 0) {
					res = f_lseek_ext(file, 1, SEEK_CUR);
					if (res) return res;
				}
			}

			// Перемещаемся в конец LIST чанка
			res = f_lseek(file, list_end);
			if (res) return res;

		} else {
			return FR_OK;
			// Пропуск неизвестных чанков
//			res = f_lseek_ext(file, size, SEEK_CUR);
//			if (res) return res;
//			if (size % 2 != 0) {
//				res = f_lseek_ext(file, 1, SEEK_CUR);
//				if (res) return res;
//			}
		}
	}

	return FR_OK;
}

FRESULT  qcParse::parse_mp3_metadata(FIL* file, mp3_metadata_t* meta) {
    memset(meta, 0, sizeof(mp3_metadata_t));
    f_lseek_ext(file, 0, SEEK_SET);

    strncpy(meta->title, "Unknown title", 13);
    strncpy(meta->artist, "Unknown artist", 14);

    UINT br;
    uint8_t header[10];
    f_read(file, header, 10, &br);

    if (memcmp(header, "ID3", 3) == 0) {
        // ID3v2
        uint8_t ver_major = header[3];
        //uint8_t ver_minor = header[4];
        snprintf(meta->version, sizeof(meta->version), "ID3v2.%d", ver_major);

        uint32_t tag_size = syncsafe_to_size(&header[6]);
        uint32_t tag_end = 10 + tag_size;

        uint32_t pos = 10;
        while (pos + 10 < tag_end) {
            uint8_t frame_header[10];
            f_lseek_ext(file, pos, SEEK_SET);
            f_read(file, frame_header, 10, &br);

            if (frame_header[0] == 0) break;

            char frame_id[5] = {0};
            uint32_t frame_size = 0;

            if (ver_major == 2) {
                memcpy(frame_id, frame_header, 3);
                frame_id[3] = '\0';
                frame_size = (frame_header[3] << 16) | (frame_header[4] << 8) | frame_header[5];
                pos += 6;
            } else {
                memcpy(frame_id, frame_header, 4);
                frame_id[4] = '\0';
                frame_size = (frame_header[4] << 24) | (frame_header[5] << 16) | (frame_header[6] << 8) | frame_header[7];
                pos += 10;
            }

            if (frame_size == 0 || frame_size > 1024) break;
            uint8_t frame_data[1024] = {0};
            f_lseek_ext(file, pos, SEEK_SET);
            f_read(file, frame_data, frame_size, &br);

            //название
            if (
                (ver_major == 2 && strcmp(frame_id, "TT2") == 0) ||
                (ver_major >= 3 && strcmp(frame_id, "TIT2") == 0)
            ) {
                // frame_data[0] — кодировка: 0 = ISO, 1 = UTF-16
                strncpy(meta->title, (char*)&frame_data[1], sizeof(meta->title) - 1);
            }
            //артист
            if (
                (ver_major == 2 && strcmp(frame_id, "TP1") == 0) ||
                (ver_major >= 3 && strcmp(frame_id, "TPE1") == 0)
            ) {
                strncpy(meta->artist, (char*)&frame_data[1], sizeof(meta->artist) - 1);
            }
            //обложка
            if (strcmp(frame_id, "APIC") == 0 || (ver_major == 2 && strcmp(frame_id, "PIC") == 0)) {
                meta->picture_flag = true;
            }

            pos += frame_size;
        }

        const uint32_t max_scan = tag_end + 1024 * 1024;
		uint8_t frame[4];
		for (; tag_end < max_scan; tag_end++) {
			f_lseek_ext(file, tag_end, SEEK_SET);
			f_read(file, frame, 4, &br);

			if (frame[0] == 0xFF && (frame[1] & 0xE0) == 0xE0) {
				// Syncword найдена
				uint8_t version_bits = (frame[1] >> 3) & 0x03;
				uint8_t layer_bits = (frame[1] >> 1) & 0x03;
				uint8_t bitrate_index = (frame[2] >> 4) & 0x0F;
				uint8_t sample_rate_index = (frame[2] >> 2) & 0x03;

				uint8_t mpeg_version;
				if (version_bits == 3) mpeg_version = 0;        // MPEG1
				else mpeg_version = 1;                          // MPEG2 or 2.5

				uint16_t bitrate = bitrate_table[mpeg_version][bitrate_index];
				uint16_t sample_rate = samplerate_table[mpeg_version][sample_rate_index];

				if (bitrate != 0 && sample_rate != 0) {
					meta->bitrate = bitrate * 1000;
					meta->sampleRate = sample_rate;
					meta->duration_sec = ((f_size(file) - tag_end) * 8) / meta->bitrate;
					break;
				}
			}
		}

        return FR_OK;

    } else {
        // ID3v1
        f_lseek_ext(file, -128, SEEK_END);
        uint8_t id3v1[128];
        f_read(file, id3v1, 128, &br);

        if (memcmp(id3v1, "TAG", 3) == 0) {
            strncpy(meta->version, "ID3v1", sizeof(meta->version));
            strncpy(meta->title, (char*)&id3v1[3], 30);
            strncpy(meta->artist, (char*)&id3v1[33], 30);
            return FR_OK;
        } else {
            strncpy(meta->version, "None", sizeof(meta->version));
        }
    }

    return FR_INVALID_OBJECT;
}

FRESULT qcParse::parse_flac_metadata(FIL* file, flac_metadata_t* meta)
{
    UINT br;
    uint8_t header[4];
    uint8_t blockHeader[4];
    bool flagStreamInfo = false, flagPicture = false, flagComment = false;

    memset(meta, 0, sizeof(flac_metadata_t));

    strncpy(meta->title, "Unknown title", 13);
    strncpy(meta->artist, "Unknown artist", 14);
    meta->picture_flag = false;

    // Проверка на "fLaC"
    f_read(file, header, 4, &br);
    if (br != 4 || memcmp(header, "fLaC", 4) != 0)
        return FR_INVALID_OBJECT;

    bool lastBlock = false;
    while (!lastBlock) {
        f_read(file, blockHeader, 4, &br);
        if (br != 4) return FR_DISK_ERR;

        lastBlock = (blockHeader[0] & 0x80) != 0;
        uint8_t blockType = blockHeader[0] & 0x7F;
        uint32_t blockSize = (blockHeader[1] << 16) | (blockHeader[2] << 8) | blockHeader[3];
        uint32_t blockStart = f_tell(file);

        switch (blockType) {
            case 0: { // STREAMINFO

            	if (flagStreamInfo) break;
            	flagStreamInfo = true;

                uint8_t buf[18 + 16];
                if (blockSize != 34) return FR_INVALID_OBJECT;

                f_read(file, buf, sizeof(buf), &br);
                if (br != sizeof(buf)) return FR_DISK_ERR;

                meta->sampleRate =
                    ((uint32_t)buf[10] << 12) |
                    ((uint32_t)buf[11] << 4) |
                    ((buf[12] & 0xF0) >> 4);

                meta->channels = ((buf[12] & 0x0E) >> 1) + 1;
                meta->bitsPerSample = (((buf[12] & 0x01) << 4) | ((buf[13] >> 4) & 0x0F)) + 1;

                meta->totalSamples =
                    ((uint64_t)(buf[13] & 0x0F) << 32) |
                    ((uint64_t)buf[14] << 24) |
                    ((uint64_t)buf[15] << 16) |
                    ((uint64_t)buf[16] << 8) |
                    ((uint64_t)buf[17]);

                break;
            }

            case 4: { // VORBIS_COMMENT
            	if (flagComment) break;
            	flagComment = true;

                uint8_t lenBuf[4];
                f_read(file, lenBuf, 4, &br);  // vendor length
                uint32_t vendorLen = lenBuf[0] | (lenBuf[1] << 8) | (lenBuf[2] << 16) | (lenBuf[3] << 24);
                f_lseek(file, f_tell(file) + vendorLen); // skip vendor string

                f_read(file, lenBuf, 4, &br);  // user comment count
                uint32_t count = lenBuf[0] | (lenBuf[1] << 8) | (lenBuf[2] << 16) | (lenBuf[3] << 24);

                for (uint32_t i = 0; i < count; i++) {
                    f_read(file, lenBuf, 4, &br);
                    uint32_t len = lenBuf[0] | (lenBuf[1] << 8) | (lenBuf[2] << 16) | (lenBuf[3] << 24);

                    if (len > 128) {
                        f_lseek(file, f_tell(file) + len);
                        continue;
                    }

                    char temp[129] = {0};
                    f_read(file, (BYTE*)temp, len, &br);

                    if (strncmp(temp, "TITLE=", 6) == 0)
                        strncpy(meta->title, temp + 6, sizeof(meta->title) - 1);
                    else if (strncmp(temp, "ARTIST=", 7) == 0)
                        strncpy(meta->artist, temp + 7, sizeof(meta->artist) - 1);
                }
                break;
            }

            case 6: { // PICTURE
            	if (flagPicture) break;
            	flagPicture = true;

                BYTE picHeader[32];
                if (blockSize < 32) break;

                f_read(file, picHeader, 32, &br);
                if (br != 32) return FR_DISK_ERR;

                meta->picture_flag = true;

                uint32_t mimeLen = (picHeader[4] << 24) | (picHeader[5] << 16) | (picHeader[6] << 8) | picHeader[7];
                f_lseek(file, f_tell(file) + mimeLen);

                uint32_t descLen = (picHeader[8] << 24) | (picHeader[9] << 16) | (picHeader[10] << 8) | picHeader[11];
                f_lseek(file, f_tell(file) + descLen);

                uint8_t imageInfo[20];
                f_read(file, imageInfo, 20, &br);

                meta->pictureWidth  = (imageInfo[0] << 24) | (imageInfo[1] << 16) | (imageInfo[2] << 8) | imageInfo[3];
                meta->pictureHeight = (imageInfo[4] << 24) | (imageInfo[5] << 16) | (imageInfo[6] << 8) | imageInfo[7];
                meta->pictureSize   = (imageInfo[16] << 24) | (imageInfo[17] << 16) | (imageInfo[18] << 8) | imageInfo[19];

                meta->pictureOffset = f_tell(file);
                break;
            }

            default:
                break;
        }

        f_lseek(file, blockStart + blockSize);
    }

    return FR_OK;
}


void qcParse::ScanFolders(const char* rootPath) {
	DIR dir;
	FILINFO fno;

	if (f_opendir(&dir, rootPath) != FR_OK)
		return;

	folderCount = 0;

	while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0] != 0 && folderCount < MAX_FOLDERS) {
		if (fno.fattrib & AM_DIR) {
			snprintf(folders[folderCount], MAX_NAME_LEN, "/%s/%s", rootPath, fno.fname);
			folderCount++;
		}
	}

	f_closedir(&dir);
}

ui32 qcParse::syncsafe_to_size(uint8_t* bytes) {
	return (bytes[0] << 21) | (bytes[1] << 14) | (bytes[2] << 7) | (bytes[3]);
}
