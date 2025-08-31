/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */


#include    "parse.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
/* -------------------------------------------------
 * JPEG Functions
 */
size_t qcParse::InputJPEG (JDEC *jd, ui8 *buff, size_t len)
{
	UINT bytesRead = 0;
	qcParse* self = (qcParse*)jd->device;

    if (!buff)
    {
    	buff = qmCreateArr(ui8, len);
		f_read(&self->file, buff, len, &bytesRead);
		qmFree(buff);

		return len;
    }

	if (f_read(&self->file, buff, len, &bytesRead) != FR_OK)
		return 0;

    return bytesRead;
}


int qcParse::OutputJPEG (JDEC *jd, void *bitmap, JRECT *rect)
{
	qcParse *self = (qcParse*)jd->device;

	self->ScaleJPEG((ui16*)bitmap, rect);

	return 1;
}


int qcParse::ScaleJPEG (void *bitmap, JRECT *rect)
{
	ui16 *src = (ui16 *)bitmap;

	ui16 srcWidth = rect->right - rect->left + 1;
	ui16 srcHeight = rect->bottom - rect->top + 1;

	float scaleX = (float)qcfgPICTURE_RESOLUTION / JPEG_Temp->jd.width;
	float scaleY = (float)qcfgPICTURE_RESOLUTION / JPEG_Temp->jd.height;

	for (ui16 sy = 0; sy < srcHeight; sy++)
	{
		ui16 srcY = rect->top + sy;
		float fy = srcY * scaleY;
		ui16 dy = (ui16)fy;

		if (dy >= qcfgPICTURE_RESOLUTION)
			continue;

		for (ui16 sx = 0; sx < srcWidth; sx++)
		{
			ui16 srcX = rect->left + sx;
			float fx = srcX * scaleX;
			ui16 dx = (ui16)fx;

			if (dx >= qcfgPICTURE_RESOLUTION)
				continue;

			ui16 color = src[sy * srcWidth + sx];
			BMP_Buffer[dy * qcfgPICTURE_RESOLUTION + dx] = __REV16(color);
		}
	}

	return 1;
}


bool qcParse::RenderJPEG (void)
{
	JPEG_Temp = qmCreate(qsJPEG_t);

	JPEG_Temp->jd.device = (void*)this;

	f_lseek_ext(&file, Metadata.picture.offset, SEEK_SET);

	JRESULT res = jd_prepare(&JPEG_Temp->jd, InputJPEG, JPEG_Temp->tjdBuff, qcfgTJD_BUFF_SIZE, this);

	if (res == JDR_OK)
		jd_decomp(&JPEG_Temp->jd, OutputJPEG, 0);

	else
	{
		qmFree(JPEG_Temp);

		return false;
	}

	qmFree(JPEG_Temp);

	return true;
}


int qcParse::ReadSignatureJPEG (FIL* f, ui32 offset, ui32 max_size, ui16 *w, ui16 *h)
{
    if (max_size < 4) return 0;

    if (f_lseek_ext(f, offset, SEEK_SET)!=FR_OK) return 0;

    ui8 b[2];
    if (SafeReadBytes(f, b, 2) != 0) return 0;
    if (b[0] != 0xFF || b[1] != 0xD8) return 0; // SOI

    ui32 pos = offset + 2;
    while (pos + 4 <= offset + max_size)
    {
        if (f_lseek_ext(f,pos,SEEK_SET)!=FR_OK) return 0;

        ui8 m[2];
        if (SafeReadBytes(f, m, 2) != 0) return 0;
        pos += 2;
        if (m[0]!=0xFF) { pos--; continue; }

        ui8 mk = m[1];
        if (mk == 0xD8 || mk == 0x01 || (mk >= 0xD0 && mk <= 0xD9)) continue;
        if (f_lseek_ext(f,pos,SEEK_SET)!=FR_OK) return 0;

        ui8 L[2];
        if (SafeReadBytes(f, L, 2) != 0) return 0;

        ui16 seg = TransformBigEndianTo16Bit(L);
        if (seg < 2) return 0;

        if ((mk >= 0xC0 && mk <= 0xC3) ||
        	(mk >= 0xC5 && mk <= 0xC7) ||
			(mk >= 0xC9 && mk <= 0xCB) ||
			(mk >= 0xCD && mk <= 0xCF))
        {
            if (seg < 7) return 0;
            if (f_lseek_ext(f, pos + 2, SEEK_SET)!=FR_OK) return 0;
            ui8 d[5];
            if (SafeReadBytes(f, d, 5) != 0) return 0;
            ui16 H = TransformBigEndianTo16Bit(d + 1), W = TransformBigEndianTo16Bit(d + 3);
            if (W == 0 || H == 0) return 0;
            *w = W; *h = H;
            return 1;
        }
        pos += seg;
    }
    return 0;
}

/* -------------------------------------------------
 * PNG Functions
 */

