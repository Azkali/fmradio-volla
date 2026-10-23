#include <QtQml>

#include "plugin.h"
#include "MediatekRadio.h"

void MediatekRadioPlugin::registerTypes(const char *uri) {
    qmlRegisterSingletonType<MediatekRadio>(uri, 1, 0, "MediatekRadio", [](QQmlEngine*, QJSEngine*) -> QObject* { return new MediatekRadio; });
}
