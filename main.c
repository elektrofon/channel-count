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

static void help(void) {
	int k;
	printf(
		"Usage: channelcnt [OPTION]...\n"
		"-h,--help  help\n"
		"-D,--device    device\n"
		"\n"
	);
}

int main(int argc, char **argv) {
	struct option long_option[] = {
		{"device", 1, NULL, 'D'},
		{NULL, 0, NULL, 0},
	};

	int morehelp = 0;
	int err;

	while (1) {
		int c;
		if ((c = getopt_long(argc, argv, "hD:", long_option, NULL)) < 0) {
			break;
		}

		switch (c) {
			case 'h':
				morehelp++;
				break;
			case 'D':
				device = strdup(optarg);
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
