
#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <unistd.h>

extern "C"
{
#include <libavutil/log.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavformat/avio.h>


#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
#define LOGD(format, ...)  __android_log_print(ANDROID_LOG_DEBUG,  "(^_^)", format, ##__VA_ARGS__)

const char *filters_descr = "lutyuv='u=128:v=128'";
//const char *filters_descr = "hflip";
//const char *filters_descr = "hue='h=60:s=-3'";
//const char *filters_descr = "crop=2/3*in_w:2/3*in_h";
//const char *filters_descr = "drawbox=x=200:y=200:w=300:h=300:color=pink@0.5";
//const char *filters_descr = "movie=/storage/emulated/0/ws.jpg[wm];[in][wm]overlay=5:5[out]";
//const char *filters_descr="drawgrid=width=100:height=100:thickness=4:color=pink@0.9";

void custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("/storage/emulated/0/av_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
}
const char *type_string(int type) {
    switch (type) {
        case AVIO_ENTRY_DIRECTORY:
            return "<DIR>";
        case AVIO_ENTRY_FILE:
            return "<FILE>";
        case AVIO_ENTRY_BLOCK_DEVICE:
            return "<BLOCK DEVICE>";
        case AVIO_ENTRY_CHARACTER_DEVICE:
            return "<CHARACTER DEVICE>";
        case AVIO_ENTRY_NAMED_PIPE:
            return "<PIPE>";
        case AVIO_ENTRY_SYMBOLIC_LINK:
            return "<LINK>";
        case AVIO_ENTRY_SOCKET:
            return "<SOCKET>";
        case AVIO_ENTRY_SERVER:
            return "<SERVER>";
        case AVIO_ENTRY_SHARE:
            return "<SHARE>";
        case AVIO_ENTRY_WORKGROUP:
            return "<WORKGROUP>";
        case AVIO_ENTRY_UNKNOWN:
        default:
            break;
    }
    return "<UNKNOWN>";
}

static AVFormatContext *fmt_ctx;
static AVCodecContext *dec_ctx;
static int video_stream_index = -1;
static AVFilterContext *buffersink_ctx;
static AVFilterContext *buffersrc_ctx;
static AVFilterGraph *filter_graph;

static int open_input_file(const char *filename) {
    int ret;
    AVCodec *dec;

    if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    /* select the video stream */
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
        return ret;
    }
    video_stream_index = ret;

    /* create decoding context */
    dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx)
        return AVERROR(ENOMEM);
    avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_stream_index]->codecpar);

    /* init the video decoder */
    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
        return ret;
    }

    return 0;
}



JNIEXPORT jint JNICALL
Java_com_wanwan_ffmpeglib_Examples_aviolist(JNIEnv *env, jobject, jstring input_jstr) {
    char file_name[500] = {0};
//    char *file_name = "/storage/emulated/0/DCIM/";

    sprintf(file_name, "%s", env->GetStringUTFChars(input_jstr, NULL));

    LOGE("input dir = %s", input_jstr);
    LOGE("file_name dir = %s", file_name);

    AVIODirEntry *entry = NULL;
    AVIODirContext *ctx = NULL;
    int cnt, ret;
    char filemode[4], uid_and_gid[20];

    if ((ret = avio_open_dir(&ctx, file_name, NULL)) < 0) {
        LOGE("avio_open_dir filed! ret = %s", av_err2str(ret));
        av_log(NULL, AV_LOG_ERROR, "Cannot open directory: %s.\n", av_err2str(ret));
        goto fail;
    }

    cnt = 0;
    for (;;) {
        if ((ret = avio_read_dir(ctx, &entry)) < 0) {
            LOGE("Cannot list directory:  %s", av_err2str(ret));
            av_log(NULL, AV_LOG_ERROR, "Cannot list directory: %s.\n", av_err2str(ret));
            goto fail;
        }
        if (!entry)
            break;
        if (entry->filemode == -1) {
            snprintf(filemode, 4, "???");
        } else {
            snprintf(filemode, 4, "%3" PRIo64, entry->filemode);
        }
        snprintf(uid_and_gid, 20, "%" PRId64"(%" PRId64")", entry->user_id, entry->group_id);
        if (cnt == 0)
            av_log(NULL, AV_LOG_INFO, "%-9s %12s %30s %10s %s %16s %16s %16s\n",
                   "TYPE", "SIZE", "NAME", "UID(GID)", "UGO", "MODIFIED",
                   "ACCESSED", "STATUS_CHANGED");
        LOGE("av log type = %9s , size = %12" PRId64" , name = %30s , uidgid = %10s , filemode = %s , mTime = %16" PRId64" , aTime = %16" PRId64" , cTime = %16" PRId64,
             type_string(entry->type),
             entry->size,
             entry->name,
             uid_and_gid,
             filemode,
             entry->modification_timestamp,
             entry->access_timestamp,
             entry->status_change_timestamp);

        avio_free_directory_entry(&entry);
        cnt++;
    };

    fail:
    avio_close_dir(&ctx);
    return ret;
}


