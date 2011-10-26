#include <cadence/agent.h>
#include <cadence/doste/doste.h>
#include <alsa/asoundlib.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>

#define  MIDI_DEVICE  "/dev/snd/controlC1"


using namespace cadence;
using namespace cadence::doste;

int rate = 44100; /* Sample rate */
unsigned int exact_rate;   /* Sample rate returned by */
int lastindex = 0;
int curindex;
unsigned char *data_live;
unsigned char *data_next;
int frames;
int flushcount;
extern double ttime;
double first_flush = 0.0;
double last_flush = 0.0;
double next_flush = 1.0;

int seqfd;
snd_rawmidi_t* midiin = NULL;
OID mykeys;


/* Handle for the PCM device */ 
	snd_pcm_t *pcm_handle; 

class Sound : public Agent {
	public:
	Sound(const OID &o): Agent(o) { registerEvents(); }
	~Sound() {};

	OBJECT(Agent, Sound);

	BEGIN_EVENTS(Agent);
	EVENT(evt_amp, (*this)("amp"));
	END_EVENTS;
};

OnEvent(Sound, evt_amp) {
    int pcmreturn;
    short s1,s2,s3;

	

	curindex = (int)(((ttime - last_flush) / (next_flush - last_flush)) * 2048);
	s1 = (short)(((float)get("amp") * 32768.0));
	
	//std::cout << s1 << "\n";
	
	if (lastindex < 2048)
		s3 = data_live[4*lastindex] + (data_live[4*lastindex+1] << 8);
	else if (lastindex < 4096)
		s3 = data_next[4*(lastindex-2048)] + (data_live[4*(lastindex-2048)+1] << 8);
	else
		s3 = s1;

	short d1 = s1 - s3;
	int d2 = curindex - lastindex;
	
	//if ((curindex - lastindex) > 1) std::cout << "LAG: " << d2 << " s1=" << s1 << "s3=" << s3 << "\n";

	for (int i = lastindex+1; i < curindex; i++) {
		s2 = s3 + (short)(((float)(i - lastindex) / (float)d2) * (float)d1);
		//std::cout << s2 << "\n";
		if (i >= 4096) {
			//std::cout << "OVERRUN1: " << i << "\n";
		} else if (i >= 2048) {
			data_next[4*(i-2048)] = (unsigned char)s2;
			data_next[4*(i-2048)+1] = s2 >> 8;
			data_next[4*(i-2048)+2] = (unsigned char)s2;
			data_next[4*(i-2048)+3] = s2 >> 8;
		} else {
			data_live[4*i] = (unsigned char)s2;
			data_live[4*i+1] = s2 >> 8;
			data_live[4*i+2] = (unsigned char)s2;
			data_live[4*i+3] = s2 >> 8;
		}
	}
	lastindex = curindex;

	if (curindex >= 4096) {
		//std::cout << "OVERRUN2: " << curindex << "\n";
	} else if (curindex >= 2048) {
		data_next[4*(curindex-2048)] = (unsigned char)s1;
		data_next[4*(curindex-2048)+1] = s1 >> 8;
		data_next[4*(curindex-2048)+2] = (unsigned char)s1;
		data_next[4*(curindex-2048)+3] = s1 >> 8;
	} else {
		data_live[4*curindex] = (unsigned char)s1;
		data_live[4*curindex+1] = s1 >> 8;
		data_live[4*curindex+2] = (unsigned char)s1;
		data_live[4*curindex+3] = s1 >> 8;
	}


}

IMPLEMENT_EVENTS(Sound, Agent);



