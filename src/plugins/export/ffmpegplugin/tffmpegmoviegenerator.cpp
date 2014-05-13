/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#define INT64_C
#define __STDC_CONSTANT_MACROS
#include <stdint.h>

#include "tffmpegmoviegenerator.h"
#include "tdebug.h"
#include "talgorithm.h"

#include <QDir>

#ifdef __cplusplus
extern "C" {
#include "libavutil/mathematics.h"
#include "libavformat/avformat.h"
}
#endif

struct TFFMpegMovieGenerator::Private
{
    AVFrame *frame;

    QString movieFile;
    int fps;
    double video_pts;
    int frameCount;
    double streamDuration;
    bool exception;
    const char *errorMsg;

    AVStream *video_st;
    AVFormatContext *oc;
    AVOutputFormat *fmt;

    void chooseFileExtension(int format);
    bool openVideo(AVCodec *codec, AVStream *st);
    void RGBtoYUV420P(const uint8_t *bufferRGB, uint8_t *bufferYUV, uint iRGBIncrement, bool bSwapRGB, int width, int height);
    bool writeVideoFrame(const QString &movieFile, const QImage &image);
    void closeVideo(AVStream *st);
};

static AVStream *addVideoStream(AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id, const QString &movieFile, int width, int height, int fps, const char *errorMsg)
{
    AVCodecContext *c;
    AVStream *st;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        errorMsg = "ffmpeg error: Could not find encoder. This is not a problem directly related to Tupi. \
                    Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::addVideoStream() - " << errorMsg;
        #endif
        return 0;
    }

    st = avformat_new_stream(oc, *codec);
    if (!st) {
        errorMsg = "ffmpeg error: Could not alloc stream. This is not a problem directly related to Tupi. \
                    Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::addVideoStream() - " << errorMsg;
        #endif
        return 0;
    }
    st->id = oc->nb_streams-1;
    c = st->codec;

    // c->codec_type = AVMEDIA_TYPE_VIDEO;
    c->codec_id = codec_id;

    /* put sample parameters */
    c->bit_rate = 6000000;

    /* resolution must be a multiple of two */
    c->width = width;  
    c->height = height; 

    c->time_base.den = fps;
    c->time_base.num = 1;
    c->gop_size = 12;

    if (movieFile.endsWith("gif", Qt::CaseInsensitive)) {
        c->pix_fmt = AV_PIX_FMT_RGB24;
    } else {
        c->pix_fmt = AV_PIX_FMT_YUV420P;
    }

    // if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
    if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {

	/* just for testing, we also add B frames */
	c->max_b_frames = 2;
    }

    // if (c->codec_id == CODEC_ID_MPEG1VIDEO) {
    if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
           c->mb_decision=2;
    }

    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
	
    return st;
}

void TFFMpegMovieGenerator::Private::chooseFileExtension(int format)
{
    switch (format) {
            case WEBM:
                 movieFile += ".webm";
                 break;
            case OGV:
                 movieFile += ".ogv";
                 break;
            case SWF:
                 movieFile += ".swf";
                 break;
            case MPEG:
                 movieFile += ".mpg";
                 break;
            case ASF:
                 movieFile += ".asf";
                 break;
            case AVI:
                 movieFile += ".avi";
                 break;
            case MOV:
                 movieFile += ".mov";
                 break;
            case GIF:
                 movieFile += ".gif";
                 break;
            default:
                 movieFile += ".mpg";
                 break;
    }
}

bool TFFMpegMovieGenerator::Private::openVideo(AVCodec *codec, AVStream *st)
{
    int ret;
    AVCodecContext *c = st->codec;

    /* open the codec */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        errorMsg = "The video codec required is not installed in your system. \
                    Please, check your ffmpeg installation and codec support. \
                    More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::openVideo() - " << errorMsg;
        #endif

        return false;
    }

    /* allocate and init a re-usable frame */
    frame = avcodec_alloc_frame();
    if (!frame) {
        errorMsg = "There is no available memory to export your project as a video";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::openVideo() - " << errorMsg;
        #endif

        return false;
    }
	
    return true;
}

#define RGBtoYUV(r, g, b, y, u, v) \
  y = (uint8_t)(((int)30*r + (int)59*g + (int)11*b)/100); \
  u = (uint8_t)(((int)-17*r - (int)33*g + (int)50*b + 12800)/100); \
  v = (uint8_t)(((int)50*r - (int)42*g - (int)8*b + 12800)/100);

