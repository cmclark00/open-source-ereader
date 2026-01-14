################################################################################
#
# ereader
#
################################################################################

EREADER_VERSION = 0.3.0
EREADER_SITE = $(BR2_EXTERNAL_EREADER_PATH)/src/ereader
EREADER_SITE_METHOD = local
EREADER_LICENSE = Custom
EREADER_LICENSE_FILES = README.md

# Dependencies: display-test (for EPD driver) and button-test (for input)
EREADER_DEPENDENCIES = display-test button-test

# Build commands
# The Makefile in src/ereader is already set up for cross-compilation
define EREADER_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(TARGET_CONFIGURE_OPTS) \
		-C $(@D)
endef

# Install binary to /usr/bin
define EREADER_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/ereader \
		$(TARGET_DIR)/usr/bin/ereader
endef

# Install init script for automatic startup
define EREADER_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 0755 $(BR2_EXTERNAL_EREADER_PATH)/board/ereader/rootfs-overlay/etc/init.d/S99ereader \
		$(TARGET_DIR)/etc/init.d/S99ereader
endef

$(eval $(generic-package))
