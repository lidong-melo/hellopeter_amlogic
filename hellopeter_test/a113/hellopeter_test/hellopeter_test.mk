#
# hellopeter_test
#

HELLOPETER_TEST_VERSION = 1.0
HELLOPETER_TEST_SITE = $(TOPDIR)/package/hellopeter_test/src
HELLOPETER_TEST_SITE_METHOD = local

HELLOPETER_TEST_CFLAGS = \
    "-I$(STAGING_DIR)/usr/include/"

define HELLOPETER_TEST_BUILD_CMDS
    $(MAKE) CC=$(TARGET_CXX) CFLAGS=$(HELLOPETER_TEST_CFLAGS) -C $(@D) all
endef



define HELLOPETER_TEST_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/hellopeter_test $(TARGET_DIR)/usr/bin/hellopeter_test
endef

define HELLOPETER_TEST_INSTALL_CLEAN_CMDS
    $(MAKE) CC=$(TARGET_CXX) -C $(@D) clean
endef

$(eval $(generic-package))