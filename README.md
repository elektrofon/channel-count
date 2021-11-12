# Channel Count

Get maximum number of input and output channels of an ALSA device.

## Usage:

Basic:

```bash
$ channel-count -D "hw:0,0"
Device: hw:0,0
Inputs: 128
Outputs: 128
```

Get number of inputs:

```bash
$ channel-count -D "hw:0,0" | awk '/Inputs/ {print $2}'
128
```

Use USB product name instead of ALSA device ID:

```bash
$ channel-count -U "ES-8"
Device: hw:ES8
Inputs: 12
Outputs: 16
```
