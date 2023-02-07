# flipperzero-unleashed-DumpKeystore

[![](https://upload.wikimedia.org/wikipedia/commons/f/fd/Sample_09-F9_protest_art%2C_Free_Speech_Flag_by_John_Marcotte.svg)](https://en.wikipedia.org/wiki/AACS_encryption_key_controversy)

## prerequisites

Works only with custom builds of **unleashed-firmware**!

## preparation

Take [https://github.com/DarkFlippers/unleashed-firmware](unleashed-firmware) and clone this repo into `applications_user` there.

Then run this to export internal symbols:

```bash
gsed -i 's/\,-\,subghz_keystore_alloc/\,+\,subghz_keystore_alloc/g' firmware/targets/f7/api_symbols.csv
gsed -i 's/\,-\,subghz_keystore_load/\,+\,subghz_keystore_load/g' firmware/targets/f7/api_symbols.csv
gsed -i 's/\,-\,subghz_keystore_get_data/\,+\,subghz_keystore_get_data/g' firmware/targets/f7/api_symbols.csv
```

Finally, rebuild and reflash firmware (first execution of `./fbt` may take some time to download toolchain and then to compile everything):

```bash
./fbt COMPACT=1 DEBUG=0 VERBOSE=1 updater_package copro_dist 
./fbt COMPACT=1 DEBUG=0 VERBOSE=1 FORCE=1 flash_usb 
```

## build & run

```bash
./fbt COMPACT=1 DEBUG=0 VERBOSE=1 icons resources fap_DumpKeystore

./fbt COMPACT=1 DEBUG=0 VERBOSE=1 launch_app APPSRC=applications_user/dump_keystore
# or upload manuall it from ./assets/resources/apps/DumpKeystore.fap
```

Look around for dumped keystore in the same place as original file.

## take care

![](screenshot.png)

## disclaimer

This repo does not store any propertiarty data. If you happen to own such propertiarty data, even in encrypted form you should do so legally :)

## licence

Parts of code used under GPG licence from [unleashed-firmware](https://github.com/DarkFlippers/unleashed-firmware).
