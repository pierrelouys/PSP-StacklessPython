#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <psputility_avmodules.h>
#include <pspaudiocodec.h>
#include <pspaudio.h>
#include <pspsdk.h>
#include <unistd.h>

#include "MusicEngine.h"

#define THREAD_PRIORITY 0x12
#define OUTPUT_BUFFER_SIZE	(AUDIO_SAMPLES*4)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Globals:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static char currentDir[264];
static int MP3ME_threadActive = 0;
static char MP3ME_fileName[264];
static int MP3ME_isPlaying = 0;
static int MP3ME_thid = -1;
static int MP3ME_audio_channel = 0;
static int MP3ME_eof = 0;
static float MP3ME_playingTime = 0;
static int MP3ME_volume = 0;
static double MP3ME_filePos = 0;
static double MP3ME_musicStart = 0;

//shared global vars for ME
static int HW_ModulesInit = 0;
u16 data_align;
u32 sample_per_frame;
u16 channel_mode;
u32 samplerate;
long data_start;
long data_size;
u8 getEDRAM;
u32 channels;
SceUID data_memid;
volatile int OutputBuffer_flip;
static int MP3ME_Loop = 0;

//Globals for decoding:
static SceUID MP3ME_handle = -1;

static int samplerates[4][3] =
{
    {11025, 12000, 8000,},//mpeg 2.5
    {0, 0, 0,}, //reserved
    {22050, 24000, 16000,},//mpeg 2
    {44100, 48000, 32000}//mpeg 1
};
static int bitrates[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 };
static int bitrates_v2[] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 };

static unsigned char MP3ME_input_buffer[2889]__attribute__((aligned(64)));//mp3 has the largest max frame, at3+ 352 is 2176
static unsigned long MP3ME_codec_buffer[65]__attribute__((aligned(64)));
static unsigned char MP3ME_output_buffer[2048*4]__attribute__((aligned(64)));//at3+ sample_per_frame*4
static short OutputBuffer[2][OUTPUT_BUFFER_SIZE];
static short *OutputPtrME = OutputBuffer[0];


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Private functions:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//reserve channel
int openAudio(int channel, int samplecount){
	int audio_channel = sceAudioChReserve(channel, samplecount, PSP_AUDIO_FORMAT_STEREO );
    if(audio_channel < 0)
        audio_channel = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, samplecount, PSP_AUDIO_FORMAT_STEREO );
	return audio_channel;
}

//release channel
int releaseAudio(void){
	while(sceAudioOutput2GetRestSample() > 0)
        sceKernelDelayThread(10);
    return sceAudioChRelease(MP3ME_audio_channel);
}

//audio output
int audioOutput(int volume, void *buffer){
    return sceAudioOutputBlocking(MP3ME_audio_channel, volume, buffer);
}

//Load a module:
SceUID LoadStartAudioModule(char *modname, int partition){
    SceKernelLMOption option;
    SceUID modid;

    memset(&option, 0, sizeof(option));
    option.size = sizeof(option);
    option.mpidtext = partition;
    option.mpiddata = partition;
    option.position = 0;
    option.access = 1;

    modid = sceKernelLoadModule(modname, 0, &option);
    if (modid < 0)
        return modid;

    return sceKernelStartModule(modid, 0, NULL, NULL, NULL);
}

//Load and start needed modules:
int initMEAudioModules(){
   if (!HW_ModulesInit){
       if (sceKernelDevkitVersion() == 0x01050001){
           LoadStartAudioModule("flash0:/kd/me_for_vsh.prx", PSP_MEMORY_PARTITION_KERNEL);
           LoadStartAudioModule("flash0:/kd/audiocodec.prx", PSP_MEMORY_PARTITION_KERNEL);
       }else{
           sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
		   sceUtilityLoadAvModule(PSP_AV_MODULE_ATRAC3PLUS);
	   }

       HW_ModulesInit = 1;
   }
   return 0;
}

