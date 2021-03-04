#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include <time.h>

#include <mosquitto.h>
#include <wiringPi.h>
#include <confuse.h>


/* *** configuration options ***
/* have to use long int because of CFG_SIMPLE_INT */
static char *host = NULL;
static long int port = 0;
static long int keepalive = 0;
static char *user = NULL;
static char *password = NULL;
static char *topic = NULL;
static char *msg = NULL;
static long int ring_pin = 0;
static long int ring_debounce_ms = 0;

/* internal global datastructures */
static struct mosquitto *mosq = NULL;

void mosq_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	/* Pring all log messages regardless of level. */
	switch(level){
	//case MOSQ_LOG_DEBUG:
	//case MOSQ_LOG_INFO:
	//case MOSQ_LOG_NOTICE:
	  case MOSQ_LOG_WARNING:
	  case MOSQ_LOG_ERR: {
            printf("%i:%s\n", level, str);
	  }
	}
}

void mqtt_setup() {
        mosquitto_lib_init();
        mosq = mosquitto_new(NULL, true /* always clean session */, NULL);
		
        if(!mosq) {
                fprintf(stderr, "Error: Out of memory.\n");
                exit(1);
        }
        mosquitto_log_callback_set(mosq, mosq_log_callback);
        mosquitto_username_pw_set(mosq, user, password);
        if(mosquitto_connect(mosq, host, (int)port, (int)keepalive)){
                fprintf(stderr, "Error: Unable to connect to MQTT host\n");
                exit(1);
        }
        int loop = mosquitto_loop_start(mosq);
        if(loop != MOSQ_ERR_SUCCESS) {
                fprintf(stderr, "Error: Unable to start mosquitto loop: %i\n", loop);
                exit(1);
        }
}

void ring_detected(void) {
	static unsigned long lastISRTime = 0;
	unsigned long currentISRTime = millis();

	if(currentISRTime - lastISRTime > ring_debounce_ms) {
		int snd = mosquitto_publish(mosq, NULL, topic, strlen(msg), msg, 0, 0);
        	if(snd != 0) {
			fprintf(stderr, "Error: mqtt_send error=%i\n", snd);
		}
	}
	lastISRTime = currentISRTime;
}

int main(int argc, char **argv) {
	cfg_opt_t opts[] = {
		CFG_SIMPLE_STR("host", &host),
		CFG_SIMPLE_INT("port", &port),
		CFG_SIMPLE_INT("keepalive", &keepalive),
		CFG_SIMPLE_STR("user", &user),
		CFG_SIMPLE_STR("password", &password),
		CFG_SIMPLE_STR("topic", &topic),
		CFG_SIMPLE_STR("msg", &msg),
		CFG_SIMPLE_INT("ring_pin", &ring_pin),
		CFG_SIMPLE_INT("ring_debounce_ms", &ring_debounce_ms),
		CFG_END()
	};
	cfg_t *cfg;

	printf("Reading configuration...\n");
	cfg = cfg_init(opts, 0);
	cfg_parse(cfg, argc > 1 ? argv[1] : "ringdong.conf");

	if(host == NULL ||
	   port == 0 ||
	   keepalive == 0 ||
	   user == NULL ||
	   password  == NULL ||
	   topic  == NULL ||
	   msg  == NULL ||
	   ring_pin  == 0 ||
	   ring_debounce_ms  == 0) {
		fprintf (stderr, "Error: Invalid or missing configuration options\n");
		exit(1);
	}

	printf("Setting up mosquitto...\n");
	mqtt_setup();

	printf("Setting up wiringPi...\n");
	fflush(stdout);

	if (wiringPiSetup () < 0)
	{
	  fprintf (stderr, "Error: Unable to setup wiringPi: %s\n", strerror (errno)) ;
	  return 1 ;
        }
	printf("Setting up ISR routine...\n");
	if (wiringPiISR ((int)ring_pin, INT_EDGE_RISING, &ring_detected) < 0)
	  {
	    fprintf (stderr, "Error: Unable to setup ISR: %s\n", strerror (errno)) ;
	    return 1 ;
	  }
#if 0
	printf("Trying to drop root privileges...\n");
	if (getuid() == 0) {
	/* process is running as root, drop privileges */
		 if (setgid(atoi(getenv("SUDO_GID"))) != 0)
		 printf("setgid: Unable to drop group privileges: %s", strerror(errno));
		 if (setuid(atoi(getenv("SUDO_UID"))) != 0)
		 printf("setuid: Unable to drop user privileges: %s", strerror(errno));
	}
#endif
	fflush(stdout);
	pause();

	cfg_free(cfg);
	free(host);
	free(user);
	free(password);
	free(topic);
	free(msg);

	return 0;
}
