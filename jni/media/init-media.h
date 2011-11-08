/**
 * 
 * @author Miguel París Díaz
 * 
 */

#include "libavformat/avformat.h"

int init_media();

URLContext* get_audio_connection(int audioPort);
URLContext* get_video_connection(int videoPort);
