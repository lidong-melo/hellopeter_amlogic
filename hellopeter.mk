#
# hellopeter
#
HELLOPETER_VERSION = 1.0
HELLOPETER_SITE = $(TOPDIR)/package/hellopeter/src
HELLOPETER_SITE_METHOD = local

HELLOPETER_CFLAGS = \
    "-I$(STAGING_DIR)/usr/include/"

define HELLOPETER_BUILD_CMDS
    $(MAKE) CC=$(TARGET_CXX) CFLAGS=$(HELLOPETER_CFLAGS) -C $(@D) all
endef



define HELLOPETER_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/hellopeter $(TARGET_DIR)/usr/bin/hellopeter
endef

define HELLOPETER_INSTALL_CLEAN_CMDS
    $(MAKE) CC=$(TARGET_CXX) -C $(@D) clean
endef

$(eval $(generic-package))