//Seek next valid frame
//NOTE: this function comes from Music prx 0.55 source
//      all credits goes to joek2100.
int SeekNextFrameMP3(SceUID fd)
{
    int offset = 0;
    unsigned char buf[1024];
    unsigned char *pBuffer;
    int i;
    int size = 0;

    offset = sceIoLseek32(fd, 0, PSP_SEEK_CUR);
    sceIoRead(fd, buf, sizeof(buf));
    if (!strncmp((char*)buf, "ID3", 3) || !strncmp((char*)buf, "ea3", 3)) //skip past id3v2 header, which can cause a false sync to be found
    {
        //get the real size from the syncsafe int
        size = buf[6];
        size = (size<<7) | buf[7];
        size = (size<<7) | buf[8];
        size = (size<<7) | buf[9];

        size += 10;

        if (buf[5] & 0x10) //has footer
            size += 10;
    }

    sceIoLseek32(fd, offset, PSP_SEEK_SET); //now seek for a sync
    while(1)
    {
        offset = sceIoLseek32(fd, 0, PSP_SEEK_CUR);
        size = sceIoRead(fd, buf, sizeof(buf));

        if (size <= 2)//at end of file
            return -1;

        if (!strncmp((char*)buf, "EA3", 3))//oma mp3 files have non-safe ints in the EA3 header
        {
            sceIoLseek32(fd, (buf[4]<<8)+buf[5], PSP_SEEK_CUR);
            continue;
        }

        pBuffer = buf;
        for( i = 0; i < size; i++)
        {
            //if this is a valid frame sync (0xe0 is for mpeg version 2.5,2+1)
            if ( (pBuffer[i] == 0xff) && ((pBuffer[i+1] & 0xE0) == 0xE0))
            {
                offset += i;
                sceIoLseek32(fd, offset, PSP_SEEK_SET);
                return offset;
            }
        }
       //go back two bytes to catch any syncs that on the boundary
        sceIoLseek32(fd, -2, PSP_SEEK_CUR);
    }
}

int swapInt32BigToHost(int arg)
{
   int i=0;
   int checkEndian = 1;
   if( 1 == *(char *)&checkEndian )
   {
      // Intel (little endian)
      i=arg;
      i=((i&0xFF000000)>>24)|((i&0x00FF0000)>>8)|((i&0x0000FF00)<<8)|((i&0x000000FF)<<24);
   }
   else
   {
      // PPC (big endian)
      i=arg;
   }
   return i;
}

//Get the id3v2 size
int ID3v2TagSize(const char *mp3path)
{
   int fp = 0;
   int size;
   char sig[3];

   fp = sceIoOpen(mp3path, PSP_O_RDONLY, 0777);
   if (fp < 0) return 0;

   sceIoRead(fp, sig, sizeof(sig));
   if (strncmp("ID3",sig,3) != 0) {
      sceIoClose(fp);
      return 0;
   }

   sceIoLseek(fp, 6, PSP_SEEK_SET);
   sceIoRead(fp, &size, sizeof(unsigned int));
   /*
    *  The ID3 tag size is encoded with four bytes where the first bit
    *  (bit 7) is set to zero in every byte, making a total of 28 bits. The zeroed
    *  bits are ignored, so a 257 bytes long tag is represented as $00 00 02 01.
    */

   size = (unsigned int) swapInt32BigToHost((int)size);
   size = ( ( (size & 0x7f000000) >> 3 ) | ( (size & 0x7f0000) >> 2 ) | ( (size & 0x7f00) >> 1 ) | (size & 0x7f) );
   sceIoClose(fp);
   return size;
}

