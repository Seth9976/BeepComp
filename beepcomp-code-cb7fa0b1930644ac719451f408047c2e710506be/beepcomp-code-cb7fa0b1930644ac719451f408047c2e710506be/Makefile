all: normal

normal:
	g++ -o beepcomp_dev Config.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp Speech.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib synthModule.a -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -lUniversalSpeech
	
pipe:
	g++ -o beepcomp_dev Config.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp Speech.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib synthModule.a -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -lUniversalSpeech &> err.txt

pipewarning:
	g++ -o beepcomp_dev Config.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp Speech.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib synthModule.a -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -lUniversalSpeech -Wall &> err.txt

release:
	g++ -o beepcomp Config.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp Speech.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib synthModule.a -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -lUniversalSpeech -mwindows	
	
portable2:
	g++ -o beepcomp_portable -I./include Config.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp Speech.cpp GUI.cpp main.cpp beepcomp.res -L./lib synthModule.a -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -mwindows -D__PORTABLE__
	
debug:
	g++ -o beepcomp_d -g Config.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp Speech.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib synthModule.a -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -lUniversalSpeech
	
synthModule:
	g++ -c MData.cpp -I./include -L./lib;
	g++ -c DData.cpp -I./include -L./lib;
	g++ -c Astro.cpp -I./include -L./lib;
	g++ -c LFO.cpp -I./include -L./lib;
	g++ -c Fall.cpp -I./include -L./lib;
	g++ -c Biquad.cpp -I./include -L./lib;
	g++ -c OSC.cpp -I./include -L./lib;
	g++ -c NOSC.cpp -I./include -L./lib;
	g++ -c DelayLine.cpp -I./include -L./lib;
	ar cr synthModule.a MData.o DData.o Astro.o LFO.o Fall.o Biquad.o OSC.o NOSC.o DelayLine.o
	rm MData.o DData.o Astro.o LFO.o Fall.o Biquad.o OSC.o NOSC.o DelayLine.o
	
clean:
	rm -f synthModule.a
	rm -f beepcomp_dev
	rm -f MData.o DData.o Astro.o LFO.o Fall.o Biquad.o OSC.o NOSC.o DelayLine.o