void TFFMpegMovieGenerator::Private::RGBtoYUV420P(const uint8_t *bufferRGB, uint8_t *bufferYUV, uint iRGBIncrement, bool bSwapRGB, int width, int height)
{
    const unsigned iPlaneSize = width * height;
    const unsigned iHalfWidth = width >> 1;

    // get pointers to the data
    uint8_t *yplane  = bufferYUV;
    uint8_t *uplane  = bufferYUV + iPlaneSize;
    uint8_t *vplane  = bufferYUV + iPlaneSize + (iPlaneSize >> 2);
    const uint8_t *bufferRGBIndex = bufferRGB;

    int iRGBIdx[3];
    iRGBIdx[0] = 0;
    iRGBIdx[1] = 1;
    iRGBIdx[2] = 2;
    if (bSwapRGB)  {
        iRGBIdx[0] = 2;
        iRGBIdx[2] = 0;
    }
 
    for (int y = 0; y < (int) height; y++) {
         uint8_t *yline  = yplane + (y * width);
         uint8_t *uline  = uplane + ((y >> 1) * iHalfWidth);
         uint8_t *vline  = vplane + ((y >> 1) * iHalfWidth);

         for (int x=0; x<width; x+=2) {
              RGBtoYUV(bufferRGBIndex[iRGBIdx[0]], bufferRGBIndex[iRGBIdx[1]], bufferRGBIndex[iRGBIdx[2]], *yline, *uline, *vline);
              bufferRGBIndex += iRGBIncrement;
              yline++;
              RGBtoYUV(bufferRGBIndex[iRGBIdx[0]], bufferRGBIndex[iRGBIdx[1]], bufferRGBIndex[iRGBIdx[2]], *yline, *uline, *vline);
              bufferRGBIndex += iRGBIncrement;
              yline++;
              uline++;
              vline++;
         }
    }
}

bool TFFMpegMovieGenerator::Private::writeVideoFrame(const QString &movieFile, const QImage &image)
{
    #ifdef K_DEBUG
           tWarning() << "TFFMpegMovieGenerator::writeVideoFrame() - Generating frame #" << frameCount;
    #endif

    int ret;
    AVCodecContext *c = video_st->codec;

    /* encode the image */
    AVPacket pkt;
    int got_output;

    av_init_packet(&pkt);
    pkt.data = NULL;    // packet data will be allocated by the encoder
    pkt.size = 0;

    int w = c->width;
    int h = c->height;

    if (movieFile.endsWith("gif", Qt::CaseInsensitive)) {
        // c->pix_fmt = AV_PIX_FMT_RGB24;
        // avpicture_fill((AVPicture *)frame, image.bits(), AV_PIX_FMT_RGB24, w, h);
    } else {
        int size = avpicture_get_size(PIX_FMT_YUV420P, w, h);
        uint8_t *pic_dat = (uint8_t *) av_malloc(size);
        RGBtoYUV420P(image.bits(), pic_dat, image.depth()/8, true, w, h);
        avpicture_fill((AVPicture *)frame, pic_dat, PIX_FMT_YUV420P, w, h);
    }

    ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
    if (ret < 0) {
        errorMsg = "[1] Error while encoding the video of your project";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::writeVideoFrame() - " << errorMsg;
        #endif

        return false;
    }

    /* If size is zero, it means the image was buffered. */
    if (got_output) {
        if (c->coded_frame->key_frame)
            pkt.flags |= AV_PKT_FLAG_KEY;

        pkt.stream_index = video_st->index;

        /* Write the compressed frame to the media file. */
        ret = av_interleaved_write_frame(oc, &pkt);
    } else {
        ret = 0;
    }

    if (ret != 0) {
        errorMsg = "[2] Error while encoding the video of your project";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::writeVideoFrame() - " << errorMsg;
        #endif

        return false;
    }

    frameCount++;

    return true;
}

void TFFMpegMovieGenerator::Private::closeVideo(AVStream *st)
{
    avcodec_close(st->codec);
    av_free(frame);
}

TFFMpegMovieGenerator::TFFMpegMovieGenerator(TMovieGeneratorInterface::Format format, int width, int height, int fps, double duration)
 : TMovieGenerator(width, height), k(new Private)
{
    k->movieFile = QDir::tempPath() + QDir::separator() + "tupi_video_" + TAlgorithm::randomString(12);
    k->chooseFileExtension(format);
    k->fps = fps;
    k->streamDuration = duration;
    k->exception = begin();
}