extern "C" void initialise(const cadence::doste::OID &base) {    

	mykeys = base.get("keys");
	

   // first open the sequencer device for reading.
   //int seqfd = open(MIDI_DEVICE, O_RDONLY);
   //if (seqfd == -1) {
    //  printf("Error: cannot open %s\n", MIDI_DEVICE);
    //  exit(1);
   //}

	int status;
	if ((status = snd_rawmidi_open(&midiin, NULL, "hw:1,0,0", SND_RAWMIDI_NONBLOCK)) < 0) {
      printf("Problem opening MIDI input: %s", snd_strerror(status));
      exit(1);
   }
 

     

    /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

    /* This structure contains information about    */
    /* the hardware and can be used to specify the  */      
    /* configuration to be used for the PCM stream. */ 
    snd_pcm_hw_params_t *hwparams;

	/* Name of the PCM device, like plughw:0,0          */
    /* The first number is the number of the soundcard, */
    /* the second number is the number of the device.   */
    char *pcm_name;

	    /* Init pcm_name. Of course, later you */
    /* will make this configurable ;-)     */
    pcm_name = strdup("default");
  

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);
  
	/* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */ 
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */
    if (snd_pcm_open(&pcm_handle, pcm_name, stream, SND_PCM_NONBLOCK) < 0) {
      fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
      return;
    }

	/* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Can not configure this PCM device.\n");
      return;
    }

                      /* snd_pcm_hw_params_set_rate_near */ 
    int dir;          /* exact_rate == rate --> dir = 0 */
                      /* exact_rate < rate  --> dir = -1 */
                      /* exact_rate > rate  --> dir = 1 */
    int periods = 2;       /* Number of periods */
    snd_pcm_uframes_t periodsize = 8192; /* Periodsize (bytes) */

	/* Set access type. This can be either    */
    /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
    /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
    /* There are also access types for MMAPed */
    /* access, but this is beyond the scope   */
    /* of this introduction.                  */
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
      fprintf(stderr, "Error setting access.\n");
      return;
    }
  
    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
      fprintf(stderr, "Error setting format.\n");
      return;
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */ 
    exact_rate = rate;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
      fprintf(stderr, "Error setting rate.\n");
      return;
    }
    if (rate != exact_rate) {
      fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", rate, exact_rate);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
      fprintf(stderr, "Error setting channels.\n");
      return;
    }

    /* Set number of periods. Periods used to be called fragments. */ 
    if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
      fprintf(stderr, "Error setting periods.\n");
      //return;
    }

	/* Set buffer size (in frames). The resulting latency is given by */
    /* latency = periodsize * periods / (rate * bytes_per_frame)     */
    if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (periodsize * periods)>>2) < 0) {
      fprintf(stderr, "Error setting buffersize.\n");
      return;
    }

	/* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Error setting HW params.\n");
      return;
    }

	

	data_live = (unsigned char *)malloc(periodsize);
	data_next = (unsigned char *)malloc(periodsize);
    frames = periodsize >> 2;

	flushcount = 0;
	first_flush = ttime;
	last_flush = ttime;
	next_flush = first_flush + ((flushcount+1) * 0.04643990);

	Object::registerType<Sound>();
	Sound *snd = base;

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
            if ((unsigned char)buffer[0] >= 0x80) {  // print command in hex
               printf("0x%x ", (unsigned char)buffer[0]);
            } else {
               printf("%d ", (unsigned char)buffer[0]);
            }
            fflush(stdout);
            //if (count % 20 == 0) {
               printf("\n");
            //}
         }
      }




	int pcmreturn;

	if (ttime >= next_flush) {
	
		curindex = 2048;
		if ((curindex - lastindex) > 1) std::cout << "LAG2: " << (curindex - lastindex) << "\n";

		for (int i = lastindex+1; i < 2048; i++) {
			data_live[4*i] = data_live[4*lastindex];
			data_live[4*i+1] = data_live[4*lastindex+1];
			data_live[4*i+2] = data_live[4*lastindex+2];
			data_live[4*i+3] = data_live[4*lastindex+3];
		}

		pcmreturn = snd_pcm_writei(pcm_handle, data_live, frames);
		if (pcmreturn < 0) {
		  snd_pcm_prepare(pcm_handle);
		  fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
			first_flush = ttime;
			flushcount = 0;
			next_flush = ttime;
	    }

		flushcount++;
		lastindex = lastindex - 2048;
		last_flush = next_flush;
		next_flush = first_flush + ((flushcount+1) * 0.04643990);

		unsigned char *temp = data_live;
		data_live = data_next;
		data_next = temp;
	}

}

extern "C" void finalise() {
	/* Stop PCM device and drop pending frames */
    snd_pcm_drop(pcm_handle);

    /* Stop PCM device after pending frames have been played */ 
    snd_pcm_drain(pcm_handle);
}
