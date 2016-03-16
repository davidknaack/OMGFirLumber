OMGFirLumber
============


Resources for the OMG FirLumber Rally

Getting started (hacking)
-------------------------

```bash
git clone https://github.com/davidknaack/OMGFirLumber.git
cd OMGFirLumber/FirLumberWeb
./setup-env.sh
```
Start a background task (tmux is a good choice for this) and
start gin. Gin will start the FirLumber server and proxy it.
When the source files change Gin will recompile and restart
the server for you.

```bash
gin -p 8080 -a 8000
```

TrackMan is the Arduino sketch that operates the hardware.
It will send events with microsecond timestamps via the serial
port. When you don't have hardware attached there is a simulator
that can be swapped in.
