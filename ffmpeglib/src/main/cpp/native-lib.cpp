
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


#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)

const char *filters_descr = "scale=78:24,transpose=cclock";

//static AVFormatContext *pFormatCtx;
//static AVCodecContext *pCodecCtx;
//static AVFilterContext *buffersink_ctx;
//static AVFilterContext *buffersrc_ctx;
//static AVFilterGraph *filter_graph;
//static int video_stream_index = -1;
//static int64_t last_pts = AV_NOPTS_VALUE;

AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx;
AVFilterGraph *filter_graph;


//Output FFmpeg's av_log()
void custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("/storage/emulated/0/av_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
}

/**
 * 通过filename获取AVFormatContext和AVCodecContext对象
 * */
//static int open_input_file(const char *filename) {
//    int ret;
//    AVCodec *dec;
//    LOGE("open_input_file filename = %s",filename);
//    if ((ret = avformat_open_input(&pFormatCtx, filename, NULL, NULL)) < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
//        return ret;
//    }
//
//    if ((ret = avformat_find_stream_info(pFormatCtx, NULL)) < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
//        return ret;
//    }
//
//    /* select the video stream */
//    ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
//    if (ret < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
//        return ret;
//    }
//    video_stream_index = ret;
//
//    /* create decoding context */
//    pCodecCtx = avcodec_alloc_context3(dec);
//    if (!pCodecCtx)
//        return AVERROR(ENOMEM);
//    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[video_stream_index]->codecpar);
//
//    /* init the video decoder */
//    if ((ret = avcodec_open2(pCodecCtx, dec, NULL)) < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
//        return ret;
//    }
//
//    return 0;
//}
/**
 * 初始化filter_graph
 * */
//static int init_filters(const char *filters_descr)
//{
//    char args[512];
//    int ret = 0;
//    LOGE("init_filters filters_descr = %s",filters_descr);
//    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
//    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
//    AVFilterInOut *outputs = avfilter_inout_alloc();
//    AVFilterInOut *inputs  = avfilter_inout_alloc();
//    LOGE("init_filters AVFilterInOut init");
//    AVRational time_base = pFormatCtx->streams[video_stream_index]->time_base;
//    LOGE("init_filters video_stream_index = %d",video_stream_index);
//    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };
//    AVBufferSinkParams *buffersink_params;
//
//    filter_graph = avfilter_graph_alloc();
//    LOGE("init_filters avfilter_graph_alloc finsh");
//    if (!outputs || !inputs || !filter_graph) {
//        ret = AVERROR(ENOMEM);
//        LOGE("init_filters ret = %d",ret);
//        avfilter_inout_free(&inputs);
//        avfilter_inout_free(&outputs);
//        return ret;
//    }
//
//    /* buffer video source: the decoded frames from the decoder will be inserted here. */
//    snprintf(args, sizeof(args),
//             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
//             pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//             pCodecCtx->time_base.num, pCodecCtx->time_base.den,
//             pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den);
//
//    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
//                                       args, NULL, filter_graph);
//    if (ret < 0) {
//        LOGE("Cannot create buffer source:%d\n",ret);
//        return ret;
//    }
//
//    /* buffer video sink: to terminate the filter chain. */
//    buffersink_params = av_buffersink_params_alloc();
//    buffersink_params->pixel_fmts = pix_fmts;
//    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
//                                       NULL, buffersink_params, filter_graph);
//    av_free(buffersink_params);
//    if (ret < 0) {
//        LOGE("Cannot create buffer sink\n");
//        return ret;
//    }
//
//    /* Endpoints for the filter graph. */
//    outputs->name       = av_strdup("in");
//    outputs->filter_ctx = buffersrc_ctx;
//    outputs->pad_idx    = 0;
//    outputs->next       = NULL;
//
//    inputs->name       = av_strdup("out");
//    inputs->filter_ctx = buffersink_ctx;
//    inputs->pad_idx    = 0;
//    inputs->next       = NULL;
//
//    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
//                                        &inputs, &outputs, NULL)) < 0) {
//        avfilter_inout_free(&inputs);
//        avfilter_inout_free(&outputs);
//    }
//    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
//        avfilter_inout_free(&inputs);
//        avfilter_inout_free(&outputs);
//    }
//
//
//
//    return ret;
//}

