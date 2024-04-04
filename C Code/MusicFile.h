#ifndef MUSICFILE_H
#define MUSICFILE_H

// Structure for a music file
typedef struct MusicFile
	{
		char *songName;
		char *artist;
		char *album;
		int duration; // Duration in seconds
		struct MusicFile *next;
		struct MusicFile *prev;
	} MusicFile;



// Function prototypes
MusicFile* createMusicFile(const char *songName, const char *artist, const char *album, int duration);
void freeMusicFile(MusicFile *file);

#endif // MUSICFILE_H
