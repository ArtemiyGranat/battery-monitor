#include <iostream>
#include <systemd/sd-bus.h>

int main() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *msg = nullptr;
    sd_bus *bus = nullptr;
    sd_bus_default_user(&bus);

    // Getting battery percentage
    int r = sd_bus_call_method(bus,
                               "org.freedesktop.UPower",
                               "/org/freedesktop/UPower/devices/battery_BAT0",
                               "org.freedesktop.DBus.Properties",
                               "Get",
                               &error,
                               &msg,
                               "ss",
                               "org.freedesktop.UPower.Device",
                               "Percentage");
    if (r < 0) {
        std::cerr << "Failed to call method: " << strerror(-r) << std::endl;
        sd_bus_error_free(&error);
        return 1;
    }
    double percentage;
    r = sd_bus_message_read(msg, "v", "d", &percentage);
    if (r < 0) {
        std::cerr << "Failed to read percentage: " << strerror(-r) << std::endl;
        sd_bus_error_free(&error);
        return 1;
    }
    
    // Sending notification to user if percentage is less than 30%
    if (percentage < 30) {
        int r_notify = sd_bus_message_new_method_call(bus, &msg, "org.freedesktop.Notifications",
            "/org/freedesktop/Notifications", "org.freedesktop.Notifications", "Notify");
        if (r_notify < 0) {
            std::cerr << "Failed to call method: " << strerror(-r_notify) << '\n';
            return -1;
        }

        std::string percentage_str = "Low battery: " + std::to_string(static_cast<int>(percentage)) + "%";

        r_notify = sd_bus_message_append(msg, "susssasa{sv}i", "battery-monitor", 0, "dialog-information",
            "Battery monitor", percentage_str.c_str(), 0, 0, -1);
        if (r_notify < 0) {
            std::cerr << "Failed to send notification: " << strerror(-r_notify) << '\n';
            sd_bus_message_unref(msg);
            return -1;
        }

        r_notify = sd_bus_call(bus, msg, 0, nullptr, nullptr);
        if (r_notify < 0) {
            std::cerr << "Failed to call method: " << strerror(-r) << '\n';
            sd_bus_message_unref(msg);
            return -1;
        }
    }
    sd_bus_message_unref(msg);
    sd_bus_unref(bus);
}
