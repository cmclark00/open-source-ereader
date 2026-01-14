################################################################################
#
# button-test
#
################################################################################

BUTTON_TEST_VERSION = 1.0
BUTTON_TEST_SITE = $(BR2_EXTERNAL_EREADER_PATH)/src/button-test
BUTTON_TEST_SITE_METHOD = local
BUTTON_TEST_LICENSE = Custom
BUTTON_TEST_LICENSE_FILES = README.md

# Build commands
# The Makefile in src/button-test is already set up for cross-compilation
define BUTTON_TEST_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(TARGET_CONFIGURE_OPTS) \
		BUILD_TYPE=release \
		-C $(@D)
endef

# Install binary to /usr/bin
define BUTTON_TEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/button-test \
		$(TARGET_DIR)/usr/bin/button-test
endef

$(eval $(generic-package))
