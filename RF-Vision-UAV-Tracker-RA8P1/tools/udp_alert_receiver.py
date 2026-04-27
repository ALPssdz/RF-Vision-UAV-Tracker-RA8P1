#!/usr/bin/env python3
"""Minimal center-host UDP receiver for RA8P1 RF edge terminals."""

import argparse
import json
import socket
from datetime import datetime


def main() -> None:
    parser = argparse.ArgumentParser(description="Receive RA8P1 RF telemetry over UDP")
    parser.add_argument("--host", default="0.0.0.0", help="local bind address")
    parser.add_argument("--port", default=9000, type=int, help="local UDP port")
    args = parser.parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((args.host, args.port))
    print(f"listening on udp://{args.host}:{args.port}")

    while True:
        data, addr = sock.recvfrom(2048)
        text = data.decode("utf-8", errors="replace").strip()
        try:
            msg = json.loads(text)
        except json.JSONDecodeError:
            print(f"{datetime.now().isoformat(timespec='seconds')} {addr} invalid-json {text!r}")
            continue

        kind = msg.get("type", "unknown")
        device = msg.get("device", "unknown")
        seq = msg.get("seq", "-")
        freq = msg.get("freq_mhz", "-")
        confidence = msg.get("confidence", "-")
        print(
            f"{datetime.now().isoformat(timespec='seconds')} "
            f"{addr[0]}:{addr[1]} type={kind} device={device} "
            f"seq={seq} freq={freq}MHz conf={confidence} raw={msg}"
        )


if __name__ == "__main__":
    main()