int qcParse::ReadSignaturePNG (FIL* f, ui32 offset, ui32 max_size, ui16 *w, ui16 *h)
{
    if (max_size < 24) return 0;
    if (f_lseek_ext(f, offset, SEEK_SET) != FR_OK) return 0;

    ui8 b[24];
    if (SafeReadBytes(f, b, 24) != 0) return 0;

    const ui8 sig[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    if (memcmp(b, sig, 8) != 0) return 0;
    if (memcmp(b + 12, "IHDR", 4) !=0 ) return 0;

    ui32 W = TransformBigEndianTo32Bit(b + 16), H = TransformBigEndianTo32Bit(b + 20);
    if (W == 0 || H == 0) return 0;

    *w = (ui16)(W > 0xFFFF ? 0xFFFF : W);
    *h = (ui16)(H > 0xFFFF ? 0xFFFF : H);
    return 1;
}

/* -------------------------------------------------
 * Cashe Functions
 */

void qcParse::ExtractMetaPicture (char *path)
{
	/* Take current address */
//	if (f_open(&file, path, FA_READ) == FR_OK)
//	{
//		if (parseFLAC_Meta(&file) == FR_OK) Metadata.existsFlag = true;

		if (Metadata.pictureFlag)
		{
			char imgName [13];

			ui32 imgMetaCoef = Metadata.picture.size + Metadata.picture.width + Metadata.picture.height;
			imgMetaCoef = 0xFFFFFFFF - (0xFFFFFFFF % imgMetaCoef);

			sprintf(imgName, "%08x.bmp", imgMetaCoef);

			for (ui16 i = 0; i < imgCacheNumber; i++)
				if (!strncmp(imgName, &imgCacheTable[i * 8], 8))
				{
//					f_close(&file);
					return;
				}

			/* Parse picture */
			if (!RenderJPEG())
			{
//				f_close(&file);
				return;
			}

//			f_close(&file);

			/* Write as cache to drive */
			CachePicture(imgName);

			char *imgCacheTableTemp = (char*)reallocarray(imgCacheTable, imgCacheNumber + 1, sizeof(char) * 8);

			if (imgCacheTableTemp)
			{
				imgCacheTable = imgCacheTableTemp;
				memcpy(&imgCacheTable[imgCacheNumber], imgName, 8);
			}

			imgCacheNumber++;
		}
//	}
}


void qcParse::CreateImgCacheTable (void)
{
	FILINFO fno;
	DIR dir;

	FRESULT res = f_chdir(qcfgCACHE_IMG_PATH);

	if (res != FR_OK)
	{
		res = f_mkdir(qcfgCACHE_PATH);
		res = f_chmod(qcfgCACHE_PATH, AM_HID, AM_HID);

		res = f_mkdir(qcfgCACHE_IMG_PATH);

		f_chdir(qcfgCACHE_IMG_PATH);
	}

	res = f_findfirst(&dir, &fno, qcfgCACHE_IMG_PATH, "*.bmp");

	while (res == FR_OK && fno.fname[0])
	{
		if (!(fno.fattrib & AM_DIR))  // Skip folders
			imgCacheNumber++;

		res = f_findnext(&dir, &fno);
	}

	f_closedir(&dir);

	imgCacheTable = qmCreateArr(char, (imgCacheNumber * 8) + 1);
	imgCacheTable[imgCacheNumber * 8] = '\0';

	res = f_findfirst(&dir, &fno, qcfgCACHE_IMG_PATH, "*.bmp");

	ui32 i = 0;

	while (res == FR_OK && fno.fname[0])
	{
		if (!(fno.fattrib & AM_DIR))
		{
			const char *name = fno.fname;

			memcpy(&imgCacheTable[i * 8], name, 8);
			i++;
		}

		res = f_findnext(&dir, &fno);
	}

	f_closedir(&dir);
}


void qcParse::CachePicture (char *filename)
{
    FIL file;
    FRESULT res;
    UINT written;

    qsBMP_Header_t header;
    qsBMP_InfoHeader_t info;

    ui32 row_size = qcfgPICTURE_RESOLUTION * 2;
    ui32 image_size = row_size * qcfgPICTURE_RESOLUTION;
    ui32 file_size = sizeof(qsBMP_Header_t) + sizeof(qsBMP_InfoHeader_t) + image_size;

    header.bfType = 0x4D42; // 'BM'
    header.bfSize = file_size;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits = sizeof(qsBMP_Header_t) + sizeof(qsBMP_InfoHeader_t);

    info.biSize = sizeof(qsBMP_InfoHeader_t);
    info.biWidth = qcfgPICTURE_RESOLUTION;
    info.biHeight = -qcfgPICTURE_RESOLUTION;
    info.biPlanes = 1;
    info.biBitCount = 16;  // RGB565
    info.biCompression = 3; // BI_BITFIELDS
    info.biSizeImage = image_size;
    info.biXPelsPerMeter = 0;
    info.biYPelsPerMeter = 0;
    info.biClrUsed = 0;
    info.biClrImportant = 0;

    const ui32 masks [3] = { 0xF800, 0x07E0, 0x001F };

    f_chdir(qcfgCACHE_IMG_PATH);

    res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) return;

    f_write(&file, &header, sizeof(header), &written);
    f_write(&file, &info, sizeof(info), &written);
    f_write(&file, masks, sizeof(masks), &written);

    // --- Запись пикселей ---
    for (int y = 0; y < qcfgPICTURE_RESOLUTION; y++)
    {
        ui16 *row = &BMP_Buffer[qcfgPICTURE_RESOLUTION * y];

        for (ui16 i = 0; i < qcfgPICTURE_RESOLUTION; i++)
		{
        	row[i] = __REV16(row[i]);
		}

        f_write(&file, row, row_size, &written);
    }

    f_close(&file);

    /* Return to old address */
	f_chdir(currentPath);
}


