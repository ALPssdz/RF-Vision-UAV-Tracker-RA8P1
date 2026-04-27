# RF-Vision RA8P1 Edge Terminal

This firmware is the RA8P1/RT-Thread migration target for the original
Orange Pi 5 RF-Vision-UAV-Tracker system.

## Purpose

The original system runs on a high-performance RK3588 host and includes:

- PyQt upper-computer GUI.
- K230 vision node integration.
- YOLO/RKNN waterfall detection.
- CAF-FFT protocol fingerprinting.
- SQLite evidence persistence.

The RA8P1 terminal removes those heavy components. Its job is to stay close to
the SDR hardware, run a low-cost RF pre-detector, and send alert/status packets
to a center machine. The center machine then performs map display, history
storage, and higher-level multi-terminal fusion.

## Current Firmware State

The original RGB LED blink demo has been replaced with a small RT-Thread edge
application skeleton:

- `src/hal_entry.c`: application entry, thread creation, mock SDR scan loop.
- `src/rf_detector_simple.c`: fixed-point RSSI plus kurtosis detector.
- `src/telemetry.c`: console telemetry sink, ready to be replaced by UDP/MQTT.
- `src/status_led.c`: local LED state indication.
- `src/app_config.h`: tunable thresholds, device ID, and center host settings.

The SDR input is currently mocked by `src/sdr_frontend_mock.c`.
It periodically raises a synthetic 5785 MHz event so the RT-Thread task flow,
LED state, and telemetry formatting can be verified before the real SDR driver
is connected.

## Runtime Behavior

On boot, the firmware prints a boot packet:

```json
{"type":"boot","device":"RA8P1-RF-EDGE-001","seq":1,"uptime_ms":120,"platform":"RA8P1","os":"RT-Thread"}
```

During scanning, it prints status packets:

```json
{"type":"status","device":"RA8P1-RF-EDGE-001","seq":2,"uptime_ms":250,"freq_mhz":5745,"score_q10":980,"confidence":31,"confirm":0}
```

When the lightweight detector confirms a suspicious RF event, it prints an
alert packet:

```json
{"type":"uav_alert","device":"RA8P1-RF-EDGE-001","seq":5,"uptime_ms":2250,"freq_mhz":5785,"score_q10":2648,"confidence":86,"confirm":3}
```

## LED States

- Blue blinking: idle or boot state.
- Green blinking: RF scan is running.
- Red blinking: confirmed RF alert.


## Terminal Commands

The firmware exports several MSH commands for terminal-side self-test:

```text
rf_info         Show terminal configuration and selected transport.
rf_send_status  Send one synthetic status packet.
rf_send_test    Send one synthetic UAV alert packet.
```

These commands are useful before the SDR and network are connected.

## Transport Selection

The default telemetry transport is console, configured in `src/app_config.h`:

```c
#define APP_TELEMETRY_TRANSPORT APP_TELEMETRY_TRANSPORT_CONSOLE
```

After Ethernet and RT-Thread SAL/lwIP are enabled, switch it to:

```c
#define APP_TELEMETRY_TRANSPORT APP_TELEMETRY_TRANSPORT_UDP
```

The UDP backend sends packets to `APP_TELEMETRY_CENTER_HOST:APP_TELEMETRY_CENTER_PORT`.
## Next Work

1. Replace `src/sdr_frontend_mock.c` with the real SDR sample driver.
2. Maintain per-sector noise floor estimates from real IQ blocks.
3. Enable the RT-Thread network stack for Ethernet or a 5G modem.
4. Replace console telemetry with UDP, MQTT, or another center-host protocol. Use `tools/udp_alert_receiver.py` to test the center side.
5. Tune `APP_SCORE_THRESHOLD_Q10` and `APP_CONFIRM_REQUIRED` using field data.

More detail is in `docs/ra8p1_migration_plan.md`.




