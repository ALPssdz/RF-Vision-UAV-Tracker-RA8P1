# RF-Vision RA8P1 Migration Plan

This project is the downgraded edge-terminal version of the Orange Pi 5
RF-Vision-UAV-Tracker system.

## Target Topology

Original topology:

```text
ZYNQ/AD9364 SDR -> Orange Pi 5/RK3588 -> K230 vision node -> Qt GUI -> SQLite
```

RA8P1 topology:

```text
SDR front-end -> RA8P1 edge detector -> Ethernet/5G -> center server map
```

The RA8P1 node should only decide whether a suspicious RF event exists, then
send a compact telemetry packet to the center host. The center host keeps the
heavy UI, map display, history database, model inference, and multi-node fusion.

## Removed Or Downgraded Features

Removed:

- K230 vision stream and UDP visual telemetry.
- PyQt5 full upper-computer UI.
- SQLite image/evidence persistence.
- YOLO/RKNN waterfall detection path.
- Full CAF-FFT S3 protocol fingerprinting as a real-time mandatory stage.

Kept in simplified form:

- Three 5.8 GHz sector scan: 5745 / 5785 / 5825 MHz.
- Energy based RF pre-detection.
- Burst-sensitive kurtosis weighting.
- Elastic confirmation streak to reduce single-sample false alarms.
- Alert/status uplink packet shape.
- Local LED state indication.

## RA8P1 Firmware Modules

Current firmware skeleton:

- `src/hal_entry.c`
  - RT-Thread entry point.
  - Creates the RF detection and heartbeat threads.
  - Uses a mock SDR source until the real SDR driver is connected.

- `src/rf_detector_simple.c`
  - Lightweight detector.
  - Computes a fixed-point score from RSSI and kurtosis.
  - Uses a confirmation streak before raising an alert.

- `src/telemetry.c`
  - Console telemetry sink for now.
  - Later replacement point for Ethernet/5G transport.

- `src/status_led.c`
  - Blue: idle.
  - Green blinking: scanning.
  - Red blinking: alert.

## Simplified RF Algorithm

The RA8P1 side should not generate 640x640 waterfall images or run YOLO. The
first firmware stage uses only compact features:

```text
score = RSSI / noise_floor + kurtosis_excess
alert_candidate = score >= threshold
confirmed_alert = alert_candidate persists for N scan ticks
```

Recommended real SDR feature extraction:

- Use DMA or block read to collect a short IQ window per sector.
- Remove DC offset.
- Compute average power.
- Compute fourth-order kurtosis.
- Maintain a slow noise-floor estimate per sector.
- Feed those values into `rf_detector_process()`.

Optional later upgrade:

- Add a very small Goertzel/FFT-bin cyclic feature check for one or two known
  OcuSync symbol-rate regions.
- Keep it as a second-stage confirmation only when CPU budget allows it.

## Telemetry Contract

Boot packet:

```json
{"type":"boot","device":"RA8P1-RF-EDGE-001","seq":1,"uptime_ms":120,"platform":"RA8P1","os":"RT-Thread"}
```

Status packet:

```json
{"type":"status","device":"RA8P1-RF-EDGE-001","seq":2,"uptime_ms":250,"freq_mhz":5785,"score_q10":1024,"confidence":33,"confirm":1}
```

Alert packet:

```json
{"type":"uav_alert","device":"RA8P1-RF-EDGE-001","seq":5,"uptime_ms":2250,"freq_mhz":5785,"score_q10":2300,"confidence":74,"confirm":3}
```

Fields to add once hardware support is ready:

- GNSS latitude/longitude, or fixed terminal coordinates from configuration.
- RSSI in dBm if the SDR front-end provides calibrated gain.
- Network sequence number.
- UTC timestamp.
- Terminal health flags.

## Next Integration Steps

1. Replace `src/sdr_frontend_mock.c` with an SDR driver backend that implements `sdr_frontend_read()`.
2. Enable RT-Thread network stack and choose Ethernet or 5G modem transport.
3. Replace console output in `src/telemetry.c` with UDP or MQTT.
4. Add center-server receiver that stores terminal events and renders them on a map.
5. Tune `APP_SCORE_THRESHOLD_Q10` and `APP_CONFIRM_REQUIRED` using field IQ data.

## Terminal Self-Test Commands

The RA8P1 terminal exports MSH commands while `FINSH_USING_MSH` is enabled:

```text
rf_info         Print device ID, thresholds, center endpoint, and transport.
rf_send_status  Emit one synthetic status packet through the active transport.
rf_send_test    Emit one synthetic alert packet through the active transport.
```

## UDP Transport Path

`src/telemetry_transport_udp_stub.c` already contains the UDP sender guarded by
`APP_TELEMETRY_TRANSPORT == APP_TELEMETRY_TRANSPORT_UDP` and `RT_USING_SAL`.
The terminal remains in console mode until Ethernet, lwIP, and SAL are enabled.

Switching to UDP requires:

1. Enable RT-Thread Ethernet/lwIP/SAL configuration.
2. Confirm RA8P1 has an IP address on the switch LAN.
3. Set `APP_TELEMETRY_TRANSPORT` to `APP_TELEMETRY_TRANSPORT_UDP`.
4. Keep `APP_TELEMETRY_CENTER_HOST` as the center machine IP.