/* -------------------------------------------------
 * Metadata Parsers
 */

qeAudioFormat_t qcParse::DetectFormat (const char  *filename)
{
    const char *ext = strrchr(filename, '.');

    if (ext) {
        if ((strcasecmp(ext, ".flac") == 0) || (strcasecmp(ext, ".ogg") == 0)) return uAF_FLAC;
        if ( strcasecmp(ext, ".mp3")  == 0) 								   return uAF_MP3;
        if ((strcasecmp(ext, ".wav")  == 0) || (strcasecmp(ext, ".wv") == 0))  return uAF_WAV;
    }

    return uAF_UNKNOWN;
}


bool qcParse::ParseAudio (const char *filename)
{
    Metadata.format = DetectFormat(filename);
    Metadata.existsFlag = false;
    if (f_open(&file, filename, FA_READ) == FR_OK)

        switch (Metadata.format) {
        case uAF_FLAC:		if (ParseFLAC_Meta(&file) == FR_OK) Metadata.existsFlag = true;
        case uAF_MP3:		if (ParseMP3_Meta(&file) == FR_OK)  Metadata.existsFlag = true;
        case uAF_WAV: 		if (ParseWAV_Meta(&file) == FR_OK)  Metadata.existsFlag = true;
        default: 			f_close(&file);
    }

    return Metadata.existsFlag;
}


