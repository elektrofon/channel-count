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
