# Skill: add-setting

Add a new persistent setting to SettingsManager.

## Trigger

User says: "add a setting", "new setting", "add preference", "/add-setting [name]"

## Instructions

Ask the user for:
1. Setting name (e.g. `brightness`)
2. Type: `bool`, `uint8_t`, `uint16_t`, `int`, `String`
3. Default value
4. NVS key string (max 15 chars, e.g. `"brightness"`)

Then modify `lib/settings_manager.h`:

1. Add field to `cache` struct
2. Load from NVS in `begin()` using appropriate `prefs.get*()` method
3. Add getter (returns from cache)
4. Add setter (updates cache + writes to NVS via `prefs.begin/put/end`)
5. Add to `printAll()` for debug

## NVS Type Map

| C++ type | prefs getter | prefs setter |
|----------|-------------|-------------|
| `bool` | `getBool(key, default)` | `putBool(key, val)` |
| `uint8_t` | `getUChar(key, default)` | `putUChar(key, val)` |
| `uint16_t` | `getUShort(key, default)` | `putUShort(key, val)` |
| `int` | `getInt(key, default)` | `putInt(key, val)` |
| `String` | `getString(key, default)` | `putString(key, val)` |

## Pattern

```cpp
// In cache struct:
TypeName settingName;

// In begin():
cache.settingName = prefs.getType("nvs_key", defaultVal);

// Getter:
TypeName getSettingName() { return cache.settingName; }

// Setter:
void setSettingName(TypeName value) {
    cache.settingName = value;
    prefs.begin("settings", false);
    prefs.putType("nvs_key", value);
    prefs.end();
}
```

## Notes
- NVS namespace is `"settings"` (hardcoded in `begin()`)
- NVS keys max 15 chars
- `resetToDefaults()` calls `prefs.clear()` then `begin()` — new settings get their defaults automatically