JNIEXPORT jint JNICALL
Java_com_wanwan_ffmpeglib_Examples_play
        (JNIEnv *env, jobject, jobject surface, jstring input_jstr) {
    LOGE("play");
    char file_name[500] = {0};
    sprintf(file_name, "%s", env->GetStringUTFChars(input_jstr, NULL));
    // sd卡中的视频文件地址,可自行修改或者通过jni传入
    //char *file_name = "/storage/emulated/0/ws2.mp4";

    av_register_all();

    AVFormatContext *pFormatCtx = avformat_alloc_context();

    // Open video file
    if (avformat_open_input(&pFormatCtx, file_name, NULL, NULL) != 0) {

        LOGE("Couldn't open file:%s\n", file_name);
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.");
        return -1;
    }

    // Find the first video stream
    int videoStream = -1, i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO
            && videoStream < 0) {
            videoStream = i;
        }
    }
    if (videoStream == -1) {
        LOGE("Didn't find a video stream.");
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOGE("Codec not found.");
        return -1; // Codec not found
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }



    // 获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }

    // Allocate video frame
    AVFrame *pFrame = av_frame_alloc();

    // 用于渲染
    AVFrame *pFrameRGBA = av_frame_alloc();
    if (pFrameRGBA == NULL || pFrame == NULL) {
        LOGE("Could not allocate video frame.");
        return -1;
    }

    // Determine required buffer size and allocate buffer
    // buffer中数据就是用于渲染的,且格式为RGBA
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height,
                                            1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    int frameFinished;
    AVPacket packet;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {

            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // 并不是decode一次就可解码出一帧
            if (frameFinished) {

                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                // 格式转换
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }

        }
        av_packet_unref(&packet);
    }

    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    return 0;
}


JNIEXPORT jint JNICALL
Java_com_wanwan_ffmpeglib_Examples_filterplay
        (JNIEnv *env, jobject, jobject surface, jstring input_file_jstr, jstring input_filter_jstr) {
    LOGE("filterplay");
    int ret = 0;
    char file_name[500] = {0};
    char filter_name[500] = {0};
    sprintf(file_name, "%s", env->GetStringUTFChars(input_file_jstr, NULL));
    sprintf(filter_name, "%s", env->GetStringUTFChars(input_filter_jstr, NULL));
    LOGD("file_name = %s\n", file_name);
    LOGD("filter_name = %s\n", filter_name);
    av_register_all();

    avfilter_register_all();//added by ws for AVfilter

    AVFormatContext *pFormatCtx = avformat_alloc_context();

    // Open video file
    if (avformat_open_input(&pFormatCtx, file_name, NULL, NULL) != 0) {

        LOGD("Couldn't open file:%s\n", file_name);
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGD("Couldn't find stream information.");
        return -1;
    }

    // Find the first video stream
    int videoStream = -1, i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO
            && videoStream < 0) {
            videoStream = i;
        }
    }
    if (videoStream == -1) {
        LOGD("Didn't find a video stream.");
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    //added by ws for AVfilter start----------init AVfilter--------------------------ws


    char args[512];
    const  AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const  AVFilter *buffersink = avfilter_get_by_name("buffersink");//新版的ffmpeg库必须为buffersink
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    AVBufferSinkParams *buffersink_params;

    filter_graph = avfilter_graph_alloc();

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
             pCodecCtx->time_base.num, pCodecCtx->time_base.den,
             pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGD("Cannot create buffer source\n");
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0) {
        LOGD("Cannot create buffer sink\n");
        return ret;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    // avfilter_link(buffersrc_ctx, 0, buffersink_ctx, 0);

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_name,
                                        &inputs, &outputs, NULL)) < 0) {
        LOGD("Cannot avfilter_graph_parse_ptr:%d\n",ret);
        return ret;
    }

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        LOGD("Cannot avfilter_graph_config\n");
        return ret;
    }

    //added by ws for AVfilter start------------init AVfilter------------------------------ws

    // Find the decoder for the video stream
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOGD("Codec not found.");
        return -1; // Codec not found
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGD("Could not open codec.");
        return -1; // Could not open codec
    }

    // 获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGD("Could not open codec.");
        return -1; // Could not open codec
    }

    // Allocate video frame
    AVFrame *pFrame = av_frame_alloc();

    // 用于渲染
    AVFrame *pFrameRGBA = av_frame_alloc();
    if (pFrameRGBA == NULL || pFrame == NULL) {
        LOGD("Could not allocate video frame.");
        return -1;
    }

    // Determine required buffer size and allocate buffer
    // buffer中数据就是用于渲染的,且格式为RGBA
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height,
                                            1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    int frameFinished;
    AVPacket packet;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {

            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);


            // 并不是decode一次就可解码出一帧
            if (frameFinished) {

                //added by ws for AVfilter start
                pFrame->pts = av_frame_get_best_effort_timestamp(pFrame);

                //* push the decoded frame into the filtergraph
                if (av_buffersrc_add_frame(buffersrc_ctx, pFrame) < 0) {
                    LOGD("Could not av_buffersrc_add_frame");
                    break;
                }

                ret = av_buffersink_get_frame(buffersink_ctx, pFrame);
                if (ret < 0) {
                    LOGD("Could not av_buffersink_get_frame");
                    break;
                }
                //added by ws for AVfilter end

                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                // 格式转换
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }

        }
        av_packet_unref(&packet);
    }

    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    avfilter_graph_free(&filter_graph); //added by ws for avfilter
    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    return 0;

}


}