TFFMpegMovieGenerator::TFFMpegMovieGenerator(TMovieGeneratorInterface::Format format, const QSize &size, int fps, double duration) : TMovieGenerator(size.width(), size.height()), k(new Private)
{
    k->movieFile = QDir::tempPath() + QDir::separator() + "tupi_video_" + TAlgorithm::randomString(12);
    k->chooseFileExtension(format);
    k->fps = fps;
    k->streamDuration = duration;
    k->exception = begin();
}

TFFMpegMovieGenerator::~TFFMpegMovieGenerator()
{
    if (QFile::exists(k->movieFile)) 
        QFile::remove(k->movieFile);

    delete k;
}

bool TFFMpegMovieGenerator::begin()
{
    int ret;
    AVCodec *video_codec = 0;

    av_register_all();

    k->fmt = av_guess_format(NULL, k->movieFile.toLocal8Bit().data(), NULL);
    if (!k->fmt) {
        k->fmt = av_guess_format("mpeg", NULL, NULL);
    }

    if (!k->fmt) {
        k->errorMsg = "ffmpeg error: Error while doing export. This is not a problem directly related to Tupi. \
                       Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        return false;
    }

    k->oc = avformat_alloc_context();
    if (!k->oc) {
        fprintf(stderr, "Memory error\n");
        return false;
    }
    k->oc->oformat = k->fmt;

    if (!k->oc) {
        k->errorMsg = "ffmpeg error: Error while doing export. This is not a problem directly related to Tupi. \
                       Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::begin() - " << k->errorMsg;
        #endif
        return false;
    }

    k->video_st = NULL;
    // if (k->fmt->video_codec != CODEC_ID_NONE) 
    if (k->fmt->video_codec != AV_CODEC_ID_NONE) 
        k->video_st = addVideoStream(k->oc, &video_codec, k->fmt->video_codec, k->movieFile, width(), height(), k->fps, k->errorMsg);

    av_dump_format(k->oc, 0, k->movieFile.toLocal8Bit().data(), 1); 
	
    if (k->video_st) {
        k->openVideo(video_codec, k->video_st);
    } else {
        k->errorMsg = "ffmpeg error: Can't add video stream. This is not a problem directly related to Tupi. \
                       Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::begin() - " << k->errorMsg;
        #endif
        return false;
    }

    if (!(k->fmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&k->oc->pb, k->movieFile.toLocal8Bit().data(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            k->errorMsg = "ffmpeg error: could not open video file";
            #ifdef K_DEBUG
                   tError() << "TFFMpegMovieGenerator::begin() - " << k->errorMsg;
            #endif
            return false;
        }
    }

    avformat_write_header(k->oc, NULL);

    if (k->frame)
        k->frame->pts = 0;

    k->frameCount = 0;

    return true;
}

bool TFFMpegMovieGenerator::movieHeaderOk() 
{ 
    return k->exception;
}

const char* TFFMpegMovieGenerator::getErrorMsg() 
{
    return k->errorMsg;
}

void TFFMpegMovieGenerator::handle(const QImage& image)
{
    if (k->video_st) 
        k->video_pts = (double)k->video_st->pts.val * k->video_st->time_base.num / k->video_st->time_base.den;
    else 
        k->video_pts = 0.0;

    if (!k->video_st || k->video_pts >= k->streamDuration) {
        #ifdef K_DEBUG
               tWarning() << "TFFMpegMovieGenerator::handle() - The total of frames has been processed (" << k->streamDuration << " seg)";
        #endif

        return;
    }

    #ifdef K_DEBUG
           tWarning() << "Duration: " << k->streamDuration;
           tWarning() << "Video PTS: " << k->video_pts;
    #endif

    k->writeVideoFrame(k->movieFile, image);
    k->frame->pts += av_rescale_q(1, k->video_st->codec->time_base, k->video_st->time_base);
}

void TFFMpegMovieGenerator::end()
{
    av_write_trailer(k->oc);

    if (k->video_st)
        k->closeVideo(k->video_st);

    if (!(k->fmt->flags & AVFMT_NOFILE))
        avio_close(k->oc->pb);

    avformat_free_context(k->oc);
}

void TFFMpegMovieGenerator::__saveMovie(const QString &fileName)
{
    if (QFile::exists(fileName)) 
        QFile::remove(fileName);

    QFile::copy(k->movieFile, fileName);
}
