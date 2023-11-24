import inputs
import bluetooth


def connect_to_controller():
    print("Searching for Xbox controller...")
    nearby_devices = bluetooth.discover_devices(lookup_names=True, device_class=0x002508)

    for addr, name in nearby_devices:
        if "Xbox" in name:
            print(f"Connecting to {name} ({addr})...")
            return addr

    print("Xbox controller not found.")
    return None


def main():
    controller_addr = connect_to_controller()

    if not controller_addr:
        return

    # Create a connection to the Xbox controller using evdev
    gamepad = inputs.devices.gamepads[0]

    print("Connected to Xbox controller.")

    try:
        while True:
            events = inputs.get_key()
            for event in events:
                print(event.ev_type, event.ev_code, event.ev_value)

    except KeyboardInterrupt:
        print("Disconnecting...")
    finally:
        gamepad.close()


if __name__ == "__main__":
    main()
