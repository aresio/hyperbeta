//#include "stdafx.h"
#ifndef __AUDIO__
#define __AUDIO__
#include <Windows.h>
#include <mmsystem.h>
#include <string>

bool use_audio = true;

void play_sound(std::string path) {

	if (!use_audio)  return;
		
#ifdef _WIN32
	PlaySoundA((LPCSTR)(path.c_str()), NULL, SND_FILENAME | SND_ASYNC);
#endif

}

void switch_audio() {
	use_audio = !use_audio;
}

void play_midi(std::string file) {

/*	HMIDIOUT out;
 
   unsigned int err = midiOutOpen(&out, MIDI_MAPPER, 0, 0, CALLBACK_NULL);
   if (err != MMSYSERR_NOERROR)
      printf("error opening MIDI Mapper: %d\n", err);
   else
      printf("successfully opened MIDI Mapper\n");


	std::string fullstring("play "+file);
	mciSendString((LPCWSTR)(fullstring.c_str()),NULL,0,NULL); */
}


#endif