FRESULT qcParse::ParseFLAC_Meta(FIL* file)
{
    UINT br;

    // ---- helpers ----
    auto read_n = [&](void* buf, UINT n)->FRESULT {
        f_read(file, (BYTE*)buf, n, &br);
        return (br == n) ? FR_OK : FR_DISK_ERR;
    };

    auto tell = [&]()->FSIZE_t { return f_tell(file); };
    auto seek = [&](FSIZE_t pos)->FRESULT { return f_lseek(file, pos); };
    auto skip = [&](FSIZE_t n)->FRESULT { return seek(tell() + n); };

    auto TransformBigEndianTo32Bit = [&](ui32 &out)->FRESULT {
        ui8 b[4];
        if (read_n(b, 4) != FR_OK) return FR_DISK_ERR;
        out = ((ui32)b[0] << 24) | ((ui32)b[1] << 16) | ((ui32)b[2] << 8) | (ui32)b[3];
        return FR_OK;
    };

    auto le32 = [&](ui32 &out)->FRESULT {
        ui8 b[4];
        if (read_n(b, 4) != FR_OK) return FR_DISK_ERR;
        out = (ui32)b[0] | ((ui32)b[1] << 8) | ((ui32)b[2] << 16) | ((ui32)b[3] << 24);
        return FR_OK;
    };

    auto be24_from_buf = [&](const ui8 *b)->ui32 {
        return ((ui32)b[0] << 16) | ((ui32)b[1] << 8) | (ui32)b[2];
    };

    // ---- parse meta ----
    memset(&Metadata, 0, sizeof(qsMetadata_t));
    strncpy(Metadata.title, "Unknown title", sizeof(Metadata.title)-1);
    strncpy(Metadata.artist, "Unknown artist", sizeof(Metadata.artist)-1);
    Metadata.pictureFlag = false;

    ui8 sig [4];
    if (read_n(sig, 4) != FR_OK) return FR_DISK_ERR;
    if (memcmp(sig, "fLaC", 4) != 0) return FR_INVALID_OBJECT;

    bool lastBlock = false;
    bool haveStreamInfo = false;
    bool haveComment = false;
    bool havePicture = false;

    while (!lastBlock) {
        ui8 hdr [4];
        if (read_n(hdr, 4) != FR_OK) return FR_DISK_ERR;

        lastBlock = (hdr [0] & 0x80) != 0;
        ui8 type = hdr [0] & 0x7F;
        ui32 blockSize = be24_from_buf(&hdr [1]);

        if (blockSize > MAX_BLOCK_SIZE) return FR_INVALID_OBJECT;

        FSIZE_t blockStart = tell();
        FSIZE_t blockEnd = blockStart + blockSize;

        switch (type) {
            case 0:
            { // STREAMINFO
                if (haveStreamInfo) return FR_INVALID_OBJECT;
                haveStreamInfo = true;

                if (blockSize < 34) return FR_INVALID_OBJECT;
                ui8 buf[34];
                if (read_n(buf, 34) != FR_OK) return FR_DISK_ERR;

                Metadata.sampleRate = ((ui32)buf[10] << 12) | ((ui32)buf[11] << 4) | ((ui32)(buf[12] & 0xF0) >> 4);
                Metadata.channels = (ui8)(((buf[12] & 0x0E) >> 1) + 1);
                Metadata.bitsPerSample = (ui8)((((buf[12] & 0x01) << 4) | ((buf[13] >> 4) & 0x0F)) + 1);

                ui64 upper4 = (ui64)(buf[13] & 0x0F);
                Metadata.totalSamples = (upper4 << 32) | ((ui64)buf[14] << 24) | ((ui64)buf[15] << 16) | ((ui64)buf[16] << 8) | (ui64)buf[17];

                if (Metadata.totalSamples == 0) {
                    donothing_; // not fatal
                }

                if (seek(blockEnd) != FR_OK) return FR_DISK_ERR;
                break;
            }

            case 4:
            { // VORBIS COMMENT
                if (haveComment) return FR_INVALID_OBJECT;
                haveComment = true;

                ui32 vendorLen;
                if (le32(vendorLen) != FR_OK) return FR_DISK_ERR;
                if ((FSIZE_t)vendorLen > (blockEnd - tell())) return FR_INVALID_OBJECT;
                if (vendorLen) { if (skip(vendorLen) != FR_OK) return FR_DISK_ERR; }

                ui32 count;
                if (le32(count) != FR_OK) return FR_DISK_ERR;
                if (count > 1000000) return FR_INVALID_OBJECT;

                for (ui32 i = 0; i < count; ++i) {
                    ui32 len;
                    if (le32(len) != FR_OK) return FR_DISK_ERR;
                    if ((FSIZE_t)len > (blockEnd - tell())) return FR_INVALID_OBJECT;

                    if (len == 0) continue;

                    ui32 toRead = len;
                    if (toRead > 255) toRead = 255;

                    char tmp[256] = {0};
                    if (read_n(tmp, toRead) != FR_OK) return FR_DISK_ERR;

                    if (len > toRead) {
                        if (skip(len - toRead) != FR_OK) return FR_DISK_ERR;
                    }

                    if (strncasecmp(tmp, "TITLE=", 6) == 0) { // also for lowercase letters
                        strncpy(Metadata.title, tmp + 6, sizeof(Metadata.title) - 1);
                        Metadata.title[sizeof(Metadata.title)-1] = '\0';
                    } else if (strncasecmp(tmp, "ARTIST=", 7) == 0) {
                        strncpy(Metadata.artist, tmp + 7, sizeof(Metadata.artist) - 1);
                        Metadata.artist[sizeof(Metadata.artist)-1] = '\0';
                    }
                }

                if (seek(blockEnd) != FR_OK) return FR_DISK_ERR;
                break;
            }

            case 6:
            { // PICTURE
                if (havePicture) return FR_INVALID_OBJECT;
                havePicture = true;

                // picture type, mime length
                ui32 picType, mimeLen;
                if (TransformBigEndianTo32Bit(picType) != FR_OK) return FR_DISK_ERR;
                if (TransformBigEndianTo32Bit(mimeLen) != FR_OK) return FR_DISK_ERR;
                if ((FSIZE_t)mimeLen > (blockEnd - tell())) return FR_INVALID_OBJECT;
                if (mimeLen) { if (skip(mimeLen) != FR_OK) return FR_DISK_ERR; }

                // description length
                ui32 descLen;
                if (TransformBigEndianTo32Bit(descLen) != FR_OK) return FR_DISK_ERR;
                if ((FSIZE_t)descLen > (blockEnd - tell())) return FR_INVALID_OBJECT;
                if (descLen) { if (skip(descLen) != FR_OK) return FR_DISK_ERR; }

                ui32 width, height, depth, colors;
                if (TransformBigEndianTo32Bit(width)  != FR_OK) return FR_DISK_ERR;
                if (TransformBigEndianTo32Bit(height) != FR_OK) return FR_DISK_ERR;
                if (TransformBigEndianTo32Bit(depth)  != FR_OK) return FR_DISK_ERR;
                if (TransformBigEndianTo32Bit(colors) != FR_OK) return FR_DISK_ERR;

                if (width > MAX_DIMENSION || height > MAX_DIMENSION) {
                    width = height = 0;
                }

                // picture length
                ui32 picLen;
                if (TransformBigEndianTo32Bit(picLen) != FR_OK) return FR_DISK_ERR;
                if ((FSIZE_t)picLen > (blockEnd - tell())) return FR_INVALID_OBJECT;

                FSIZE_t picOffset = tell(); //absolute offset
                Metadata.picture.offset = (ui32)picOffset;
                Metadata.picture.size = picLen;
                Metadata.picture.width = width;
                Metadata.picture.height = height;
                Metadata.pictureFlag = true;
                snprintf(Metadata.picture.mime, sizeof(Metadata.picture.mime), "image/jpeg");

                if (seek(blockEnd) != FR_OK) return FR_DISK_ERR;
                break;
            }

            default: {
                if (seek(blockEnd) != FR_OK) return FR_DISK_ERR;
                break;
            }
        } // switch
    } // while

    return FR_OK;
}


