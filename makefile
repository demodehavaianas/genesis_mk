SGDK_VERSION := 2.11
UID := $(shell id -u)
GID := $(shell id -g)
DOCKER := $(shell which docker)
DOCKER_IMAGE := registry.gitlab.com/doragasu/docker-sgdk:v$(SGDK_VERSION)
UNAME_S := $(shell uname -s)
RETROARCH ?= $(shell which retroarch 2>/dev/null)
CORE := genesis_plus_gx
TAG_NAME := $(shell git describe --tags --exact-match 2>/dev/null || git rev-parse --short HEAD)

ifeq ($(UNAME_S),Darwin)
    RETROARCH := /Applications/RetroArch.app/Contents/MacOS/RetroArch
endif

.PHONY: compile
compile:
	docker run --rm --user ${UID}:${GID} -v "${PWD}":/workdir -w /workdir ${DOCKER_IMAGE}

shell:
	docker run -it --rm -v "${PWD}":/workdir -w /workdir --entrypoint=/bin/bash ${DOCKER_IMAGE}

clean:
	rm -rf out/* build/* src/boot/*

format:
	clang-format -i src/main.c

run:
	"${RETROARCH}" -L "${CORE}" out/rom.bin
	
zip:
	zip -9 -j mk-plus-$(TAG_NAME).zip out/rom.bin

doc:
	$(DOCKER) run --rm -u $(UID):$(GID) -v "${PWD}":/workdir -w /workdir nakatt/doxygen

wasm:	compile
	$(DOCKER) build . -f dockerfile-wasm --build-arg CORE_NAME=blastem --build-arg GAME_ROM=out/rom.bin -t wasm
	$(DOCKER) run -i -u $(UID):$(GID) -v $(PWD):/outside wasm sh -c 'cp -r /workdir/lotr/example/* /outside/web/wasm'

web:	doc	wasm

web/serve:
	$(DOCKER) run -d -p 8080:80 --name doxygen-server --rm -v "${PWD}"/web/html:/usr/share/nginx/html:ro nginx
	echo "Doxygen documentation server running at http://localhost:8080"