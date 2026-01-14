################################################################################
#
# display-test
#
################################################################################

DISPLAY_TEST_VERSION = 1.0
DISPLAY_TEST_SITE = $(BR2_EXTERNAL_EREADER_PATH)/src/display-test
DISPLAY_TEST_SITE_METHOD = local
DISPLAY_TEST_LICENSE = Custom
DISPLAY_TEST_LICENSE_FILES = README.md

# Build commands
# The Makefile in src/display-test is already set up for cross-compilation
define DISPLAY_TEST_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(TARGET_CONFIGURE_OPTS) \
		BUILD_TYPE=release \
		-C $(@D)
endef

# Install binary to /usr/bin
define DISPLAY_TEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/display-test \
		$(TARGET_DIR)/usr/bin/display-test
endef

# Install init script for automatic startup
define DISPLAY_TEST_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 0755 $(BR2_EXTERNAL_EREADER_PATH)/package/display-test/S99display-test \
		$(TARGET_DIR)/etc/init.d/S99display-test
endef

$(eval $(generic-package))