FRESULT qcParse::ParseMP3_Meta (FIL* file)
{
    memset(&Metadata, 0, sizeof(qsMetadata_t));
    snprintf(Metadata.title,  sizeof(Metadata.title),  "Unknown title");
    snprintf(Metadata.artist, sizeof(Metadata.artist), "Unknown artist");
    Metadata.pictureFlag   = false;

    if (f_lseek_ext(file, 0, SEEK_SET) != FR_OK) return FR_DISK_ERR;

    UINT br = 0;
    ui8 tagHeader[10];
    if (f_read(file, tagHeader, 10, &br) != FR_OK || br != 10) return FR_DISK_ERR;

    if (memcmp(tagHeader, "ID3", 3) == 0)
    {
        ui8 versionMajor = tagHeader[3];
        snprintf(Metadata.version, sizeof(Metadata.version), "ID3v2.%u", versionMajor);

        ui8 flags = tagHeader[5];
        ui32 tagSize = DecodeSyncsafeTo_u32(tagHeader + 6);
        ui32 pos = 10;

        // Extended header for v2.3/v2.4
        if (versionMajor>=3 && (flags & 0x40))
        {
            ui8 ex[6];
            if (f_lseek_ext(file, pos, SEEK_SET) != FR_OK) return FR_DISK_ERR;
            if (f_read(file, ex, (versionMajor == 4) ? 4 : 4, &br) != FR_OK || br < 4) return FR_DISK_ERR;
            ui32 exSize = (versionMajor == 4) ? DecodeSyncsafeTo_u32(ex) : TransformBigEndianTo32Bit(ex);
            pos += exSize + ((versionMajor == 3) ? 0 : 0);
            if (pos > 10 + tagSize) pos = 10 + tagSize;
        }

        // Footer v2.4
        ui32 tagEnd = 10 + tagSize + ((versionMajor == 4 && (flags & 0x10)) ? 10 : 0);

        while (pos + ((versionMajor==2)?6:10) <= 10 + tagSize){
            if (f_lseek_ext(file, pos, SEEK_SET)!=FR_OK) break;
            ui8 fhdr[10]={0}; UINT need = (versionMajor==2)?6:10;
            if (f_read(file, fhdr, need, &br)!=FR_OK || br<need) break;

            int zero_id = 1; for (unsigned i=0;i<((versionMajor==2)?3:4);++i) if (fhdr[i]!=0) {zero_id=0;break;}
            if (zero_id) break;

            char fid[5] = {0};
            ui32 fileSize=0;
            int headerLen = ParseFrameHeaderSizeID(fid, &fileSize, versionMajor, fhdr, br);
            if (headerLen<0) break;

            if (fileSize==0 || fileSize > (50u*1024u*1024u)) { pos += headerLen; continue; }

            ui32 fileDataPos = pos + (ui32)headerLen;
            ui32 fileDataEnd = fileDataPos + fileSize;
            if (fileDataEnd > 10 + tagSize) fileDataEnd = 10 + tagSize;

            // --- TEXT: title ---
            if ((versionMajor == 2 && strcmp(fid, "TT2") == 0) || (versionMajor >= 3 && strcmp(fid, "TIT2") == 0))
                CopyID3TextField(file, fileDataPos, fileSize, Metadata.title, sizeof(Metadata.title));

            // --- TEXT: artist ---
            if ((versionMajor == 2 && strcmp(fid, "TP1") == 0) || (versionMajor >= 3 && strcmp(fid, "TPE1") == 0))
                CopyID3TextField(file, fileDataPos, fileSize, Metadata.artist, sizeof(Metadata.artist));

            // --- APIC: picture ---
            if ((versionMajor >= 3 && strcmp(fid, "APIC") == 0) || (versionMajor == 2 && strcmp(fid, "PIC") == 0))
            {
                ui32 cur = fileDataPos;
                if (f_lseek_ext(file, cur, SEEK_SET) != FR_OK) { pos += (ui32)headerLen + fileSize; continue; }

                ui8 enc = 0; if (SafeRead_u8(file, &enc) != 0) { pos += (ui32)headerLen + fileSize; continue; }
                cur += 1;

                char mimeTmp[32]; mimeTmp[0]='\0';

                if (versionMajor==2)
                {
                    ui8 fmt[3];
                    if (SafeReadBytes(file, fmt, 3) != 0) { pos += (ui32)headerLen + fileSize; continue; }
                    cur += 3;
                    if (!memcmp(fmt, "PNG", 3)) snprintf(mimeTmp, sizeof(mimeTmp), "image/png");
                    else snprintf(mimeTmp, sizeof(mimeTmp), "image/jpeg");
                }

                else
                {
                    ui32 i = 0;
                    while (cur < fileDataPos + fileSize && i < sizeof(mimeTmp) - 1)
                    {
                        ui8 b;
                        if (SafeRead_u8(file, &b) != 0) break;
                        cur++;
                        if (b == 0x00) break;
                        mimeTmp[i++] = (char)b;
                    }
                    mimeTmp[i] = '\0';
                }

                // picture type
                ui8 pictureType = 0;
                if (f_lseek_ext(file, cur, SEEK_SET) == FR_OK && SafeRead_u8(file, &pictureType) == 0) cur += 1;

                // description
                if (enc == 1 || enc == 2)
                {
                	// UTF-16: 0x00 0x00
                    while (cur + 1 < fileDataPos + fileSize)
                    {
                        ui8 d[2]; if (f_lseek_ext(file, cur, SEEK_SET) != FR_OK) break;
                        if (SafeReadBytes(file, d, 2) != 0) break;
                        cur += 2;
                        if (d[0] == 0x00 && d[1] == 0x00) break;
                    }
                }

                else
                {
                    while (cur < fileDataPos + fileSize){
                        ui8 b;
                        if (SafeRead_u8(file, &b) != 0) break;
                        cur++;
                        if (b == 0x00) break;
                    }
                }

                if (cur < fileDataPos + fileSize)
                {
                    Metadata.pictureFlag   = true;
                    Metadata.picture.offset = cur;
                    Metadata.picture.size   = fileDataPos + fileSize - cur;

                    ui16 W = 0, H = 0;
                    int isPNG = 0, isJPEG = 0;
                    if (ReadSignaturePNG(file, cur, Metadata.picture.size, &W, &H)) isPNG = 1;
                    else if (ReadSignatureJPEG(file, cur, Metadata.picture.size, &W, &H)) isJPEG = 1;

                    if (isPNG) snprintf(Metadata.picture.mime, sizeof(Metadata.picture.mime), "image/png");
                    else if (isJPEG) snprintf(Metadata.picture.mime, sizeof(Metadata.picture.mime), "image/jpeg");
                    else if (mimeTmp[0]) snprintf(Metadata.picture.mime, sizeof(Metadata.picture.mime), "%s", mimeTmp);

                    Metadata.picture.width  = W;
                    Metadata.picture.height = H;
                }
            }

            pos = fileDataEnd;
        }

        // --- Parse MPEG ---
        ui32 fileSize = f_size(file);
        ui32 scanStart = tagEnd;
        ui32 scanLimit = scanStart + 1024u * 1024u;
        if (scanLimit > fileSize) scanLimit = fileSize;

        ui8 fr[4];
        for (ui32 p = scanStart; p + 4 <= scanLimit; ++p)
        {
            if (f_lseek_ext(file, p, SEEK_SET) != FR_OK) break;
            if (f_read(file, fr, 4, &br) != FR_OK || br < 4) break;
            if (fr[0] != 0xFF || (fr[1] & 0xE0) != 0xE0) continue;

            ui8 versionBits = (fr[1] >> 3) & 0x03; // 3 = MPEG1, 2 = MPEG2, 0 = MPEG2.5
            ui8 layerBits = (fr[1] >> 1) & 0x03;
            unused_(layerBits);

            ui8 bitRateIndex  = (fr[2] >> 4) & 0x0F;
            if (bitRateIndex == 0 || bitRateIndex == 0x0F) continue;
            ui8 sampleRateIndex = (fr[2] >> 2) & 0x03;
            if (sampleRateIndex == 0x03) continue;

            ui8 mpegVersion = (versionBits == 3) ? 0 : 1; // 0: MPEG1, 1: MPEG2/2.5
            uint16_t bitRate = bitrate_table[mpegVersion][bitRateIndex];
            uint16_t sampleRate   = samplerate_table[mpegVersion][sampleRateIndex];
            if (!bitRate || !sampleRate) continue;

            Metadata.bitRate    = (ui32)bitRate * 1000u;
            Metadata.sampleRate = (ui32)sampleRate;
            if (fileSize > p) Metadata.durationSec = ((fileSize - p) * 8u) / Metadata.bitRate;
            break;
        }
        return FR_OK;
    }

    else
    {
        // ID3v1
        if (f_lseek_ext(file, -128, SEEK_END) != FR_OK) return FR_INVALID_OBJECT;
        ui8 v1[128]; if (f_read(file, v1, 128, &br) != FR_OK || br != 128) return FR_DISK_ERR;
        if (memcmp(v1,"TAG",3) == 0)
        {
            snprintf(Metadata.version, sizeof(Metadata.version), "ID3v1");
            // title
            {
                size_t n = (sizeof(Metadata.title) - 1 < 30) ? sizeof(Metadata.title) - 1 : 30;
                memcpy(Metadata.title, v1 + 3, n); Metadata.title[n] = '\0';
            }
            // artist
            {
                size_t n = (sizeof(Metadata.artist) - 1 < 30) ? sizeof(Metadata.artist) - 1 : 30;
                memcpy(Metadata.artist, v1 + 33, n); Metadata.artist[n] = '\0';
            }
            return FR_OK;
        }

        else
        {
            snprintf(Metadata.version, sizeof(Metadata.version), "None");
        }
    }
    return FR_INVALID_OBJECT;
}


