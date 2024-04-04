#include "MusicFile.h"
#include <stdlib.h>
#include <string.h>

MusicFile* createMusicFile(const char *songName, const char *artist, const char *album, int duration)
	{
		MusicFile *newFile = (MusicFile *)malloc(sizeof(MusicFile));
		if (newFile)
			{
				newFile->songName = strdup(songName);
				newFile->artist = strdup(artist);
				newFile->album = strdup(album);
				newFile->duration = duration;
				newFile->next = NULL;
				newFile->prev = NULL;
			}
		return newFile;
	}



void freeMusicFile(MusicFile *file)
	{
		if (file)
			{
				free(file->songName);
				free(file->artist);
				free(file->album);
				free(file);
			}
	}
