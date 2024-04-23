TARGET = uqm
OBJS = src/regex/regex.o \
    src/getopt/getopt1.o \
    src/getopt/getopt.o \
    src/sc2code/ships/zoqfot/zoqfot.o \
    src/sc2code/ships/yehat/yehat.o \
    src/sc2code/ships/vux/vux.o \
    src/sc2code/ships/utwig/utwig.o \
    src/sc2code/ships/urquan/urquan.o \
    src/sc2code/ships/umgah/umgah.o \
    src/sc2code/ships/thradd/thradd.o \
    src/sc2code/ships/syreen/syreen.o \
    src/sc2code/ships/supox/supox.o \
    src/sc2code/ships/spathi/spathi.o \
    src/sc2code/ships/slylandr/slylandr.o \
    src/sc2code/ships/sis_ship/sis_ship.o \
    src/sc2code/ships/shofixti/shofixti.o \
    src/sc2code/ships/probe/probe.o \
    src/sc2code/ships/pkunk/pkunk.o \
    src/sc2code/ships/orz/orz.o \
    src/sc2code/ships/mycon/mycon.o \
    src/sc2code/ships/mmrnmhrm/mmrnmhrm.o \
    src/sc2code/ships/melnorme/melnorme.o \
    src/sc2code/ships/lastbat/lastbat.o \
    src/sc2code/ships/ilwrath/ilwrath.o \
    src/sc2code/ships/human/human.o \
    src/sc2code/ships/druuge/druuge.o \
    src/sc2code/ships/chmmr/chmmr.o \
    src/sc2code/ships/chenjesu/chenjesu.o \
    src/sc2code/ships/blackurq/blackurq.o \
    src/sc2code/ships/arilou/arilou.o \
    src/sc2code/ships/androsyn/androsyn.o \
    src/sc2code/planets/surface.o \
    src/sc2code/planets/solarsys.o \
    src/sc2code/planets/scan.o \
    src/sc2code/planets/roster.o \
    src/sc2code/planets/report.o \
    src/sc2code/planets/pstarmap.o \
    src/sc2code/planets/plangen.o \
    src/sc2code/planets/planets.o \
    src/sc2code/planets/pl_stuff.o \
    src/sc2code/planets/oval.o \
    src/sc2code/planets/orbits.o \
    src/sc2code/planets/lander.o \
    src/sc2code/planets/genzoq.o \
    src/sc2code/planets/genyeh.o \
    src/sc2code/planets/genwreck.o \
    src/sc2code/planets/genvux.o \
    src/sc2code/planets/genvault.o \
    src/sc2code/planets/genutw.o \
    src/sc2code/planets/gentopo.o \
    src/sc2code/planets/genthrad.o \
    src/sc2code/planets/gensyr.o \
    src/sc2code/planets/gensup.o \
    src/sc2code/planets/genspa.o \
    src/sc2code/planets/gensol.o \
    src/sc2code/planets/gensly.o \
    src/sc2code/planets/genshof.o \
    src/sc2code/planets/gensam.o \
    src/sc2code/planets/genrain.o \
    src/sc2code/planets/genpku.o \
    src/sc2code/planets/genpet.o \
    src/sc2code/planets/genorz.o \
    src/sc2code/planets/genmyc.o \
    src/sc2code/planets/genmel.o \
    src/sc2code/planets/genilw.o \
    src/sc2code/planets/gendru.o \
    src/sc2code/planets/gencol.o \
    src/sc2code/planets/genchmmr.o \
    src/sc2code/planets/genburv.o \
    src/sc2code/planets/devices.o \
    src/sc2code/planets/cargo.o \
    src/sc2code/planets/calc.o \
    src/sc2code/libs/log/uqmlog.o \
    src/sc2code/libs/video/dukvid.o \
    src/sc2code/libs/video/vidplayer.o \
    src/sc2code/libs/video/videodec.o \
    src/sc2code/libs/video/video.o \
    src/sc2code/libs/video/vfileins.o \
    src/sc2code/libs/uio/zip/zip.o \
    src/sc2code/libs/uio/stdio/stdio.o \
    src/sc2code/libs/uio/debug.o \
    src/sc2code/libs/uio/utils.o \
    src/sc2code/libs/uio/uioutils.o \
    src/sc2code/libs/uio/uiostream.o \
    src/sc2code/libs/uio/physical.o \
    src/sc2code/libs/uio/paths.o \
    src/sc2code/libs/uio/mounttree.o \
    src/sc2code/libs/uio/mount.o \
    src/sc2code/libs/uio/match.o \
    src/sc2code/libs/uio/ioaux.o \
    src/sc2code/libs/uio/io.o \
    src/sc2code/libs/uio/gphys.o \
    src/sc2code/libs/uio/fstypes.o \
    src/sc2code/libs/uio/fileblock.o \
    src/sc2code/libs/uio/defaultfs.o \
    src/sc2code/libs/uio/charhashtable.o \
    src/sc2code/libs/time/sdl/sdltime.o \
    src/sc2code/libs/time/timecommon.o \
    src/sc2code/libs/threads/sdl/sdlthreads.o \
    src/sc2code/libs/threads/thrcommon.o \
    src/sc2code/libs/task/tasklib.o \
    src/sc2code/libs/strings/unicode.o \
    src/sc2code/libs/strings/strings.o \
    src/sc2code/libs/strings/sresins.o \
    src/sc2code/libs/strings/sfileins.o \
    src/sc2code/libs/strings/getstr.o \
    src/sc2code/libs/sound/decoders/oggaud.o \
    src/sc2code/libs/sound/decoders/modaud.o \
    src/sc2code/libs/sound/decoders/dukaud.o \
    src/sc2code/libs/sound/decoders/wav.o \
    src/sc2code/libs/sound/decoders/decoder.o \
    src/sc2code/libs/sound/mixer/nosound/audiodrv_nosound.o \
    src/sc2code/libs/sound/mixer/sdl/audiodrv_sdl.o \
    src/sc2code/libs/sound/mixer/mixer.o \
    src/sc2code/libs/sound/trackplayer.o \
    src/sc2code/libs/sound/stream.o \
    src/sc2code/libs/sound/music.o \
    src/sc2code/libs/sound/sfx.o \
    src/sc2code/libs/sound/sound.o \
    src/sc2code/libs/sound/resinst.o \
    src/sc2code/libs/sound/fileinst.o \
    src/sc2code/libs/sound/audiocore.o \
    src/sc2code/libs/resource/stringbank.o \
    src/sc2code/libs/resource/resinit.o \
    src/sc2code/libs/resource/resdata.o \
    src/sc2code/libs/resource/mapres.o \
    src/sc2code/libs/resource/loadres.o \
    src/sc2code/libs/resource/getres.o \
    src/sc2code/libs/resource/filecntl.o \
    src/sc2code/libs/resource/direct.o \
    src/sc2code/libs/resource/alist.o \
    src/sc2code/libs/memory/w_memlib.o \
    src/sc2code/libs/math/sqrt.o \
    src/sc2code/libs/math/random.o \
    src/sc2code/libs/list/list.o \
    src/sc2code/libs/input/sdl/vcontrol.o \
    src/sc2code/libs/input/sdl/keynames.o \
    src/sc2code/libs/input/sdl/input.o \
    src/sc2code/libs/input/input_common.o \
    src/sc2code/libs/heap/heap.o \
    src/sc2code/libs/graphics/sdl/rotozoom.o \
    src/sc2code/libs/graphics/sdl/sdluio.o \
    src/sc2code/libs/graphics/sdl/bbox.o \
    src/sc2code/libs/graphics/sdl/canvas.o \
    src/sc2code/libs/graphics/sdl/hq2x.o \
    src/sc2code/libs/graphics/sdl/triscan2x.o \
    src/sc2code/libs/graphics/sdl/biadv2x.o \
    src/sc2code/libs/graphics/sdl/bilinear2x.o \
    src/sc2code/libs/graphics/sdl/nearest2x.o \
    src/sc2code/libs/graphics/sdl/2xscalers_3dnow.o \
    src/sc2code/libs/graphics/sdl/2xscalers_sse.o \
    src/sc2code/libs/graphics/sdl/2xscalers_mmx.o \
    src/sc2code/libs/graphics/sdl/2xscalers.o \
    src/sc2code/libs/graphics/sdl/scalers.o \
    src/sc2code/libs/graphics/sdl/sdl_common.o \
    src/sc2code/libs/graphics/sdl/rndzoom.o \
    src/sc2code/libs/graphics/sdl/pure.o \
    src/sc2code/libs/graphics/sdl/primitives.o \
    src/sc2code/libs/graphics/sdl/opengl.o \
    src/sc2code/libs/graphics/sdl/dcqueue.o \
    src/sc2code/libs/graphics/sdl/3do_getbody.o \
    src/sc2code/libs/graphics/sdl/3do_funcs.o \
    src/sc2code/libs/graphics/sdl/3do_blt.o \
    src/sc2code/libs/graphics/widgets.o \
    src/sc2code/libs/graphics/tfb_prim.o \
    src/sc2code/libs/graphics/tfb_draw.o \
    src/sc2code/libs/graphics/resgfx.o \
    src/sc2code/libs/graphics/pixmap.o \
    src/sc2code/libs/graphics/loaddisp.o \
    src/sc2code/libs/graphics/intersec.o \
    src/sc2code/libs/graphics/gfx_common.o \
    src/sc2code/libs/graphics/frame.o \
    src/sc2code/libs/graphics/font.o \
    src/sc2code/libs/graphics/filegfx.o \
    src/sc2code/libs/graphics/drawable.o \
    src/sc2code/libs/graphics/context.o \
    src/sc2code/libs/graphics/cmap.o \
    src/sc2code/libs/graphics/clipline.o \
    src/sc2code/libs/graphics/boxint.o \
    src/sc2code/libs/file/temp.o \
    src/sc2code/libs/file/files.o \
    src/sc2code/libs/file/dirs.o \
    src/sc2code/libs/decomp/update.o \
    src/sc2code/libs/decomp/lzencode.o \
    src/sc2code/libs/decomp/lzdecode.o \
    src/sc2code/libs/callback/callback.o \
    src/sc2code/libs/callback/alarm.o \
    src/sc2code/comm/zoqfot/zoqfotc.o \
    src/sc2code/comm/yehat/yehatc.o \
    src/sc2code/comm/vux/vuxc.o \
    src/sc2code/comm/utwig/utwigc.o \
    src/sc2code/comm/urquan/urquanc.o \
    src/sc2code/comm/umgah/umgahc.o \
    src/sc2code/comm/thradd/thraddc.o \
    src/sc2code/comm/talkpet/talkpet.o \
    src/sc2code/comm/syreen/syreenc.o \
    src/sc2code/comm/supox/supoxc.o \
    src/sc2code/comm/starbas/starbas.o \
    src/sc2code/comm/spathi/spathic.o \
    src/sc2code/comm/spahome/spahome.o \
    src/sc2code/comm/slyland/slyland.o \
    src/sc2code/comm/slyhome/slyhome.o \
    src/sc2code/comm/shofixt/shofixt.o \
    src/sc2code/comm/rebel/rebel.o \
    src/sc2code/comm/pkunk/pkunkc.o \
    src/sc2code/comm/orz/orzc.o \
    src/sc2code/comm/mycon/myconc.o \
    src/sc2code/comm/melnorm/melnorm.o \
    src/sc2code/comm/ilwrath/ilwrathc.o \
    src/sc2code/comm/druuge/druugec.o \
    src/sc2code/comm/comandr/comandr.o \
    src/sc2code/comm/chmmr/chmmrc.o \
    src/sc2code/comm/blackur/blackurc.o \
    src/sc2code/comm/arilou/arilouc.o \
    src/sc2code/weapon.o \
    src/sc2code/velocity.o \
    src/sc2code/util.o \
    src/sc2code/uqmdebug.o \
    src/sc2code/trans.o \
    src/sc2code/tactrans.o \
    src/sc2code/status.o \
    src/sc2code/state.o \
    src/sc2code/starmap.o \
    src/sc2code/starcon.o \
    src/sc2code/starbase.o \
    src/sc2code/sounds.o \
    src/sc2code/sis.o \
    src/sc2code/shipyard.o \
    src/sc2code/shipstat.o \
    src/sc2code/ship.o \
    src/sc2code/setupmenu.o \
    src/sc2code/setup.o \
    src/sc2code/settings.o \
    src/sc2code/save.o \
    src/sc2code/restart.o \
    src/sc2code/process.o \
    src/sc2code/plandata.o \
    src/sc2code/pickship.o \
    src/sc2code/pickmele.o \
    src/sc2code/outfit.o \
    src/sc2code/oscill.o \
    src/sc2code/mouse_err.o \
    src/sc2code/misc.o \
    src/sc2code/menu.o \
    src/sc2code/melee.o \
    src/sc2code/master.o \
    src/sc2code/loadship.o \
    src/sc2code/load.o \
    src/sc2code/ipdisp.o \
    src/sc2code/intro.o \
    src/sc2code/intel.o \
    src/sc2code/init.o \
    src/sc2code/hyper.o \
    src/sc2code/grpinfo.o \
    src/sc2code/gravwell.o \
    src/sc2code/gravity.o \
    src/sc2code/globdata.o \
    src/sc2code/getchar.o \
    src/sc2code/gendef.o \
    src/sc2code/gameopt.o \
    src/sc2code/gameinp.o \
    src/sc2code/gameev.o \
    src/sc2code/galaxy.o \
    src/sc2code/fmv.o \
    src/sc2code/encount.o \
    src/sc2code/dummy.o \
    src/sc2code/displist.o \
    src/sc2code/demo.o \
    src/sc2code/cyborg.o \
    src/sc2code/credits.o \
    src/sc2code/confirm.o \
    src/sc2code/commglue.o \
    src/sc2code/commanim.o \
    src/sc2code/comm.o \
    src/sc2code/collide.o \
    src/sc2code/cnctdlg.o \
    src/sc2code/clock.o \
    src/sc2code/cleanup.o \
    src/sc2code/build.o \
    src/sc2code/border.o \
    src/sc2code/battle.o \
    src/starcon2.o \
    src/port.o \
    src/options.o



