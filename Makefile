include $(TOPDIR)/rules.mk

PKG_NAME:=mosq-subscriber
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/mosq-subscriber
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Mosquitto Subscriber Test
	DEPENDS:=+libmosquitto
endef

define Package/mosq-subscriber/description
	Mosquitto subscriber test program
endef

define Package/mosq-subscriber/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mosq-subscriber $(1)/usr/bin

endef

$(eval $(call BuildPackage,mosq-subscriber))
