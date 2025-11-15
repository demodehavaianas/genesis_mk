[![CI](https://github.com/demodehavaianas/genesis_mk/actions/workflows/ci.yml/badge.svg)](https://github.com/demodehavaianas/genesis_mk/actions/workflows/ci.yml)
[![CD](https://github.com/demodehavaianas/genesis_mk/actions/workflows/cd.yml/badge.svg)](https://github.com/demodehavaianas/genesis_mk/actions/workflows/cd.yml)
[![Wiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/demodehavaianas/genesis_mk)
![GitHub all releases](https://img.shields.io/github/downloads/demodehavaianas/genesis_mk/total)

## Mortal Kombat Plus

Material de estudo para desenvolvimento de um jogo de luta para o Mega Drive usando a SGDK me baseando no jogo Mortal Kombat.

Versão web: [mk1-plus](https://demodehavaianas.github.io/genesis_mk/wasm/) | [doc](https://demodehavaianas.github.io/genesis_mk/doc)

### Build

```shell
make SGDK_VERSION=2.11 compile
````

Rom em `out/rom.bin`

### Retroarch

```shell
make run
```
<img width="991" height="716" alt="Screenshot 2025-11-11 at 7 16 49 PM" src="https://github.com/user-attachments/assets/ae236c10-8045-408b-a9aa-fa5e5d37d2a5" />

### References

- Algumas músicas retiradas de [vgmrips](https://vgmrips.net/packs/pack/mortal-kombat-mega-drive-genesis)

- Ripagem de alguns sons usando o método descrito em [vgmpf](https://www.vgmpf.com/Wiki/index.php?title=VGM_Logging:_GEN,_GG,_SMS)

- Framework de desenvolvimento [SGDK](https://github.com/Stephane-D/SGDK)

- SDGK via docker [docker-sgdk](https://gitlab.com/doragasu/docker-sgdk)

- Scripts para o Aseprite [sgdk-aseprite-scripts](https://github.com/retrodevbr/sgdk-aseprite-scripts)

- Images em [spriters-resource](https://www.spriters-resource.com/arcade/mortalkombat/)
