include $(TOPDIR)/rules.mk

PKG_NAME:=mosq-subscriber
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/mosq-subscriber
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Mosquitto Subscriber Test
	DEPENDS:=+libmosquitto +libsqlite3 +libuci +libjson-c +libcurl
endef

define Package/mosq-subscriber/description
	Mosquitto subscriber test program
endef

define Package/mosq-subscriber/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mosq-subscriber.init $(1)/etc/init.d/mosq-subscriber
	$(INSTALL_CONF) ./files/mosq-subscriber.config $(1)/etc/config/mosq-subscriber
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mosq-subscriber $(1)/usr/bin

endef

$(eval $(call BuildPackage,mosq-subscriber))
