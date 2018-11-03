# Building with Docker

Building in Docker is remarkably easy.

## Build a container with toolchain

```
docker build -t inav .
```

## Building firmware with Docker on Ubuntu

Build specified target
```
sh build.sh SPRACINGF3
```

## Building firmware with Docker on Windows 10

Path in Docker on Windows works in a _strange_ way, so you have to provide full path for `docker run` command. For example:

`docker run --rm -v //c/Users/pspyc/Documents/Projects/inav:/home/src/ flyandi/docker-inav make TARGET=AIRBOTF4`

So, `c:\Users\pspyc\Documents\Projects\inav` becomes `//c/Users/pspyc/Documents/Projects/inav`

Tip: Run all commands from powershell (if you are using windows of course)
1: Register an account at docker.com and download https://www.docker.com/products/docker-desktop
2: After installing docker desktop make sure that you login to docker desktop with the docker.com account. Try to run "docker login" from command line (powershell).
3: Download docker image "flyandi/docker-inav" with command "docker pull flyandi/docker-inav"
4: Download source code e.g. https://github.com/iNavFlight/inav/archive/master.zip and unzip to e.g. "c:\inav"
5: run "cd c:\inav"
6: run command "docker run --rm -v //c/inav:/home/src/ flyandi/docker-inav make TARGET=OMNIBUSF4V3". Replace "OMNIBUSF4V3" with the firmware to your FC.

From now on you can do changes to INAV code (Tip: Visual Studio Code is free) and run command 6 to compile a hex file.
Note that after compilation the hex file is stored here: "c:\inav\obj\inav_2.0.0_OMNIBUSF4V3.hex"
