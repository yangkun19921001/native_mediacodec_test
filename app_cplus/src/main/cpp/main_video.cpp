
#include "codec_app_def.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include <jni.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "GPU_codec_api.h"

// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
#include <android/log.h>
#define TAG "NativeCodec"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

uint8_t g_yuv_buffer[1024*4096];
uint8_t g_dec_buffer[1024*4096];
uint8_t g_stream_buffer[2048*1024];
uint8_t g_encoder_buffer[2048*1024];

volatile int   g_capture_stop = 0;
SSourcePicture _sSrcPic;
SLayerBSInfo   _sLayerData;
volatile int g_total_decode_frames = 0;
volatile int g_total_decode_frames_ok = 0;

/////////////////////////////////////////////////////////////////////////////////////////
void *thread_capture(void *argu)
{
    FILE * inFile = fopen("/storage/emulated/0/test.yuv", "rb");
    FILE * outFile = fopen("/storage/emulated/0/test.264", "wb");

    if (!inFile)
    {
        LOGV("open YUV  file error!");
        return NULL;
    }
    if (!outFile)
    {
        LOGV("open bitstream  file error!");
        return NULL;
    }

    LOGV("open YUV or bitstream file OK!\n");

    int total_frame_count = 0;
    int width = 176;
    int height = 144;
    int framerate = 25;
    int frame_size = width * height * 3 / 2;
    int file_length = 0;

    MSdkInputParam InputParam;
	InputParam.nFrameRate = framerate;
	InputParam.nWidth = width;
	InputParam.nHeight = height;
	InputParam.nTargetKbps = 2000;
	InputParam.nTemporalLayers = 0;
	InputParam.nSpatialId = 0;
	InputParam.nMemType = SYSTEM_MEMORY;
	
	InputParam.InStreamType = videoFormatI420;
	InputParam.InFrameRate = 25;
	InputParam.InWidth = width;
	InputParam.InHeight = height;
	
	VM_MSDKEncoder *gpuEncoder = VM_MSDKEncoder::CreateEncoder(&InputParam);
	if (gpuEncoder == NULL)
	{
		LOGV("create encoder failed ************************");
		return NULL;
	}

    LOGV("start encoder successful ************************");

    while (total_frame_count < 50)
    {
        usleep(1000*50);

        int readBytes = fread(g_yuv_buffer, 1, frame_size, inFile);
        if (readBytes != frame_size) break;
        
        ////////////////////////////////////////////////////////////////////
        memset(&_sSrcPic, 0, sizeof(SSourcePicture));

        _sSrcPic.uiTimeStamp = total_frame_count * 40;
        _sSrcPic.iColorFormat = videoFormatI420;
        _sSrcPic.iPicWidth = width;
        _sSrcPic.iPicHeight = height;

        _sSrcPic.iStride[0] = width;
        _sSrcPic.iStride[1] = (width + 1) / 2;
        _sSrcPic.iStride[2] = (width + 1) / 2;

        _sSrcPic.pData[0] = g_yuv_buffer;
        _sSrcPic.pData[1] = _sSrcPic.pData[0] + width * height;
        _sSrcPic.pData[2] = _sSrcPic.pData[1] + width * height / 4;

		int nal_count_buffer[8];
		_sLayerData.pBsBuf = g_encoder_buffer;
		_sLayerData.pNalLengthInByte = &nal_count_buffer[0];
		
		int status = gpuEncoder->EncodeFrame(&_sSrcPic, &_sLayerData);
		if (status == MCODEC_SUCCEED)
		{
			status = gpuEncoder->GetBitstream(&_sLayerData);
			if (status == MCODEC_SUCCEED)
			{
				SLayerBSInfo* pLayerBsInfo = &_sLayerData;
				if (pLayerBsInfo->iNalCount > 0)
				{
					int total_length = 0;
					for (int k = 0; k < _sLayerData.iNalCount; k++)
					{
						total_length += _sLayerData.pNalLengthInByte[k];
					}

                    LOGV("encoder: %d frame, bitstream length = %d", total_frame_count, total_length+4);

                    file_length += total_length+4;
					g_total_decode_frames_ok++;
                    uint8_t *data_buf = pLayerBsInfo->pBsBuf;

                    if(outFile != NULL)
                    {
                        uint8_t start_code[4] = {0x00, 0x00, 0x00, 0x01};
                        fwrite(start_code, 1, 4, outFile);
                        fwrite(pLayerBsInfo->pBsBuf, 1, total_length, outFile);
                    }
				}
			} else{
                LOGV("encoder: %d frame, failed", total_frame_count);
            }
		} else{
            LOGV("encoder: %d frame, failed", total_frame_count);
        }
        
        ////////////////////////////////////////////////////////////////////
        total_frame_count++;
		g_total_decode_frames++;

        if ((total_frame_count % 25) == 0)
        {
            LOGV("capture frames, length = %d, total %d frames", file_length, total_frame_count);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    VM_MSDKEncoder::DeleteEncoder(gpuEncoder);

    fclose(inFile);
    fclose(outFile);
    
    LOGV("capture exited ************************");
    
    g_capture_stop = 1;
    return argu;
}

//////////////////////////////////////////////////////////////////////////////////////////
int main_video(void)
{
    pthread_t tid_cap;
    int err = pthread_create(&tid_cap, NULL, thread_capture, NULL);
    if (err != 0)
    {
        LOGV("pthread_create failed************************");
        return -1;
    }

    g_capture_stop = 0;
    while (g_capture_stop == 0)
    {
        usleep(1000*100);
    }

    return 0;
}
