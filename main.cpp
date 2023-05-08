#include <iostream>
#include <systemd/sd-bus.h>

double get_percentage() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *msg = nullptr;
    sd_bus *bus = nullptr;
    sd_bus_default_system(&bus);
    int r = sd_bus_open_system(&bus);

    // Getting battery percentage
    r = sd_bus_call_method(bus,
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
        std::cerr << "Failed to call method: " << strerror(-r) << '\n';
        sd_bus_error_free(&error);
        return 1;
    }
    double percentage;
    r = sd_bus_message_read(msg, "v", "d", &percentage);
    if (r < 0) {
        std::cerr << "Failed to read percentage: " << strerror(-r) << '\n';
        sd_bus_error_free(&error);
        return 1;
    }
    sd_bus_message_unref(msg);
    sd_bus_unref(bus);
    return percentage;
}

int main()
{
    sd_bus_message *msg = nullptr;
    sd_bus *bus = nullptr;
    sd_bus_default_user(&bus);
    
    double percentage = get_percentage();

    // Sending notification to user if percentage is less than 30%
    if (percentage < 30) {
        sd_bus_message *notify = nullptr;

        // Creating the notification
        int r = sd_bus_message_new_method_call(bus,
                                               &notify,
                                               "org.freedesktop.Notifications",
                                               "/org/freedesktop/Notifications",
                                               "org.freedesktop.Notifications",
                                               "Notify");
        if (r < 0) {
            std::cerr << "Failed to create notification: " << strerror(-r) << '\n';
            return r;
        }

        // Appending the arguments to notification
        std::string percentage_str = "Low battery: " + std::to_string(static_cast<int>(percentage)) + "%";
        r = sd_bus_message_append(notify,
                                  "susssasa{sv}i",
                                  "battery-monitor",
                                  0,
                                  "dialog-information",
                                  "Battery monitor",
                                  percentage_str.c_str(),
                                  0,
                                  0,
                                  -1);
        if (r < 0) {
            std::cerr << "Failed to append arguments to notification: " << strerror(-r) << '\n';
            return r;
        }

        // Sending the notification to user
        r = sd_bus_call(bus, notify, 0, nullptr, nullptr);
        if (r < 0) {
            std::cerr << "Failed to send notification: " << strerror(-r) << '\n';
            return r;
        }

        sd_bus_message_unref(notify);
    }

    sd_bus_message_unref(msg);
    sd_bus_unref(bus);
}
