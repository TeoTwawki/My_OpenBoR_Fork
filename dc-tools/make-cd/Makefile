CDRECORD    = cdrecord dev=IODVDServices
SCRAMBLE    = scramble
UNMOUNT     = diskutil unmountDisk disk1
DD          = dd
CP          = cp
MKISOFS     = mkisofs
CAT         = cat
TOUCH       = touch
1ST_READ    = /usr/local/dcdev/dcload/dcload-ip/target-src/1st_read/1st_read.bin
IP_BIN      = /usr/local/dcdev/dcload/dcload-ip/make-cd/IP.BIN

all: burn-data

1st_read.bin: $(1ST_READ)
	$(SCRAMBLE) $(1ST_READ) cd/1st_read.bin

IP.BIN: $(IP_BIN)
	$(CP) $(IP_BIN) IP.BIN

audio.raw:
	$(DD) if=/dev/zero of=audio.raw bs=2352 count=300

burn-audio: audio.raw
	$(UNMOUNT)
	$(CDRECORD) -v -speed=8 -tao -multi -audio audio.raw
	touch burn-audio

data.raw: 1st_read.bin IP.BIN burn-audio
	$(MKISOFS) -l -r -C `$(CDRECORD) -msinfo` -G IP.BIN -o data.raw cd/
	touch burn-data

burn-data: data.raw
	$(UNMOUNT)
	$(CDRECORD) -v -speed=8 -tao -multi -xa data.raw

.PHONY : clean
clean:
	rm -f cd/1st_read.bin IP.BIN audio.raw data.raw burn-audio burn-data