//Decode thread:
int decodeThread(SceSize args, void *argp){
    int res;
    unsigned char MP3ME_header_buf[4];
    int MP3ME_header;
    int version;
    int bitrate;
    int padding;
    int frame_size = 0;
    int size;
    int total_size;
	int offset = 0;

    //printf("pspmp3: Thread started\n");
    sceIoChdir(currentDir);
    //printf("pspmp3: chdir to %s\n", currentDir);

	sceAudiocodecReleaseEDRAM(MP3ME_codec_buffer); //Fix: ReleaseEDRAM at the end is not enough to play another mp3.
	MP3ME_threadActive = 1;
    OutputBuffer_flip = 0;
    OutputPtrME = OutputBuffer[0];

    MP3ME_handle = sceIoOpen(MP3ME_fileName, PSP_O_RDONLY, 0777);
    if (MP3ME_handle < 0){
        //printf("pspmp3: Error opening file %s\n", MP3ME_fileName);
        MP3ME_threadActive = 0;
    }

	//now search for the first sync byte, tells us where the mp3 stream starts
	total_size = sceIoLseek32(MP3ME_handle, 0, PSP_SEEK_END);
	size = total_size;
	sceIoLseek32(MP3ME_handle, 0, PSP_SEEK_SET);
	data_start = ID3v2TagSize(MP3ME_fileName);
	sceIoLseek32(MP3ME_handle, data_start, PSP_SEEK_SET);
    data_start = SeekNextFrameMP3(MP3ME_handle);
	if (data_start < 0){
        //printf("pspmp3: Error data_start\n");
		MP3ME_threadActive = 0;
    }

    MP3ME_musicStart = data_start;
    size -= data_start;

    memset(MP3ME_codec_buffer, 0, sizeof(MP3ME_codec_buffer));
    memset(MP3ME_input_buffer, 0, sizeof(MP3ME_input_buffer));
    memset(MP3ME_output_buffer, 0, sizeof(MP3ME_output_buffer));

    if ( sceAudiocodecCheckNeedMem(MP3ME_codec_buffer, 0x1002) < 0 )
    {
        //printf("pspmp3: Error sceAudiocodecCheckNeedMem\b");
        MP3ME_threadActive = 0;
    }

    if ( sceAudiocodecGetEDRAM(MP3ME_codec_buffer, 0x1002) < 0 )
    {
        //printf("pspmp3: Error sceAudiocodecGetEDRAM\n");
        MP3ME_threadActive = 0;
    }else{
        getEDRAM = 1;
    }

    if ( sceAudiocodecInit(MP3ME_codec_buffer, 0x1002) < 0 )
    {
        //printf("pspmp3: Error sceAudiocodecInit\n");
        MP3ME_threadActive = 0;
    }

    MP3ME_eof = 0;
    //printf("pspmp3: MP3ME_threadActive %i\n", MP3ME_threadActive);

	while (MP3ME_threadActive){
		while( !MP3ME_eof && MP3ME_isPlaying )
		{
            MP3ME_filePos = sceIoLseek32(MP3ME_handle, 0, PSP_SEEK_CUR);
			if ( sceIoRead( MP3ME_handle, MP3ME_header_buf, 4 ) != 4 ){
				MP3ME_isPlaying = 0;
				MP3ME_threadActive = 0;
				continue;
			}

			MP3ME_header = MP3ME_header_buf[0];
			MP3ME_header = (MP3ME_header<<8) | MP3ME_header_buf[1];
			MP3ME_header = (MP3ME_header<<8) | MP3ME_header_buf[2];
			MP3ME_header = (MP3ME_header<<8) | MP3ME_header_buf[3];

			bitrate = (MP3ME_header & 0xf000) >> 12;
			padding = (MP3ME_header & 0x200) >> 9;
			version = (MP3ME_header & 0x180000) >> 19;
			samplerate = samplerates[version][ (MP3ME_header & 0xC00) >> 10 ];

			if ((bitrate > 14) || (version == 1) || (samplerate == 0) || (bitrate == 0))//invalid frame, look for the next one
			{
				data_start = SeekNextFrameMP3(MP3ME_handle);
				if(data_start < 0)
				{
					MP3ME_eof = 1;
					continue;
				}
				size -= (data_start - offset);
				offset = data_start;
				continue;
			}

			if (version == 3) //mpeg-1
			{
				sample_per_frame = 1152;
				frame_size = 144000*bitrates[bitrate]/samplerate + padding;
			}else{
				sample_per_frame = 576;
				frame_size = 72000*bitrates_v2[bitrate]/samplerate + padding;
			}

			sceIoLseek32(MP3ME_handle, data_start, PSP_SEEK_SET); //seek back


			size -= frame_size;
			if ( size <= 0)
			{
			   MP3ME_eof = 1;
			   continue;
			}

			//since we check for eof above, this can only happen when the file
			// handle has been invalidated by syspend/resume/usb
			if ( sceIoRead( MP3ME_handle, MP3ME_input_buffer, frame_size ) != frame_size ){
                //Resume from suspend:
                if ( MP3ME_handle >= 0 ){
                   sceIoClose(MP3ME_handle);
                   MP3ME_handle = -1;
                }
                MP3ME_handle = sceIoOpen(MP3ME_fileName, PSP_O_RDONLY, 0777);
                if (MP3ME_handle < 0){
                    MP3ME_isPlaying = 0;
                    MP3ME_threadActive = 0;
                    continue;
                }
                size = sceIoLseek32(MP3ME_handle, 0, PSP_SEEK_END);
                sceIoLseek32(MP3ME_handle, offset, PSP_SEEK_SET);
                data_start = offset;
				continue;
			}
			data_start += frame_size;
			offset = data_start;

			MP3ME_codec_buffer[6] = (unsigned long)MP3ME_input_buffer;
			MP3ME_codec_buffer[8] = (unsigned long)MP3ME_output_buffer;

			MP3ME_codec_buffer[7] = MP3ME_codec_buffer[10] = frame_size;
			MP3ME_codec_buffer[9] = sample_per_frame * 4;

			res = sceAudiocodecDecode(MP3ME_codec_buffer, 0x1002);

			if ( res < 0 )
			{
				//instead of quitting see if the next frame can be decoded
				//helps play files with an invalid frame
				//we must look for a valid frame, the offset above may be wrong
				data_start = SeekNextFrameMP3(MP3ME_handle);
				if(data_start < 0)
				{
					MP3ME_eof = 1;
					continue;
				}
				size -= (data_start - offset);
				offset = data_start;
				continue;
			}
            MP3ME_playingTime += (float)sample_per_frame/(float)samplerate;

            //Output:
			memcpy( OutputPtrME, MP3ME_output_buffer, sample_per_frame*4);
			OutputPtrME += (sample_per_frame * 4);
			if( OutputPtrME + (sample_per_frame * 4) > &OutputBuffer[OutputBuffer_flip][OUTPUT_BUFFER_SIZE])
			{
                audioOutput(MP3ME_volume, OutputBuffer[OutputBuffer_flip]);

				OutputBuffer_flip ^= 1;
				OutputPtrME = OutputBuffer[OutputBuffer_flip];
			}
		}
        if (MP3ME_eof && MP3ME_Loop)
        {
            sceIoLseek32(MP3ME_handle, MP3ME_musicStart, PSP_SEEK_SET);
            data_start = MP3ME_musicStart;
            MP3ME_playingTime = 0;

            offset = data_start;
            size = total_size - data_start;
            MP3ME_eof = 0;
        }
        else
    		sceKernelDelayThread(10000);
	}
    if (getEDRAM)
        sceAudiocodecReleaseEDRAM(MP3ME_codec_buffer);

    if ( MP3ME_handle >= 0){
      sceIoClose(MP3ME_handle);
      MP3ME_handle = -1;
    }
    //printf("pspmp3: Thread ended\n");
	sceKernelExitThread(0);
    return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Public API
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Init(int channel)
{
    MP3ME_playingTime = 0;
	MP3ME_volume = PSP_AUDIO_VOLUME_MAX;
	initMEAudioModules();
    MP3ME_audio_channel = openAudio(channel, AUDIO_SAMPLES);
    if (MP3ME_audio_channel < 0){
        //printf("pspmp3: failed to reserve channel\n");
        return 0;
    }
    //printf("pspmp3: reserved channel %i\n", MP3ME_audio_channel);
    return 1;
}


int Load(const char* musicfile)
{
    getcwd(currentDir, 256);
    //printf("pspmp3: Loading %s\n", musicfile);
    MP3ME_filePos = 0;
    MP3ME_isPlaying = 0;
    strcpy(MP3ME_fileName, musicfile);

    MP3ME_thid = -1;
    MP3ME_eof = 0;
    MP3ME_thid = sceKernelCreateThread("decodeThread", decodeThread, THREAD_PRIORITY, DEFAULT_THREAD_STACK_SIZE, PSP_THREAD_ATTR_USER, NULL);
    if(MP3ME_thid < 0)
        return 0;

    sceKernelStartThread(MP3ME_thid, 0, NULL);
    return 1;
}

int Play(int loop)
{
    if(MP3ME_thid < 0)
        return 0;
    MP3ME_Loop = loop;
    MP3ME_isPlaying = 1;
    return 1;
}

int Stop()
{
    MP3ME_isPlaying = 0;
    MP3ME_threadActive = 0;
	sceKernelWaitThreadEnd(MP3ME_thid, NULL);
    sceKernelDeleteThread(MP3ME_thid);
    strcpy(MP3ME_fileName, "");
    return 1;
}

int Pause()
{
    MP3ME_isPlaying = !MP3ME_isPlaying;
    return 1;
}

int EndOfStream()
{
    return MP3ME_eof;
}

void GetTimeString(char *dest)
{
    char timeString[9];
    int secs = (int)MP3ME_playingTime;
    int hh = secs / 3600;
    int mm = (secs - hh * 3600) / 60;
    int ss = secs - hh * 3600 - mm * 60;
    snprintf(timeString, sizeof(timeString), "%2.2i:%2.2i:%2.2i", hh, mm, ss);
    strcpy(dest, timeString);
}

void FreeTune()
{

}

void End()
{
    Stop();
    releaseAudio();
    MP3ME_audio_channel = 0;
}