FRESULT qcParse::ParseWAV_Meta (FIL *file)
{
	FRESULT res;
	UINT br;
	memset(&Metadata, 0, sizeof(qsMetadata_t));

	char id[4];
	ui32 size;
	char format[4];

	// ---- RIFF ----
	res = f_read(file, id, 4, &br);
	if (res || memcmp(id, "RIFF", 4) != 0) return FR_INVALID_OBJECT;
	res = f_lseek_ext(file, 4, SEEK_CUR);
	res = f_read(file, format, 4, &br);
	if (res || memcmp(format, "WAVE", 4) != 0) return FR_INVALID_OBJECT;

	while (f_tell(file) < f_size(file))
	{
		FSIZE_t chunk_offset = f_tell(file); unused_(chunk_offset);
		res = f_read(file, id, 4, &br);
		if (res) return res;

		res = f_read(file, &size, 4, &br);
		if (res) return res;

		// ---- "fmt " ----
		if (memcmp(id, "fmt ", 4) == 0)
		{
			ui8 fmt_buf[16];
			if (size < 16) return FR_INVALID_OBJECT;
			res = f_read(file, fmt_buf, 16, &br);

			Metadata.sampleRate   = *(ui32*)&fmt_buf[4];
			Metadata.byteRate     = *(ui32*)&fmt_buf[8];
			Metadata.bitsPerSample= *(ui16*)&fmt_buf[14];
			Metadata.channels     = *(ui16*)&fmt_buf[2];

			if (size > 16)
				res = f_lseek_ext(file, size - 16, SEEK_CUR);
		}
		// ---- "data" ----
		else if (memcmp(id, "data", 4) == 0)
		{
			Metadata.dataSize = size;
			res = f_lseek_ext(file, size, SEEK_CUR);
		}
		// ---- "LIST" ----
		else if (memcmp(id, "LIST", 4) == 0)
		{
			FSIZE_t list_end = f_tell(file) + size;
			char list_type[4];
			res = f_read(file, list_type, 4, &br);
			if (memcmp(list_type, "INFO", 4) != 0)
			{
				res = f_lseek_ext(file, size - 4, SEEK_CUR);
				continue;
			}

			while (f_tell(file) < list_end)
			{
				char sub_id[4];
				ui32 sub_size;
				res = f_read(file, sub_id, 4, &br);
				res = f_read(file, &sub_size, 4, &br);

				if (memcmp(sub_id, "INAM", 4) == 0)
				{
					res = f_read(file, Metadata.title, (sub_size < 63) ? sub_size : 63, &br);
					Metadata.title[(br < 63) ? br : 63] = '\0';
				}
				else if (memcmp(sub_id, "IART", 4) == 0)
				{
					res = f_read(file, Metadata.artist, (sub_size < 63) ? sub_size : 63, &br);
					Metadata.artist[(br < 63) ? br : 63] = '\0';
				}
				else res = f_lseek_ext(file, sub_size, SEEK_CUR);

				if (sub_size % 2 != 0) f_lseek_ext(file, 1, SEEK_CUR);
			}
			f_lseek(file, list_end);
		}

		else if (memcmp(id, "id3 ", 4) == 0)
		{
		    Metadata.pictureFlag    = true;
		    Metadata.picture.offset = f_tell(file);
		    Metadata.picture.size   = size;

		    // need parser from ParseMP3_Meta chapter APIC

		    res = f_lseek_ext(file, size, SEEK_CUR);
		}

		else if (memcmp(id, "PICT", 4) == 0)
		{
		    Metadata.pictureFlag    = true;
		    Metadata.picture.offset = f_tell(file);
		    Metadata.picture.size   = size;

		    ui8 sig[8];
		    res = f_read(file, sig, (size > 8) ? 8 : size, &br);
		    f_lseek(file, Metadata.picture.offset);

		    if (!memcmp(sig, "\x89PNG", 4))
		    {
		    	snprintf(Metadata.picture.mime, sizeof(Metadata.picture.mime), "image/png");
		        ui16 w = 0, h = 0;
		        if (ReadSignaturePNG(file, Metadata.picture.offset, Metadata.picture.size, &w, &h))
		        {
		            Metadata.picture.width  = w;
		            Metadata.picture.height = h;
		        }
		    }
		    else if (!memcmp(sig, "\xFF\xD8", 2))
		    {
		    	snprintf(Metadata.picture.mime, sizeof(Metadata.picture.mime), "image/jpeg");
		        ui16 w = 0, h = 0;
		        if (ReadSignatureJPEG(file, Metadata.picture.offset, Metadata.picture.size, &w, &h))
		        {
		            Metadata.picture.width  = w;
		            Metadata.picture.height = h;
		        }
		    }
		    else
		    {
		    	snprintf(Metadata.picture.mime, sizeof(Metadata.picture.mime), "unknown");
		    }

		    res = f_lseek_ext(file, size, SEEK_CUR);
		}

		else res = f_lseek_ext(file, size, SEEK_CUR);

		if (size % 2 != 0) f_lseek_ext(file, 1, SEEK_CUR);
	}

	if (Metadata.byteRate) Metadata.durationSec = (float)Metadata.dataSize / (float)Metadata.byteRate;

	return FR_OK;
}


