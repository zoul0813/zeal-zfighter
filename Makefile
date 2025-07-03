BIN=zfighter.bin


ENABLE_GFX=1
ENABLE_SOUND=1

ifdef BREAK
ZOS_CFLAGS += -DBREAK
endif

ifndef ZGDK_PATH
	$(error "Failure: ZGDK_PATH variable not found. It must point to ZGDK path.")
endif

GFX_COMPRESSED=1
GFX_STRIP=160

include $(ZGDK_PATH)/base_sdcc.mk


run:
	$(ZEAL_NATIVE_BIN) -H bin -r $(ZEAL_NATIVE_ROM) #-t tf.img -e eeprom.img

native: all run
