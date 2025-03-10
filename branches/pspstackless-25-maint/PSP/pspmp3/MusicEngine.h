/*!
*
*		This file is created by R.N. den Hollander
*		a.k.a Ghoti.
*		This file is intended for use for the stackless
*		python project. If you want to use this for something
*		else that is ok but please let me know.
*
*		url: www.ghoti.nl
*
*
*/


#ifndef MUSICENGINE_H
#define MUSICENGINE_H


#include <pspkernel.h>


#define AUDIO_SAMPLES 1152
#define DEFAULT_THREAD_STACK_SIZE 32*1024

/*!
*
*	\fn	int Init()
*	\brief	This function initializes the PSP audio,
*			the MAD libraries and other info to enable
*			mp3 buffering.
*/
int Init(int channel);



/*!
*
*	\fn	int Load(const char* musicfile)
*	\brief	This function sets the path of the current
*			mp3. This path will be used in the callback
*			function to retrieve parts of the mp3.
*
*	@param	musicfile is the path to the song.
*/
int Load(const char* musicfile);



/*!
*
*	\fn	int Play()
*	\brief	This functions turns on the flag that the
*			current mp3 file can be played.
*/
int Play();



/*!
*
*	\fn int Stop()
*	\brief	This function does not stop the thread. This
*			function sets the pause variable to true so
*			that the callbackfunction passes zero's
*			instead of musicparts. So the callbackfunction
*			keeps on playing but plays zero's which means
*			no sound. Also the file indicator is set to
*			zero.
*/
int Stop();




/*!
*
*	\fn int Pause()
*	\brief	This function pauses the music by turning the
*			variable pausesong to 1. This means that zero's
*			are passed to the callbackfunction so that no
*			sound is played.
*			This function does not reset the file indicator.
*/
int Pause();



/*!
*
*	\fn int EndOfStream()
*	\brief	This function reads the flag that indicates
*			if a strem has reached its end.
*/
int EndOfStream();


/*!
*
*	\fn	void GetTimeString(char *dest)
*	\brief	This function returns a string into a reference containing the stream time.
*
*	@param	string reference
*
*/
void GetTimeString(char *dest);

void FreeTune();

void End();

#endif