INCDIR = . \
    src \
    src/sc2code \
    src/sc2code/libs \
    src/regex

# DEBUG CONFIG
# CFLAGS = -g -O0 -DDEBUG -G0 -Wall -D_GNU_SOURCE=1 -D_REENTRANT -DPSP -DGFXMODULE_SDL -DSOUNDMODULE_SDL -DUSE_INTERNAL_MIKMOD -DHAVE_ZIP=1 -DHAVE_JOYSTICK -DOVCODEC_TREMOR
# LDFLAGS = -Wl,-Map,uqm.map

# RELEASE CONFIG
#CFLAGS = -O2 -G0 -Wall -D_GNU_SOURCE=1 -D_REENTRANT -DPSP -DGFXMODULE_SDL -DSOUNDMODULE_SDL -DUSE_INTERNAL_MIKMOD -DHAVE_ZIP=1 -DHAVE_JOYSTICK -DOVCODEC_TREMOR

BUILD_PRX=1

CFLAGS = -O2 -G0 -Wall -D_GNU_SOURCE=1 -D_REENTRANT -DPSP -DGFXMODULE_SDL -DSOUNDMODULE_SDL -DHAVE_ZIP=1 -DHAVE_JOYSTICK
PSP_EBOOT_TITLE = UQM 0.6.2 - R02 Mod

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 660

PSPSDK=$(shell psp-config --pspsdk-path)
PSPBIN = $(PSPSDK)/../bin

CFLAGS += $(shell $(PSPBIN)/sdl-config --cflags)
LIBS += -L$(PSPDEV)/psp/lib -lSDLmain -lSDL -lm -lGL -lSDL_image -lpspirkeyb -ljpeg -lpng -lmikmod -lvorbisidec -lvorbisfile -lz -lpspgu -lpspaudio -lpsphprm -lpspvfpu -lpsppower
EXTRA_TARGETS=EBOOT.PBP

include $(PSPSDK)/lib/build.mak

