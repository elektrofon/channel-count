#include <getopt.h>
#include <alsa/asoundlib.h>

static char *device = "hw:0,0";

static int get_input_channel_count() {
	snd_pcm_t *pcm;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_hw_params_alloca(&hw_params);
	int err;
	unsigned int min, max;
	unsigned int i;

	err = snd_pcm_open(&pcm, device, SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK);
	if (err < 0) {
		fprintf(stderr, "Error opening input device '%s': %s\n", device, snd_strerror(err));

		return 1;
	}

	snd_pcm_hw_params_alloca(&hw_params);
	err = snd_pcm_hw_params_any(pcm, hw_params);
	if (err < 0) {
		fprintf(stderr, "Error getting hardware parameters: %s\n", snd_strerror(err));
		snd_pcm_close(pcm);

		return 1;
	}

	err = snd_pcm_hw_params_get_channels_min(hw_params, &min);
	if (err < 0) {
		fprintf(stderr, "Error getting minimum input channels count: %s\n", snd_strerror(err));
		snd_pcm_close(pcm);

		return 1;
	}

	err = snd_pcm_hw_params_get_channels_max(hw_params, &max);
	if (err < 0) {
		fprintf(stderr, "Error getting maximum input channels count: %s\n", snd_strerror(err));
		snd_pcm_close(pcm);

		return 1;
	}

	if (!snd_pcm_hw_params_test_channels(pcm, hw_params, max)) {
		printf("Inputs: %u\n", max);
	} else {
		fprintf(stderr, "Error verifying input channels count: %s\n", snd_strerror(err));
		snd_pcm_close(pcm);

		return 1;
	}

	snd_pcm_close(pcm);

	return 0;
}

static int get_output_channel_count() {
	snd_pcm_t *pcm;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_hw_params_alloca(&hw_params);
	int err;
	unsigned int max;
	unsigned int i;

	err = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
	if (err < 0) {
		fprintf(stderr, "Error opening device '%s': %s\n", device, snd_strerror(err));

		return 1;
	}

	snd_pcm_hw_params_alloca(&hw_params);
	err = snd_pcm_hw_params_any(pcm, hw_params);
	if (err < 0) {
		fprintf(stderr, "Error getting hardware output parameters: %s\n", snd_strerror(err));
		snd_pcm_close(pcm);

		return 1;
	}

	err = snd_pcm_hw_params_get_channels_max(hw_params, &max);
	if (err < 0) {
		fprintf(stderr, "Error getting maximum output channels count: %s\n", snd_strerror(err));
		snd_pcm_close(pcm);

		return 1;
	}

	if (!snd_pcm_hw_params_test_channels(pcm, hw_params, max)) {
		printf("Outputs: %u\n", max);
	} else {
		fprintf(stderr, "Error verifying output channels count: %s\n", snd_strerror(err));
		snd_pcm_close(pcm);

		return 1;
	}

	snd_pcm_close(pcm);

	return 0;
}

static void remove_char(char *str, char remove) {
	char *src, *dst;

	for (src = dst = str; *src != '\0'; src++) {
		*dst = *src;
		if (*dst != remove) dst++;
	}

	*dst = '\0';
}

static void prepend(char *str, const char *prepend) {
	size_t len = strlen(prepend);
	memmove(str + len, str, strlen(str) + 1);
	memcpy(str, prepend, len);
}

static void help(void) {
	int k;
	printf(
		"Usage: channelcnt [OPTION]...\n"
		"-h,--help  help\n"
		"-D,--device    device\n"
		"-U,--usb-name  usb device name\n"
		"\n"
	);
}

int main(int argc, char **argv) {
	struct option long_option[] = {
		{"device", 1, NULL, 'D'},
		{"usb-name", 1, NULL, 'U'},
		{NULL, 0, NULL, 0},
	};

	int morehelp = 0;
	int err;

	while (1) {
		int c;
		if ((c = getopt_long(argc, argv, "hD:U:", long_option, NULL)) < 0) {
			break;
		}

		switch (c) {
			case 'h':
				morehelp++;
				break;
			case 'D':
				device = strdup(optarg);
				break;
			case 'U':
				{
					char *usb_name = strdup(optarg);
					remove_char(usb_name, '-');
					remove_char(usb_name, ' ');
					char *s = malloc(100);
					strcpy(s, usb_name);
					prepend(s, "hw:");

					device = strdup(s);

					free(usb_name);
					free(s);
				}

				break;
		}
	}

	if (morehelp) {
		help();

		return 0;
	}

	printf("Device: %s\n", device);

	err = get_input_channel_count(device);
	if (err < 0) {
		return 1;
	}

	err = get_output_channel_count(device);
	if (err < 0) {
		return 1;
	}

	snd_config_update_free_global();

	return 0;
}
