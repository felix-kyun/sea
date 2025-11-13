# Sea Panel Modules

The sea panel supports various modules to display system information. Modules are configured in your `~/.config/sea/panel.ini` file.

## Available Modules

### System Information Modules

#### cpu
Displays CPU usage percentage.

**Configuration:**
```ini
[cpu]
color = 'cyan'
background = 'default'
```

**Features:**
- Updates every 5 seconds
- Shows current CPU usage percentage
- Icon: 

---

#### cpu_temp
Displays CPU temperature.

**Configuration:**
```ini
[cpu_temp]
color = 'red'
background = 'default'
```

**Features:**
- Monitors CPU temperature from thermal sensors
- Icon: 

---

#### ram
Displays RAM usage.

**Configuration:**
```ini
[ram]
color = 'magenta'
background = 'default'
```

**Features:**
- Shows used RAM in GiB
- Updates every 5 seconds
- Icon:  

---

#### disk
Displays disk usage for the root filesystem.

**Configuration:**
```ini
[disk]
color = 'yellow'
background = 'default'
```

**Features:**
- Shows used/total disk space in GB
- Displays usage percentage
- Color changes based on usage:
  - Green: < 50%
  - Yellow: 50-75%
  - Magenta: 75-90%
  - Red: >= 90%
- Updates every 30 seconds
- Icon: 

**Example output:** ` 45G/120G (37%)`

---

#### uptime
Displays system uptime.

**Configuration:**
```ini
[uptime]
color = 'cyan'
background = 'default'
```

**Features:**
- Shows days, hours, and minutes
- Automatically adjusts format based on uptime:
  - Less than 1 hour: "Xm"
  - Less than 1 day: "Xh Ym"
  - 1 day or more: "Xd Yh Zm"
- Updates every 60 seconds
- Icon: 

**Example output:** ` 2d 5h 32m`

---

#### net_speed
Displays network upload and download speeds.

**Configuration:**
```ini
[net_speed]
color = 'blue'
background = 'default'
```

**Features:**
- Shows upload and download speeds in KiB/s
- Monitors all non-loopback interfaces
- Updates every 2 seconds
- Icons:  (upload),  (download)

---

### Audio & Display Modules

#### volume
Displays audio volume level with interactive controls.

**Configuration:**
```ini
[volume]
color = 'blue'
background = 'default'
```

**Features:**
- Shows current volume percentage
- Interactive controls:
  - **Scroll up**: Increase volume by 5%
  - **Scroll down**: Decrease volume by 5%
  - **Left click**: Toggle mute
- Dynamic icon based on volume level:
  -  (high, ≥70%)
  -  (medium, 30-69%)
  -  (low, <30%)
  - 󰝟 (muted)
- Updates every second
- Requires PulseAudio/PipeWire with `pactl`

**Example output:** ` 75%` or `󰝟 0%` (when muted)

---

#### brightness
Displays and controls screen brightness.

**Configuration:**
```ini
[brightness]
color = 'green'
background = 'default'
```

**Features:**
- Shows current brightness percentage
- Interactive controls:
  - **Scroll up**: Increase brightness
  - **Scroll down**: Decrease brightness
- Updates automatically when brightness changes
- Icon: 
- Requires membership in `video` group for control

---

### Date & Time Modules

#### date
Displays the current date.

**Configuration:**
```ini
[date]
color = 'white'
background = 'default'
```

**Features:**
- Shows date in configured format
- Updates daily
- Icon: 

---

#### time
Displays the current time.

**Configuration:**
```ini
[time]
color = 'white'
background = 'default'
```

**Features:**
- Shows time in configured format
- Updates every second
- Icon: 

---

### Power Modules

#### battery
Displays battery level and status.

**Configuration:**
```ini
[battery]
color = 'auto'
background = 'default'
```

**Features:**
- Shows battery percentage
- Dynamic icon based on charge level (10 different icons)
- Color changes based on battery level:
  - Green: >= 75%
  - Yellow: 50-74%
  - Magenta: 25-49%
  - Red: < 25%
- Shows charging icon when plugged in
- Updates every 60 seconds

---

### Media Module

#### media
Displays currently playing media.

**Configuration:**
```ini
[media]
color = 'white'
background = 'default'
```

**Features:**
- Shows currently playing media information
- Supports MPRIS-compatible players
- Icon: 

---

### Custom Modules

#### text
Displays custom static text.

**Configuration:**
```ini
[custom_module_name]
type = 'text'
content = 'Your custom text here'
color = 'white'
background = 'default'
```

**Features:**
- Display any custom text
- Useful for static labels or separators

---

## Panel Configuration

Configure your panel layout in `~/.config/sea/panel.ini`:

```ini
[panel]
left_modules = 'cpu,ram,disk'
center_modules = 'time,date'
right_modules = 'volume,battery,uptime'
module_spacing = 1
panel_padding = 0
auto_reset_style = true
log_file = '/tmp/panel.log'
log_to_stdout = false
```

## Module Position

Modules can be placed in three sections:
- **left_modules**: Left side of the panel
- **center_modules**: Center of the panel
- **right_modules**: Right side of the panel

List modules as comma-separated values in your desired order.

## Color Options

Available color values:
- `default`, `black`, `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `white`
- Some modules (like `battery`, `disk`) automatically adjust colors based on values

## Background Options

Background colors follow the same options as foreground colors. Use `default` for transparent background.