JNIEXPORT jint JNICALL
Java_com_wanwan_ffmpeglib_FfmpegUtils_decode(
        JNIEnv *env,
        jobject, jstring input_jstr, jstring output_jstr) {

    AVFormatContext *pFormatCtx;
    int i, videoindex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    FILE *fp_yuv;
    int frame_cnt;
    clock_t time_start, time_finish;
    double time_duration = 0.0;

    char input_str[500] = {0};
    char output_str[500] = {0};
    char info[1000] = {0};
    sprintf(input_str, "%s", env->GetStringUTFChars(input_jstr, NULL));
    sprintf(output_str, "%s", env->GetStringUTFChars(output_jstr, NULL));
    LOGE("inputStr = %s", input_str);
    LOGE("outputStr = %s", output_str);

    //FFmpeg av_log() callback
    av_log_set_callback(custom_log);

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, input_str, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.\n");
        return -1;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }
    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    if (videoindex == -1) {
        LOGE("Couldn't find a video stream.\n");
        return -1;
    }
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOGE("codec_id = %d\n", pCodecCtx->codec_id);
        LOGE("Couldn't find Codec.\n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Couldn't open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    out_buffer = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
                         AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);


    packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);


    sprintf(info, "[Input     ]%s\n", input_str);
    sprintf(info, "%s[Output    ]%s\n", info, output_str);
    sprintf(info, "%s[Format    ]%s\n", info, pFormatCtx->iformat->name);
    sprintf(info, "%s[Codec     ]%s\n", info, pCodecCtx->codec->name);
    sprintf(info, "%s[Resolution]%dx%d\n", info, pCodecCtx->width, pCodecCtx->height);


    fp_yuv = fopen(output_str, "wb+");
    if (fp_yuv == NULL) {
        printf("Cannot open output file.\n");
        return -1;
    }

    frame_cnt = 0;
    time_start = clock();

    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == videoindex) {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if (ret < 0) {
                LOGE("Decode Error.\n");
                return -1;
            }
            if (got_picture) {
                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                          0, pCodecCtx->height,
                          pFrameYUV->data, pFrameYUV->linesize);

                y_size = pCodecCtx->width * pCodecCtx->height;
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
                fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
                //Output info
                char pictype_str[10] = {0};
                switch (pFrame->pict_type) {
                    case AV_PICTURE_TYPE_I:
                        sprintf(pictype_str, "I");
                        break;
                    case AV_PICTURE_TYPE_P:
                        sprintf(pictype_str, "P");
                        break;
                    case AV_PICTURE_TYPE_B:
                        sprintf(pictype_str, "B");
                        break;
                    default:
                        sprintf(pictype_str, "Other");
                        break;
                }
                LOGI("Frame Index: %5d. Type:%s", frame_cnt, pictype_str);
                frame_cnt++;
            }
        }
        av_free_packet(packet);
    }
    //flush decoder
    //FIX: Flush Frames remained in Codec
    while (1) {
        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        if (ret < 0)
            break;
        if (!got_picture)
            break;
        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
                  pCodecCtx->height,
                  pFrameYUV->data, pFrameYUV->linesize);
        int y_size = pCodecCtx->width * pCodecCtx->height;
        fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
        fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
        fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
        //Output info
        char pictype_str[10] = {0};
        switch (pFrame->pict_type) {
            case AV_PICTURE_TYPE_I:
                sprintf(pictype_str, "I");
                break;
            case AV_PICTURE_TYPE_P:
                sprintf(pictype_str, "P");
                break;
            case AV_PICTURE_TYPE_B:
                sprintf(pictype_str, "B");
                break;
            default:
                sprintf(pictype_str, "Other");
                break;
        }
        LOGI("Frame Index: %5d. Type:%s", frame_cnt, pictype_str);
        frame_cnt++;
    }
    time_finish = clock();
    time_duration = (double) (time_finish - time_start);

    sprintf(info, "%s[Time      ]%fms\n", info, time_duration);
    sprintf(info, "%s[Count     ]%d\n", info, frame_cnt);

    sws_freeContext(img_convert_ctx);

    fclose(fp_yuv);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}


