#include <cadence/agent.h>
#include <cadence/doste/doste.h>
#include <alsa/asoundlib.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>

#define  MIDI_DEVICE  "/dev/snd/controlC1"


using namespace cadence;
using namespace cadence::doste;

int seqfd;
snd_rawmidi_t* midiin = NULL;
OID mykeys;



extern "C" void initialise(const cadence::doste::OID &base) {    

	mykeys = base.get("keys");
	

   // first open the sequencer device for reading.
   //int seqfd = open(MIDI_DEVICE, O_RDONLY);
   //if (seqfd == -1) {
    //  printf("Error: cannot open %s\n", MIDI_DEVICE);
    //  exit(1);
   //}

	int status;
	if ((status = snd_rawmidi_open(&midiin, NULL, "hw:1,0,0", SND_RAWMIDI_NONBLOCK)) < 0)
	if ((status = snd_rawmidi_open(&midiin, NULL, "hw:2,0,0", SND_RAWMIDI_NONBLOCK)) < 0) {
	  printf("Problem opening MIDI input: %s", snd_strerror(status));
	  return;
	}

}

extern "C" void cadence_update() {

	//unsigned char inpacket[4];
	
	//if (read(seqfd, &inpacket, sizeof(inpacket)) <= 0) {
	//	std::cout << "NO DATA\n";
	//}
 
      // print the MIDI byte if this input packet contains one
      //if (inpacket[0] == SEQ_MIDIPUTC) {
       //  printf("received MIDI byte: %d\n", inpacket[1]);
	//}

char buffer[1];
int	status = 0;
int key;
int strength;

      while (status != -EAGAIN) {
         status = snd_rawmidi_read(midiin, buffer, 1);
         if ((status < 0) && (status != -EBUSY) && (status != -EAGAIN)) {
            printf("Problem reading MIDI input: %s",snd_strerror(status));
         } else if (status >= 0) {

		if ((unsigned char)buffer[0] == 0x90) {
			status = snd_rawmidi_read(midiin, buffer, 1);
			key = (unsigned char)buffer[0];
			status = snd_rawmidi_read(midiin, buffer, 1);
			strength = (unsigned char)buffer[0];

			if (strength > 0)
				mykeys.set(key, true);
			else
				mykeys.set(key, false);
		}

            //count++;
            /*if ((unsigned char)buffer[0] >= 0x80) {  // print command in hex
               printf("0x%x ", (unsigned char)buffer[0]);
            } else {
               printf("%d ", (unsigned char)buffer[0]);
            }
            fflush(stdout);*/
            //if (count % 20 == 0) {
               //printf("\n");
            //}
         }
      }


}

extern "C" void finalise() {
	/* Stop PCM device and drop pending frames */
    //snd_pcm_drop(pcm_handle);

    /* Stop PCM device after pending frames have been played */ 
   // snd_pcm_drain(pcm_handle);
}
