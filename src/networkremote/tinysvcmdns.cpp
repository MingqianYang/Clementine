#include "tinysvcmdns.h"

extern "C" {
#include "mdnsd.h"
}

#include <QHostInfo>
#include <QNetworkInterface>
#include <QtEndian>

#include "core/logging.h"

TinySVCMDNS::TinySVCMDNS()
    : mdnsd_(NULL) {
  // Get our hostname
  QString host = QHostInfo::localHostName();
  
  // Get all network interfaces
  QList<QNetworkInterface> network_interfaces = QNetworkInterface::allInterfaces();
  foreach (QNetworkInterface network_interface, network_interfaces) {
    // Only use up and non loopback interfaces
    if (network_interface..flags().testFlag(a.IsUp) 
     && !network_interface..flags().testFlag(a.IsLoopBack))
    {
      uint32_t ipv4 = 0;
      
      // Now check all network addresses for this device
      QList<QNetworkAddressEntry> network_address_entries = a.addressEntries();
      foreach (QNetworkAddressEntry network_address_entry, network_address_entries) {
        QHostAddress host_address = network_address_entry.ip();
        if (host_address.protocol() == QAbstractSocket::IPv4Protocol) {
          ipv4 = qToBigEndian(host_address.toIPv4Address());
        }
      }
      
      // Now start the service
      mdnsd* mdnsd = mdnsd_start_bind(ipv4);
  
      mdnsd_set_hostname(
        mdnsd,
        QString(host + ".local").toUtf8().constData(),
        ip_address);
        
      // Add to the list
      mdnsd_.append(mdnsd);
    }
  }
}

TinySVCMDNS::~TinySVCMDNS() {
  if (mdnsd_) {
    mdnsd_stop(mdnsd_);
  }
}

void TinySVCMDNS::PublishInternal(
    const QString& domain,
    const QString& type,
    const QByteArray& name,
    quint16 port) {
  if (!mdnsd_) {
    return;
  }

  // Some pointless text, so tinymDNS publishes the service correctly.
  const char* txt[] = {
    "cat=nyan",
    NULL
  };
  
  foreach(mdnsd* mdnsd, mdnsd_) {
    mdnsd_register_svc(
        mdnsd,
        name.constData(),
        QString(type + ".local").toUtf8().constData(),
        port,
        NULL,
        txt);
  }
}