jstring
Java_com_wanwan_ffmpeglib_FfmpegUtils_avcodecinfo(
        JNIEnv *env, jobject) {
    char info[40000] = {0};

    av_register_all();
    AVCodec *c_temp = av_codec_next(NULL);

    while (c_temp != NULL) {
        if (c_temp->decode != NULL) {
            sprintf(info, "%sdecode:", info);
        } else {
            sprintf(info, "%sencode:", info);
        }
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s(video):", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s(audio):", info);
                break;
            default:
                sprintf(info, "%s(other):", info);
                break;
        }
        sprintf(info, "%s[%10s]\n", info, c_temp->name);
        c_temp = c_temp->next;
    }
    return env->NewStringUTF(info);
}

jint
Java_com_wanwan_ffmpeglib_FfmpegUtils_avcodecVersion(JNIEnv *env, jobject) {
    int version = LIBAVCODEC_VERSION_MAJOR * 1000 * 1000 + LIBAVCODEC_VERSION_MINOR * 1000 +
                  LIBAVCODEC_VERSION_MICRO;
    return version;
}


JNIEXPORT jint JNICALL
Java_com_wanwan_ffmpeglib_FfmpegUtils_play
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


//JNIEXPORT jint JNICALL
//Java_com_wanwan_ffmpeglib_FfmpegUtils_playAvFilter
//        (JNIEnv *env, jobject clazz, jobject surface, jstring input_jstr) {
//    LOGE("playAvFilter");
//
//    av_register_all();
//
//    avfilter_register_all();//added by ws for AVfilter
//
//    char file_name[500] = {0};
//    sprintf(file_name, "%s", env->GetStringUTFChars(input_jstr, NULL));
//
//    AVFormatContext *pFormatCtx = avformat_alloc_context();
//
//    // Open video file
//    if (avformat_open_input(&pFormatCtx, file_name, NULL, NULL) != 0) {
//
//        LOGE("Couldn't open file:%s\n", file_name);
//        return -1; // Couldn't open file
//    }
//
//    // Retrieve stream information
//    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
//        LOGE("Couldn't find stream information.");
//        return -1;
//    }
//
//    // Find the first video stream
//    int videoStream = -1, i;
//    for (i = 0; i < pFormatCtx->nb_streams; i++) {
//        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO
//            && videoStream < 0) {
//            videoStream = i;
//        }
//    }
//    if (videoStream == -1) {
//        LOGE("Didn't find a video stream.");
//        return -1; // Didn't find a video stream
//    }
//
//    // Get a pointer to the codec context for the video stream
//    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;
//
//    char args[512];
//    int ret;
//    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
//    const AVFilter *buffersink = avfilter_get_by_name("buffersink");//新版的ffmpeg库必须为buffersink
//    AVFilterInOut *outputs = avfilter_inout_alloc();
//    AVFilterInOut *inputs  = avfilter_inout_alloc();
//    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
//    AVBufferSinkParams *buffersink_params;
//
//    filter_graph = avfilter_graph_alloc();
//
//    /* buffer video source: the decoded frames from the decoder will be inserted here. */
//    snprintf(args, sizeof(args),
//             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
//             pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//             pCodecCtx->time_base.num, pCodecCtx->time_base.den,
//             pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den);
//
//    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
//                                       args, NULL, filter_graph);
//    if (ret < 0) {
//        LOGE("Cannot create buffer source\n");
//        return ret;
//    }
//
//    /* buffer video sink: to terminate the filter chain. */
//    buffersink_params = av_buffersink_params_alloc();
//    buffersink_params->pixel_fmts = pix_fmts;
//    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
//                                       NULL, buffersink_params, filter_graph);
//    av_free(buffersink_params);
//    if (ret < 0) {
//        LOGE("Cannot create buffer sink\n");
//        return ret;
//    }
//
//    /* Endpoints for the filter graph. */
//    outputs->name       = av_strdup("in");
//    outputs->filter_ctx = buffersrc_ctx;
//    outputs->pad_idx    = 0;
//    outputs->next       = NULL;
//
//    inputs->name       = av_strdup("out");
//    inputs->filter_ctx = buffersink_ctx;
//    inputs->pad_idx    = 0;
//    inputs->next       = NULL;
//
//    // avfilter_link(buffersrc_ctx, 0, buffersink_ctx, 0);
//
//    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
//                                        &inputs, &outputs, NULL)) < 0) {
//        LOGE("Cannot avfilter_graph_parse_ptr\n");
//        return ret;
//    }
//
//    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
//        LOGE("Cannot avfilter_graph_config\n");
//        return ret;
//    }
//
//    // Find the decoder for the video stream
//    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
//    if (pCodec == NULL) {
//        LOGE("Codec not found.");
//        return -1; // Codec not found
//    }
//
//    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
//        LOGE("Could not open codec.");
//        return -1; // Could not open codec
//    }
//
//    // 获取native window
//    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
//
//    // 获取视频宽高
//    int videoWidth = pCodecCtx->width;
//    int videoHeight = pCodecCtx->height;
//
//    // 设置native window的buffer大小,可自动拉伸
//    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
//                                     WINDOW_FORMAT_RGBA_8888);
//    ANativeWindow_Buffer windowBuffer;
//
//    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
//        LOGE("Could not open codec.");
//        return -1; // Could not open codec
//    }
//
//    // Allocate video frame
//    AVFrame *pFrame = av_frame_alloc();
//
//    // 用于渲染
//    AVFrame *pFrameRGBA = av_frame_alloc();
//    if (pFrameRGBA == NULL || pFrame == NULL) {
//        LOGE("Could not allocate video frame.");
//        return -1;
//    }
//
//    // Determine required buffer size and allocate buffer
//    // buffer中数据就是用于渲染的,且格式为RGBA
//    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height,
//                                            1);
//    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
//    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
//                         pCodecCtx->width, pCodecCtx->height, 1);
//
//    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
//    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
//                                                pCodecCtx->height,
//                                                pCodecCtx->pix_fmt,
//                                                pCodecCtx->width,
//                                                pCodecCtx->height,
//                                                AV_PIX_FMT_RGBA,
//                                                SWS_BILINEAR,
//                                                NULL,
//                                                NULL,
//                                                NULL);
//
//    int frameFinished;
//    AVPacket packet;
//    while (av_read_frame(pFormatCtx, &packet) >= 0) {
//        // Is this a packet from the video stream?
//        if (packet.stream_index == videoStream) {
//
//            // Decode video frame
//            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
//
//
//
//            // 并不是decode一次就可解码出一帧
//            if (frameFinished) {
//
//                //added by ws for AVfilter start
//                pFrame->pts = av_frame_get_best_effort_timestamp(pFrame);
//
//                //* push the decoded frame into the filtergraph
//                if (av_buffersrc_add_frame(buffersrc_ctx, pFrame) < 0) {
//                    LOGE("Could not av_buffersrc_add_frame");
//                    break;
//                }
//
//                ret = av_buffersink_get_frame(buffersink_ctx, pFrame);
//                if (ret < 0) {
//                    LOGE("Could not av_buffersink_get_frame");
//                    break;
//                }
//                //added by ws for AVfilter end
//
//                // lock native window buffer
//                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
//
//                // 格式转换
//                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
//                          pFrame->linesize, 0, pCodecCtx->height,
//                          pFrameRGBA->data, pFrameRGBA->linesize);
//
//                // 获取stride
//                uint8_t *dst = (uint8_t *) windowBuffer.bits;
//                int dstStride = windowBuffer.stride * 4;
//                uint8_t *src = (pFrameRGBA->data[0]);
//                int srcStride = pFrameRGBA->linesize[0];
//
//                // 由于window的stride和帧的stride不同,因此需要逐行复制
//                int h;
//                for (h = 0; h < videoHeight; h++) {
//                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
//                }
//
//                ANativeWindow_unlockAndPost(nativeWindow);
//            }
//
//        }
//        av_packet_unref(&packet);
//    }
//
//    av_free(buffer);
//    av_free(pFrameRGBA);
//
//    // Free the YUV frame
//    av_free(pFrame);
//
//    avfilter_graph_free(&filter_graph); //added by ws for avfilter
//    // Close the codecs
//    avcodec_close(pCodecCtx);
//
//    // Close the video file
//    avformat_close_input(&pFormatCtx);
//    return 0;
//}


}