void qcParse::ScanFolders (const char *rootPath)
{
	DIR dir;
	FILINFO fno;

	if (f_opendir(&dir, rootPath) != FR_OK)
		return;

	folderCount = 0;

	while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0] != 0 && folderCount < MAX_FOLDERS)
	{
		if (fno.fattrib & AM_DIR)
		{
			snprintf(folders[folderCount], MAX_NAME_LEN, "/%s/%s", rootPath, fno.fname);
			folderCount++;
		}
	}

	f_closedir(&dir);
}


void qcParse::CopyID3TextField (FIL* file, ui32 dataPosition, ui32 frameSize, char *string, size_t strinLen)
{
    if (strinLen == 0 || frameSize == 0) return;
    if (f_lseek_ext(file, dataPosition, SEEK_SET) != FR_OK) return;

    ui8 enc=0;
    if (SafeRead_u8(file, &enc) != 0) return;
    if (frameSize <= 1)
    {
    	if (strinLen) string[0] = '\0';
    	return;
    }

    ui32 remain = frameSize - 1;

    size_t w = 0;
    if (enc == 0 || enc == 3)
    {
    	// ISO-8859-1 / UTF-8
        ui32 need = (remain < (strinLen - 1)) ? remain : (ui32)(strinLen - 1);
        if (need > 0)
        {
            if (SafeReadBytes(file, string, (UINT)need) == 0)
            {
                size_t i = 0;
                for (; i < need; ++i)
                {
                	if ((unsigned char)string[i] == 0) break;
                }
                string[i] = '\0';
            }
            else if (strinLen) string[0] = '\0';
        }
        else if (strinLen) string[0] = '\0';
    }
    else
    {
    	// UTF-16 with BOM (1) or UTF-16BE without BOM (2)
        int big_endian = (enc == 2) ? 1 : -1;
        while (remain >= 2 && w < strinLen-1)
        {
            ui8 cu[2];
            if (SafeReadBytes(file, cu, 2)!=0) break;
            remain -= 2;
            if (big_endian < 0)
            {
                // serch BOM
                if (cu[0] == 0xFE && cu[1] == 0xFF) { big_endian = 1; continue; }
                if (cu[0] == 0xFF && cu[1] == 0xFE) { big_endian = 0; continue; }
                big_endian = 0;
            }
            uint16_t u = big_endian ? TransformBigEndianTo16Bit(cu) : (uint16_t)((cu[1] << 8) | cu[0]);
            if (u == 0x0000) break;

            if (u >= 0xD800 && u <= 0xDBFF)
            {
                if (remain < 2) break;
                if (SafeReadBytes(file, cu, 2) != 0) break;
                remain -= 2;
                uint16_t u2 = big_endian ? TransformBigEndianTo16Bit(cu) : (uint16_t)((cu[1] << 8) | cu[0]);
                if (u2 >= 0xDC00 && u2 <= 0xDFFF)
                {
                    ui32 cp = 0x10000 + (((ui32)u-0xD800) << 10) + ((ui32)u2 - 0xDC00);
                    char tmp[4];
                    size_t k = TransformCodePointToUTF8(cp, tmp, sizeof(tmp));
                    if (k == 0) break;
                    if (w+k >= strinLen - 1) break;
                    memcpy(string + w, tmp, k); w += k;
                }
                else
                {
                    if (w+1 < strinLen-1){ string[w++]='?'; }
                }
            }
            else
            {
                char tmp[3]; size_t k = TransformCodePointToUTF8(u, tmp, sizeof(tmp));
                if (k == 0) break;
                if (w + k >= strinLen - 1) break;
                memcpy(string + w, tmp, k); w += k;
            }
        }
        string[w] = '\0';
    }
}


int qcParse::ParseFrameHeaderSizeID (char *outID, ui32 *outSize, ui32 versionMajor, const ui8 *header, UINT headerRead)
{
    if (versionMajor == 2)
    {
        if (headerRead < 6) return -1;
        memcpy(outID, header, 3); outID[3] = '\0';
        *outSize = (ui32)(header[3] << 16) | (ui32)(header[4] << 8) | header[5];
        return 6;
    }

    else
    {
        if (headerRead < 10) return -1;
        memcpy(outID, header, 4); outID[4] = '\0';
        if (versionMajor == 4)
            *outSize = DecodeSyncsafeTo_u32((ui8*)header + 4);
        else
            *outSize = TransformBigEndianTo32Bit(header + 4);
        return 10;
    }
}
