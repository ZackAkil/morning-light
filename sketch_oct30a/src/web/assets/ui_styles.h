#pragma once

#include <avr/pgmspace.h>

static const char kUiStyles[] PROGMEM =
    "body{font-family:system-ui,sans-serif;max-width:340px;margin:48px auto;padding:0 16px;color:#111}"
    "h1{font-size:1.25rem;margin:0 0 16px}"
    ".sec{margin-top:24px;font-size:.8rem;text-transform:uppercase;letter-spacing:.05em;color:#666;padding-bottom:6px}"
    ".row{display:flex;align-items:center;justify-content:space-between;gap:8px;padding:10px 0;border-bottom:1px solid #ddd}"
    ".top{border-top:1px solid #ddd}"
    ".grp{display:flex;align-items:center;gap:6px;font-size:.9rem;color:#555}"
    "input[type=time],input[type=number]{font:inherit;padding:4px 6px;border:1px solid #ccc;border-radius:4px;color:#111;background:#fff}"
    "input[type=number]{width:48px;text-align:right}"
    "input[type=color]{width:36px;height:28px;padding:1px;border:1px solid #ccc;border-radius:4px;background:#fff;cursor:pointer}"
    "input[type=range]{width:110px;accent-color:#111}"
    ".pct{min-width:34px;text-align:right;font-variant-numeric:tabular-nums}"
    "button{font:inherit;padding:6px 14px;border:1px solid #111;background:#111;color:#fff;border-radius:4px;cursor:pointer}"
    "button.off{background:#fff;color:#111}"
    "button:disabled{opacity:.5;cursor:default}"
    ".actions{display:flex;justify-content:flex-end;padding:12px 0 4px}";
