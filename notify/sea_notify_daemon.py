import dbus
import dbus.service
import dbus.mainloop.glib
from gi.repository import GLib
from subprocess import Popen
from os.path import join, dirname, abspath
from sys import argv

current_dir = dirname(abspath(__file__))
script_path = join(current_dir, argv[1])


class NotificationDaemon(dbus.service.Object):
    def __init__(self, bus, path="/org/freedesktop/Notifications"):
        super().__init__(bus, path)

    @dbus.service.method(
        "org.freedesktop.Notifications",
        in_signature="susssasa{sv}i",
        out_signature="u"
    )
    def Notify(self, app_name, replaces_id, app_icon, summary, body, actions, hints, expire_timeout):
        # spawn bash script
        Popen([script_path, app_name, summary,
              body, str(expire_timeout), app_icon])
        return 1

    @dbus.service.method(
        "org.freedesktop.Notifications",
        in_signature="",
        out_signature="as"
    )
    def GetCapabilities(self):
        return [
            "body",
            "icon-static",
            "icon-multi"
        ]

    @dbus.service.method(
        "org.freedesktop.Notifications",
        in_signature="",
        out_signature="ssss"
    )
    def GetServerInformation(self):
        return (
            "sea-notify",
            "github.com/felix-kyun",
            "1.0",
            "1.2"
        )

    @dbus.service.method(
        "org.freedesktop.Notifications",
        in_signature="u",
        out_signature=""
    )
    def CloseNotification(self, id):
        pass


if __name__ == "__main__":
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    session_bus = dbus.SessionBus()
    name = dbus.service.BusName("org.freedesktop.Notifications", session_bus)
    daemon = NotificationDaemon(session_bus)
    print("Press Ctrl+C to quit.")
    GLib.MainLoop().run()
