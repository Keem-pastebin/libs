require "import"
import "res/init"
import "android.app.*"
import "android.os.*"
import "android.widget.*"
import "android.view.*"
import "android.net.*"
import "android.content.*"
import "android.graphics.*"
import "android.graphics.drawable.*"
import "android.provider.*"
import "android.net.Uri"
import "android.text.style.ForegroundColorSpan"
import "android.text.Spannable"
import "android.text.SpannableString"
import "android.graphics.Typeface"
import "android.graphics.PorterDuff"
import "android.graphics.PorterDuffColorFilter"
import "android.graphics.PixelFormat"
import "android.view.Gravity"
import "android.view.MotionEvent"
import "android.view.WindowManager"
import "android.app.AlertDialog"
import "android.animation.ObjectAnimator"
import "android.animation.ArgbEvaluator"
import "android.animation.ValueAnimator"
import "android.graphics.Color"
import "android.view.animation.AlphaAnimation"
import "android.view.animation.TranslateAnimation"
import "java.io.File"
import "android.content.Intent"
import "android.graphics.drawable.GradientDrawable"
import "android.graphics.drawable.ColorDrawable"
import "android.provider.Settings"
import "com.downloader.*"
import "AndLua"
import "android.speech.tts.TextToSpeech"
import "android.widget.CompoundButton"
import "android.widget.RadioGroup"
import "android.widget.RadioButton"
import "android.content.Context"
import "java.io.FileInputStream"
import "java.io.FileOutputStream"
import "java.io.ObjectInputStream"
import "java.io.ObjectOutputStream"
import "java.io.Serializable"
import "java.util.HashMap"
import "java.util.Map"
import "android.text.InputType"
import "android.text.method.PasswordTransformationMethod"
import "android.text.method.HideReturnsTransformationMethod"
import "android.view.inputmethod.EditorInfo"

local cjson = require "cjson"

-- ============================================================
-- LOGIN SYSTEM (ONLINE PASTEBIN)
-- ============================================================

LoginManager = {}
LoginManager.CONFIG_FILE = activity.getLuaDir() .. "/login_data.dat"
LoginManager.PASTEBIN_URL = "https://pastebin.com/raw/hjkBcgKg"
LoginManager.KEYS = nil
LoginManager.IS_LOADING = false
LoginManager.SAVED_KEY = nil

function LoginManager.loadKeysFromPastebin(callback)
    if LoginManager.KEYS then
        if callback then callback(true) end
        return true
    end
    
    if LoginManager.IS_LOADING then
        return false
    end
    
    LoginManager.IS_LOADING = true
    
    Http.get(LoginManager.PASTEBIN_URL, function(code, body)
        LoginManager.IS_LOADING = false
        
        if code ~= 200 then
            idkcstmToast("Error: Cannot connect to server!")
            if callback then callback(false) end
            return
        end
        
        local success, result = pcall(cjson.decode, body)
        
        if not success or not result or not result.keys then
            idkcstmToast("Error: Invalid keys format from server!")
            if callback then callback(false) end
            return
        end
        
        LoginManager.KEYS = result.keys
        idkcstmToast("Keys loaded from server successfully!")
        if callback then callback(true) end
        return true
    end)
    
    return false
end

function LoginManager.isKeyValid(password, deviceId)
    if not LoginManager.KEYS then
        idkcstmToast("Please wait, loading keys from server...")
        return false
    end
    
    local now = os.time()
    for _, key in ipairs(LoginManager.KEYS) do
        if key.password == password then
            if key.blocked then
                idkcstmToast("This key is blocked!")
                return false
            end
            if key.mode == "single" then
                if key.device ~= deviceId then
                    idkcstmToast("This key is already used on another device!")
                    return false
                end
            end
            if key.expiry then
                local year, month, day = key.expiry:match("(%d+)/(%d+)/(%d+)")
                if year and month and day then
                    local expiryTime = os.time({year=tonumber(year), month=tonumber(month), day=tonumber(day), hour=23, min=59, sec=59})
                    if now > expiryTime then
                        idkcstmToast("This key has expired!")
                        return false
                    end
                end
            end
            return true
        end
    end
    idkcstmToast("Invalid password!")
    return false
end

function LoginManager.saveLoginState(password)
    LoginManager.SAVED_KEY = password
    local file = io.open(LoginManager.CONFIG_FILE, "w")
    if file then
        file:write(password)
        file:close()
        return true
    end
    return false
end

function LoginManager.loadLoginState()
    if LoginManager.SAVED_KEY then
        return LoginManager.SAVED_KEY
    end
    local file = io.open(LoginManager.CONFIG_FILE, "r")
    if not file then return nil end
    local content = file:read("*all")
    file:close()
    LoginManager.SAVED_KEY = content
    return content
end

function LoginManager.clearLoginState()
    LoginManager.SAVED_KEY = nil
    os.remove(LoginManager.CONFIG_FILE)
end

function LoginManager.getDeviceId()
    return Settings.Secure.getString(activity.getContentResolver(), Settings.Secure.ANDROID_ID)
end

-- ============================================================
-- CUSTOM TOAST
-- ============================================================

local toastLayout = {
  CardView,
  layout_width = "wrap_content",
  backgroundColor = "0xFF720000",
  radius = "10dp",
  padding = "10dp",
  CardElevation = "9dp",
  {
    LinearLayout,
    padding = "8dp",
    gravity = "center",
    {
      ImageView,
      src = "icon.png",
      layout_width = "10%w",
      layout_marginRight = "2%w",
      layout_height = "4%h",
    },
    {
      TextView,
      id = "msg",
      text = "Toast",
      textColor = "0xFFFFFFFF",
      textSize = "16sp",
    },
  },
}

function SansFont(view, fontPath)
  view.setTypeface(Typeface.createFromFile(File(fontPath)))
end

function FontN(view, fontPath)
  view.setTypeface(Typeface.createFromFile(File(fontPath)))
end

function idkcstmToast(text)
  local t = Toast.makeText(activity, text, Toast.LENGTH_SHORT)
  t.setView(loadlayout(toastLayout))
  t.show()
  toast = t
  pcall(SansFont, msg, activity.getLuaDir() .. "/font/doom.ttf")
  msg.setText(text)
end

-- ============================================================
-- UI HELPER FUNCTIONS
-- ============================================================

function CircleButtonAsh1(view, color1, color2, radius, strokeColor)
  import "android.graphics.drawable.GradientDrawable"
  drawable = GradientDrawable(GradientDrawable.Orientation.BOTTOM_TOP, { color1, color2, color1 })
  drawable.setShape(GradientDrawable.RECTANGLE)
  drawable.setCornerRadii({ radius, radius, radius, radius, radius, radius, radius, radius })
  drawable.setStroke(2.1, strokeColor)
  view.setBackgroundDrawable(drawable)
end

function getDeviceID()
  return Settings.Secure.getString(activity.getContentResolver(), Settings.Secure.ANDROID_ID)
end


-- ============================================================
-- LOGIN UI (COMPACT, NO AUTO-LOGIN, HIDDEN PASSWORD, PASTE KEY)
-- ============================================================

function LoginUI()
    local deviceId = LoginManager.getDeviceId()
    local savedPassword = LoginManager.loadLoginState()
    
    -- Create gradient background
    local grad = GradientDrawable(GradientDrawable.Orientation.BOTTOM_TOP, { 0xff720000, 0xff000000, 0xff720000 })
    
    local loginLayout = {
        LinearLayout,
        orientation = "vertical",
        layout_width = "fill",
        layout_height = "fill",
        id = "loginbg",
        gravity = "center_horizontal",  -- Changed from "center"
        background = grad,
        padding = "20dp",
        {
            LinearLayout,
            orientation = "vertical",
            layout_width = "fill",
            layout_height = "wrap",
            gravity = "center",
            layout_marginTop = "10%h",  -- Added margin to push content upward relative to top
            {
                ImageView,
                layout_width = "80dp",
                layout_height = "80dp",
                src = "logo.png",
                layout_marginBottom = "5dp",
            },
            {
                TextView,
                text = "HexPhantom",
                textSize = "24sp",
                textColor = "0xFFFF99CC",
                layout_gravity = "center",
                layout_marginBottom = "2dp",
            },
            {
                TextView,
                text = "Enter Your Key",
                textSize = "14sp",
                textColor = "0x88FFFFFF",
                layout_gravity = "center",
                layout_marginBottom = "12dp",
            },
            {
                LinearLayout,
                orientation = "horizontal",
                layout_width = "80%w",
                layout_height = "45dp",
                layout_marginBottom = "8dp",
                {
                    EditText,
                    layout_width = "0dp",
                    layout_height = "fill",
                    id = "login_password",
                    hint = "Password",
                    hintColor = "0x66FFFFFF",
                    textColor = "0xFFFFFFFF",
                    background = "0x33FFFFFF",
                    padding = "12dp",
                    layout_weight = "1",
                    inputType = InputType.TYPE_CLASS_TEXT or InputType.TYPE_TEXT_VARIATION_PASSWORD,
                    cursorVisible = true,
                    textSize = "14sp",
                },
                {
                    Button,
                    layout_width = "50dp",
                    layout_height = "fill",
                    id = "paste_key_btn",
                    text = "📋",
                    textColor = "0xFFFFFFFF",
                    background = "0xFF0066CC",
                    textSize = "16sp",
                    layout_marginLeft = "5dp",
                },
            },
            -- Show Password Checkbox
            {
                CheckBox,
                layout_width = "wrap",
                layout_height = "wrap",
                id = "show_password",
                text = "Show Password",
                textColor = "0x88FFFFFF",
                textSize = "12sp",
                layout_gravity = "center",
                layout_marginBottom = "10dp",
            },
            {
                LinearLayout,
                orientation = "horizontal",
                layout_width = "fill",
                layout_height = "wrap",
                gravity = "center",
                {
                    Button,
                    layout_width = "30%w",
                    layout_height = "42dp",
                    id = "login_btn",
                    text = "LOGIN",
                    textColor = "0xFFFFFFFF",
                    background = "0xFF00E000",
                    layout_marginRight = "6dp",
                    textSize = "14sp",
                },
                {
                    Button,
                    layout_width = "30%w",
                    layout_height = "42dp",
                    id = "save_key_btn",
                    text = "SAVE",
                    textColor = "0xFFFFFFFF",
                    background = "0xFF0066CC",
                    layout_marginRight = "6dp",
                    textSize = "12sp",
                },
                {
                    Button,
                    layout_width = "30%w",
                    layout_height = "42dp",
                    id = "exit_btn",
                    text = "EXIT",
                    textColor = "0xFFFFFFFF",
                    background = "0xCCFF0000",
                    textSize = "14sp",
                },
            },
            {
                LinearLayout,
                orientation = "horizontal",
                layout_width = "fill",
                layout_height = "wrap",
                gravity = "center",
                layout_marginTop = "8dp",
                {
                    Button,
                    layout_width = "40%w",
                    layout_height = "35dp",
                    id = "clear_saved_btn",
                    text = "CLEAR SAVED",
                    textColor = "0xFFFFFFFF",
                    background = "0xCCFF6600",
                    textSize = "11sp",
                },
            },
            {
                TextView,
                layout_width = "wrap",
                id = "login_status",
                text = "",
                textColor = "0xFFFF6B6B",
                textSize = "12sp",
                layout_gravity = "center",
                layout_marginTop = "8dp",
            },
            -- Device ID Section (more compact)
            {
                LinearLayout,
                layout_width = "80%w",
                layout_height = "wrap",
                orientation = "vertical",
                layout_marginTop = "10dp",
                background = "0x1AFFFFFF",
                padding = "8dp",
                {
                    TextView,
                    text = "Device ID",
                    textSize = "10sp",
                    textColor = "0x88FFFFFF",
                    layout_gravity = "center",
                    layout_marginBottom = "3dp",
                },
                {
                    LinearLayout,
                    orientation = "horizontal",
                    layout_width = "fill",
                    layout_height = "wrap",
                    {
                        TextView,
                        layout_width = "0dp",
                        layout_height = "wrap",
                        id = "device_id_display",
                        text = deviceId or "N/A",
                        textColor = "0xFF00FF00",
                        textSize = "10sp",
                        layout_weight = "1",
                        layout_marginRight = "5dp",
                        ellipsize = "marquee",
                        singleLine = true,
                        marqueeRepeatLimit = "marquee_forever",
                        focusable = true,
                        focusableInTouchMode = true,
                    },
                    {
                        Button,
                        layout_width = "50dp",
                        layout_height = "28dp",
                        id = "copy_device_id",
                        text = "COPY",
                        textColor = "0xFFFFFFFF",
                        background = "0xFF0066CC",
                        textSize = "9sp",
                        layout_gravity = "center",
                    },
                },
            },
            {
                TextView,
                text = "Loading keys from server...",
                textSize = "10sp",
                textColor = "0x66FFFFFF",
                layout_gravity = "center",
                layout_marginTop = "8dp",
                id = "loading_status",
                visibility = "gone",
            },
        },
    }
    
    activity.setContentView(loadlayout(loginLayout))
    
    -- 🔥 FIX: Force password to be hidden using transformation method
    login_password.setTransformationMethod(PasswordTransformationMethod.getInstance())
    login_password.setInputType(InputType.TYPE_CLASS_TEXT or InputType.TYPE_TEXT_VARIATION_PASSWORD)
    
    -- 🔥 No auto-login
    if savedPassword then
        login_password.setText(savedPassword)
        login_status.setText("Saved key loaded. Click LOGIN to continue.")
        login_status.setTextColor(0xFFFFFF00)
    end
    
    -- Load keys from pastebin on startup
    login_status.setText("Loading keys from server...")
    login_status.setTextColor(0xFFFFFF00)
    
    LoginManager.loadKeysFromPastebin(function(success)
        if success then
            login_status.setText("Keys loaded! Enter your password.")
            login_status.setTextColor(0xFF00FF00)
        else
            login_status.setText("Failed to load keys. Check connection.")
            login_status.setTextColor(0xFFFF0000)
        end
    end)
    
    -- Show/Hide Password
    show_password.setOnCheckedChangeListener(function(_, checked)
        if checked then
            login_password.setTransformationMethod(HideReturnsTransformationMethod.getInstance())
        else
            login_password.setTransformationMethod(PasswordTransformationMethod.getInstance())
        end
        login_password.setSelection(login_password.getText().length())
    end)
    
    -- 📋 PASTE KEY FUNCTIONALITY
    paste_key_btn.onClick = function()
        local clipboard = activity.getSystemService(Context.CLIPBOARD_SERVICE)
        if clipboard then
            local clip = clipboard.getPrimaryClip()
            if clip and clip.getItemCount() > 0 then
                local pastedText = clip.getItemAt(0).getText().toString()
                if pastedText and pastedText ~= "" then
                    login_password.setText(pastedText)
                    login_password.setSelection(pastedText:length())
                    -- Keep password hidden even after paste
                    login_password.setTransformationMethod(PasswordTransformationMethod.getInstance())
                    login_status.setText("Key pasted successfully!")
                    login_status.setTextColor(0xFF00FF00)
                    idkcstmToast("Key pasted!")
                else
                    login_status.setText("Clipboard is empty!")
                    login_status.setTextColor(0xFFFF6B6B)
                end
            else
                login_status.setText("Clipboard is empty!")
                login_status.setTextColor(0xFFFF6B6B)
            end
        end
    end
    
    local function attemptLogin()
        local password = login_password.getText().toString()
        if password == "" then
            login_status.setText("Please enter a password!")
            login_status.setTextColor(0xFFFF6B6B)
            return
        end
        
        if not LoginManager.KEYS then
            login_status.setText("Loading keys from server, please wait...")
            login_status.setTextColor(0xFFFFFF00)
            return
        end
        
        if LoginManager.isKeyValid(password, deviceId) then
            LoginManager.saveLoginState(password)
            login_status.setTextColor(0xFF00FF00)
            login_status.setText("Login successful! Loading...")
            task(500, function()
                LoaderLY()
            end)
        else
            login_status.setText("Invalid password or key blocked/expired!")
            login_status.setTextColor(0xFFFF6B6B)
        end
    end
    
    login_btn.onClick = attemptLogin
    
    -- Save Key Button
    save_key_btn.onClick = function()
        local password = login_password.getText().toString()
        if password == "" then
            login_status.setText("Please enter a password to save!")
            login_status.setTextColor(0xFFFF6B6B)
            return
        end
        LoginManager.saveLoginState(password)
        login_status.setTextColor(0xFF00FF00)
        login_status.setText("Key saved successfully!")
        idkcstmToast("Key saved successfully!")
    end
    
    -- Clear Saved Key Button
    clear_saved_btn.onClick = function()
        LoginManager.clearLoginState()
        login_password.setText("")
        login_status.setTextColor(0xFFFF6B6B)
        login_status.setText("Saved key cleared!")
        idkcstmToast("Saved key cleared!")
    end
    
    -- Enter key on keyboard triggers login
    login_password.setOnEditorActionListener({
        onEditorAction = function(v, actionId, event)
            if actionId == EditorInfo.IME_ACTION_DONE or actionId == EditorInfo.IME_ACTION_GO then
                attemptLogin()
                return true
            end
            return false
        end
    })
    
    exit_btn.onClick = function()
        os.exit()
    end
    
    -- Copy Device ID
    copy_device_id.onClick = function()
        local clipboard = activity.getSystemService(Context.CLIPBOARD_SERVICE)
        local clip = ClipData.newPlainText("Device ID", deviceId)
        clipboard.setPrimaryClip(clip)
        idkcstmToast("Device ID Copied!")
    end
    
    device_id_display.setSelected(true)
    
    -- Apply fonts
    local fontPath = activity.getLuaDir() .. "/font/doom.ttf"
    pcall(FontN, login_password, fontPath)
    pcall(FontN, login_btn, fontPath)
    pcall(FontN, save_key_btn, fontPath)
    pcall(FontN, clear_saved_btn, fontPath)
    pcall(FontN, exit_btn, fontPath)
    pcall(FontN, device_id_display, fontPath)
    pcall(FontN, copy_device_id, fontPath)
    pcall(FontN, show_password, fontPath)
    pcall(FontN, paste_key_btn, fontPath)
    
    -- Style buttons
    CircleButtonAsh1(login_btn, 0xff00E000, 0xff000000, 10, 0xff00E000)
    CircleButtonAsh1(save_key_btn, 0xff0066cc, 0xff000000, 10, 0xff0066cc)
    CircleButtonAsh1(clear_saved_btn, 0xCCFF6600, 0xff000000, 10, 0xCCFF6600)
    CircleButtonAsh1(exit_btn, 0xCCFF0000, 0xff000000, 10, 0xCCFF0000)
    CircleButtonAsh1(copy_device_id, 0xff0066cc, 0xff000000, 10, 0xff0066cc)
    CircleButtonAsh1(paste_key_btn, 0xff0066cc, 0xff000000, 10, 0xff0066cc)
    
    -- Style checkbox
    pcall(function()
        show_password.ButtonDrawable.setColorFilter(PorterDuffColorFilter(0xff830000, PorterDuff.Mode.SRC_ATOP))
    end)
    
    return false
end
-- ============================================================
-- CONFIGURATION MANAGER
-- ============================================================

ConfigManager = {}
ConfigManager.CONFIG_FILE = activity.getLuaDir() .. "/config.dat"

function ConfigManager.saveConfig()
    local config = {}
    
    for i = 1, 16 do
        local cb = _G["dg" .. i]
        if cb then
            config["dg" .. i] = cb.isChecked()
        end
    end
    
    local bypassToggles = {"logo", "clearlogs", "fps", "skip"}
    for _, id in ipairs(bypassToggles) do
        local view = _G[id]
        if view then
            config[id] = view.isChecked()
        end
    end
    
    if aimbot_seekbar then
        config["aimbot_progress"] = aimbot_seekbar.getProgress()
    end
    if snowboard_seekbar then
        config["snowboard_progress"] = snowboard_seekbar.getProgress()
    end
    
    local antennaToggles = {"antenna", "antenna","antenna2"}
    for _, id in ipairs(antennaToggles) do
        local view = _G[id]
        if view then
            config[id] = view.isChecked()
        end
    end
    
    local charSkins = {"kuiji", "lazarus", "shepherd", "sophia", "spectre", "templar", 
                       "siren", "ghost", "chunli", "ryu", "cammy", "akuma", 
                       "blacknoir", "starlight", "homelander"}
    for _, id in ipairs(charSkins) do
        local view = _G[id]
        if view then
            config[id] = view.isChecked()
        end
    end
    
    local mythicWeapons = {"ak117lava", "ak117", "bp50", "ffar", "grau", "krig6", 
                          "type19", "oden", "xm4", "ak47", "lw3", "dlq33", "vmp", 
                          "uss9", "kilo", "switchh", "jak12", "cx9", "qq9", "mg42", 
                          "m13", "fennec", "rytec", "holger", "em2", "cbr", "asval", 
                          "peace", "ram7", "type25", "so14", "lachmann", "dp27"}
    for _, id in ipairs(mythicWeapons) do
        local view = _G[id]
        if view then
            config[id] = view.isChecked()
        end
    end
    
    local legendaryWeapons = {"krm", "krmred", "krmload", "locus", "locusdemon", 
                             "locusmetal", "locusneptune", "locuscomet", "locusicy", 
                             "by15", "hssong", "dlqholi", "dlqzealot"}
    for _, id in ipairs(legendaryWeapons) do
        local view = _G[id]
        if view then
            config[id] = view.isChecked()
        end
    end
    
    local meleeWeapons = {"tang", "longq", "spear", "scissors", "tomahawk", "saber", "fiery"}
    for _, id in ipairs(meleeWeapons) do
        local view = _G[id]
        if view then
            config[id] = view.isChecked()
        end
    end
    
    local vehicles = {"sand", "blaze", "mech", "parachute"}
    for _, id in ipairs(vehicles) do
        local view = _G[id]
        if view then
            config[id] = view.isChecked()
        end
    end
    
    local camos = {"diamond", "redsprite", "glacial", "aether", "alchemy", "fluorescent",
                  "lunar", "sunken", "ocean", "psychic", "shimmer", "astronomy",
                  "powernova", "makingwaves", "blistering", "emerald", "assault", "scorch"}
    for _, id in ipairs(camos) do
        local view = _G[id]
        if view then
            config[id] = view.isChecked()
        end
    end
    
    local menuToggles = {"showmenu", "characmenu", "mythicmenu", "legendarymenu", 
                        "meleemenu", "vehiclemenu", "camomenu"}
    for _, id in ipairs(menuToggles) do
        local view = _G[id]
        if view then
            config[id] = view.isChecked()
        end
    end
    
    local success, err = pcall(function()
        local file = io.open(ConfigManager.CONFIG_FILE, "w")
        if file then
            file:write("return " .. table.serialize(config))
            file:close()
        end
    end)
    
    if success then
        idkcstmToast("Config Saved Successfully")
    else
        idkcstmToast("Error Saving Config")
    end
end

function ConfigManager.loadConfig()
    local file = io.open(ConfigManager.CONFIG_FILE, "r")
    if not file then
        idkcstmToast("No Config Found")
        return
    end
    
    local content = file:read("*all")
    file:close()
    
    local config = loadstring(content)()
    if not config then
        idkcstmToast("Error Loading Config")
        return
    end
    
    for i = 1, 16 do
        local cb = _G["dg" .. i]
        if cb and config["dg" .. i] ~= nil then
            cb.setChecked(config["dg" .. i])
        end
    end
    
    local bypassToggles = {"logo", "clearlogs", "fps", "skip"}
    for _, id in ipairs(bypassToggles) do
        local view = _G[id]
        if view and config[id] ~= nil then
            view.setChecked(config[id])
        end
    end
    
    if aimbot_seekbar and config["aimbot_progress"] then
        aimbot_seekbar.setProgress(config["aimbot_progress"])
        aimbot_text.setText("Adjustable Aim (" .. config["aimbot_progress"] .. "%)")
    end
    if snowboard_seekbar and config["snowboard_progress"] then
        snowboard_seekbar.setProgress(config["snowboard_progress"])
        snowboard_text.setText("Adjustable SnowB. (" .. config["snowboard_progress"] .. "%)")
    end
    
    local antennaToggles = {"antenna", "antenna1", "antenna2"}
    for _, id in ipairs(antennaToggles) do
        local view = _G[id]
        if view and config[id] ~= nil then
            view.setChecked(config[id])
        end
    end
    
    local charSkins = {"kuiji", "lazarus", "shepherd", "sophia", "spectre", "templar", 
                       "siren", "ghost", "chunli", "ryu", "cammy", "akuma", 
                       "blacknoir", "starlight", "homelander"}
    for _, id in ipairs(charSkins) do
        local view = _G[id]
        if view and config[id] ~= nil then
            view.setChecked(config[id])
        end
    end
    
    local mythicWeapons = {"ak117lava", "ak117", "bp50", "ffar", "grau", "krig6", 
                          "type19", "oden", "xm4", "ak47", "lw3", "dlq33", "vmp", 
                          "uss9", "kilo", "switchh", "jak12", "cx9", "qq9", "mg42", 
                          "m13", "fennec", "rytec", "holger", "em2", "cbr", "asval", 
                          "peace", "ram7", "type25", "so14", "lachmann", "dp27"}
    for _, id in ipairs(mythicWeapons) do
        local view = _G[id]
        if view and config[id] ~= nil then
            view.setChecked(config[id])
        end
    end
    
    local legendaryWeapons = {"krm", "krmred", "krmload", "locus", "locusdemon", 
                             "locusmetal", "locusneptune", "locuscomet", "locusicy", 
                             "by15", "hssong", "dlqholi", "dlqzealot"}
    for _, id in ipairs(legendaryWeapons) do
        local view = _G[id]
        if view and config[id] ~= nil then
            view.setChecked(config[id])
        end
    end
    
    local meleeWeapons = {"tang", "longq", "spear", "scissors", "tomahawk", "saber", "fiery"}
    for _, id in ipairs(meleeWeapons) do
        local view = _G[id]
        if view and config[id] ~= nil then
            view.setChecked(config[id])
        end
    end
    
    local vehicles = {"sand", "blaze", "mech", "parachute"}
    for _, id in ipairs(vehicles) do
        local view = _G[id]
        if view and config[id] ~= nil then
            view.setChecked(config[id])
        end
    end
    
    local camos = {"diamond", "redsprite", "glacial", "aether", "alchemy", "fluorescent",
                  "lunar", "sunken", "ocean", "psychic", "shimmer", "astronomy",
                  "powernova", "makingwaves", "blistering", "emerald", "assault", "scorch"}
    for _, id in ipairs(camos) do
        local view = _G[id]
        if view and config[id] ~= nil then
            view.setChecked(config[id])
        end
    end
    
    local menuToggles = {"showmenu", "characmenu", "mythicmenu", "legendarymenu", 
                        "meleemenu", "vehiclemenu", "camomenu"}
    for _, id in ipairs(menuToggles) do
        local view = _G[id]
        if view and config[id] ~= nil then
            view.setChecked(config[id])
        end
    end
    
    idkcstmToast("Config Loaded Successfully")
end

function ConfigManager.resetConfig()
    for i = 1, 16 do
        local cb = _G["dg" .. i]
        if cb then
            cb.setChecked(false)
        end
    end
    
    local bypassToggles = {"logo", "clearlogs", "fps", "skip"}
    for _, id in ipairs(bypassToggles) do
        local view = _G[id]
        if view then
            view.setChecked(false)
        end
    end
    
    if aimbot_seekbar then
        aimbot_seekbar.setProgress(0)
        aimbot_text.setText("Adjustable Aim (0%)")
    end
    if snowboard_seekbar then
        snowboard_seekbar.setProgress(0)
        snowboard_text.setText("Adjustable SnowB. (0%)")
    end
    
    local antennaToggles = {"antenna", "antenna1", "antenna2"}
    for _, id in ipairs(antennaToggles) do
        local view = _G[id]
        if view then
            view.setChecked(false)
        end
    end
    
    local charSkins = {"kuiji", "lazarus", "shepherd", "sophia", "spectre", "templar", 
                       "siren", "ghost", "chunli", "ryu", "cammy", "akuma", 
                       "blacknoir", "starlight", "homelander"}
    for _, id in ipairs(charSkins) do
        local view = _G[id]
        if view then
            view.setChecked(false)
        end
    end
    
    local mythicWeapons = {"ak117lava", "ak117", "bp50", "ffar", "grau", "krig6", 
                          "type19", "oden", "xm4", "ak47", "lw3", "dlq33", "vmp", 
                          "uss9", "kilo", "switchh", "jak12", "cx9", "qq9", "mg42", 
                          "m13", "fennec", "rytec", "holger", "em2", "cbr", "asval", 
                          "peace", "ram7", "type25", "so14", "lachmann", "dp27"}
    for _, id in ipairs(mythicWeapons) do
        local view = _G[id]
        if view then
            view.setChecked(false)
        end
    end
    
    local legendaryWeapons = {"krm", "krmred", "krmload", "locus", "locusdemon", 
                             "locusmetal", "locusneptune", "locuscomet", "locusicy", 
                             "by15", "hssong", "dlqholi", "dlqzealot"}
    for _, id in ipairs(legendaryWeapons) do
        local view = _G[id]
        if view then
            view.setChecked(false)
        end
    end
    
    local meleeWeapons = {"tang", "longq", "spear", "scissors", "tomahawk", "saber", "fiery"}
    for _, id in ipairs(meleeWeapons) do
        local view = _G[id]
        if view then
            view.setChecked(false)
        end
    end
    
    local vehicles = {"sand", "blaze", "mech", "parachute"}
    for _, id in ipairs(vehicles) do
        local view = _G[id]
        if view then
            view.setChecked(false)
        end
    end
    
    local camos = {"diamond", "redsprite", "glacial", "aether", "alchemy", "fluorescent",
                  "lunar", "sunken", "ocean", "psychic", "shimmer", "astronomy",
                  "powernova", "makingwaves", "blistering", "emerald", "assault", "scorch"}
    for _, id in ipairs(camos) do
        local view = _G[id]
        if view then
            view.setChecked(false)
        end
    end
    
    local menuToggles = {"showmenu", "characmenu", "mythicmenu", "legendarymenu", 
                        "meleemenu", "vehiclemenu", "camomenu"}
    for _, id in ipairs(menuToggles) do
        local view = _G[id]
        if view then
            view.setChecked(false)
        end
    end
    
    pcall(function() os.remove(ConfigManager.CONFIG_FILE) end)
    
    idkcstmToast("Config Reset Successfully")
end

function table.serialize(tbl)
    local function serializeValue(val)
        if type(val) == "boolean" then
            return val and "true" or "false"
        elseif type(val) == "number" then
            return tostring(val)
        elseif type(val) == "string" then
            return string.format("%q", val)
        elseif type(val) == "table" then
            return table.serialize(val)
        else
            return "nil"
        end
    end
    
    local parts = {}
    for k, v in pairs(tbl) do
        table.insert(parts, "[" .. serializeValue(k) .. "]=" .. serializeValue(v))
    end
    return "{" .. table.concat(parts, ",") .. "}"
end

-- ============================================================
-- UI HELPER FUNCTIONS
-- ============================================================

function Waterdropanimation(view, duration)
  local anim = ObjectAnimator().ofFloat(view, "scaleX", { 0, 0, 0, 0.99999, 1 })
  anim.setDuration(duration).start()
end

function CircleButtonAsh(view, color1, color2, radius, strokeColor)
  import "android.graphics.drawable.GradientDrawable"
  drawable = GradientDrawable(GradientDrawable.Orientation.LEFT_RIGHT, { color1, color2 })
  drawable.setShape(GradientDrawable.RECTANGLE)
  drawable.setCornerRadii({ radius, radius, radius, radius, radius, radius, radius, radius })
  drawable.setStroke(2.1, strokeColor)
  view.setBackgroundDrawable(drawable)
end

function CircleButtonAsh1(view, color1, color2, radius, strokeColor)
  import "android.graphics.drawable.GradientDrawable"
  drawable = GradientDrawable(GradientDrawable.Orientation.BOTTOM_TOP, { color1, color2, color1 })
  drawable.setShape(GradientDrawable.RECTANGLE)
  drawable.setCornerRadii({ radius, radius, radius, radius, radius, radius, radius, radius })
  drawable.setStroke(2.1, strokeColor)
  view.setBackgroundDrawable(drawable)
end

-- ============================================================
-- PROCESS / MEMORY HELPERS
-- ============================================================

local Mem = {}

function Mem.MemoryPatch(lib, offset, hex)
  local pid = getProcessId("com.garena.game.codm")
  if not pid then
    idkcstmToast("Error: Cannot find game process")
    return
  end
  local base
  for line in io.lines("/proc/" .. pid .. "/maps") do
    if line:find(lib) then
      local addr = line:match("^(%x+)-")
      base = tonumber(addr, 16)
      break
    end
  end
  if not base then
    idkcstmToast("Error: Cannot find library base")
    return
  end
  local mem = io.open("/proc/" .. pid .. "/mem", "r+b")
  if not mem then
    idkcstmToast("Error: Cannot open process memory")
    return
  end
  mem:seek("set", base + offset)
  local bytes = {}
  for b in hex:gmatch("%x%x") do
    table.insert(bytes, string.char(tonumber(b, 16)))
  end
  mem:write(table.concat(bytes))
  mem:close()
end

function isRootAvailable()
  local p = io.popen("su -c 'echo root'")
  if not p then
    return false
  end
  local out = p:read("*a")
  p:close()
  return out:find("root") ~= nil
end

function floatToHexLE(f)
  local neg = f < 0
  if neg then f = -f end
  if f == 0 then return "00 00 00 00" end
  if f == math.huge then return "00 00 80 7F" end
  if f ~= f then return "00 00 C0 7F" end
  local m, e = math.frexp(f)
  local bits = (neg and 0x80000000 or 0) | ((e + 126) << 23) | ((m * 2 - 1) * 0x800000)
  local s = string.format("%08X", bits)
  return s:sub(7, 8) .. " " .. s:sub(5, 6) .. " " .. s:sub(3, 4) .. " " .. s:sub(1, 2)
end

function cppPatch(lib, arg)
  local path = activity.getLuaDir("lib/arm64-v8a/" .. lib)
  os.execute("chmod 777 " .. path)
  Runtime.getRuntime().exec(path .. " " .. arg .. " 2 3 4")
end

function antihook()
  local patterns = {"%[.+%]", "n0n3m4", "droidc", "busybox"}
  for _, pat in ipairs(patterns) do
    local pids = {}
    local ps = io.popen("ps -e")
    for line in ps:lines() do
      local pid, name = line:match("^(%S+)%s+%S+%s+%S+%s+%S+%s+(.+)")
      if name and name:find(pat) then
        table.insert(pids, pid)
      end
    end
    ps:close()
    for _, pid in ipairs(pids) do
      os.execute("kill -9 " .. pid)
    end
  end
end

function getProcessId(name)
  local ps = io.popen("pgrep -f " .. name)
  if not ps then return nil end
  local pid = ps:read("*a"):match("%d+")
  ps:close()
  return pid
end

-- ============================================================
-- LOADER UI
-- ============================================================

function LoaderLY()

  local loaderLayout = {
    LinearLayout,
    orientation = "vertical",
    layout_width = "fill",
    layout_height = "fill",
    id = "mainbg",
    gravity = "start",
    {
      LinearLayout,
      layout_margin = "5dp",
      layout_marginTop = "-70dp",
      layout_marginBottom = "30dp",
    },
    {
      ImageView,
      layout_width = "70%w",
      src = "font/codm.png",
      layout_gravity = "center",
      id = "NOWI",
      layout_height = "300",
      layout_marginTop = "66dp",
    },
    {
      LinearLayout,
      layout_width = "30dp",
      layout_height = "5%h",
    },
    {
      LinearLayout,
      layout_width = "fill",
      gravity = "center",
      layout_height = "wrap",
      orientation = "horizontal",
      {
        CardView,
        layout_width = "45%w",
        id = "start",
        radius = "10dp",
        layout_gravity = "center",
        layout_height = "11%h",
        backgroundColor = "0xFF00E000",
        {
          LinearLayout,
          layout_width = "fill",
          gravity = "center",
          layout_height = "fill",
          orientation = "vertical",
          {
            ImageView,
            layout_width = "50dp",
            src = "font/play.png",
            layout_height = "50dp",
            layout_weight = "2",
          },
          {
            TextView,
            layout_width = "wrap",
            id = "txtStartCheat",
            textSize = "16sp",
            textColor = "0xFFFFFFFF",
            text = "Start Hack ",
            layout_height = "wrap",
            layout_marginBottom = "10dp",
          },
        },
      },
      {
        LinearLayout,
        layout_width = "20dp",
        layout_height = "1%h",
      },
      {
        CardView,
        layout_width = "45%w",
        id = "exit",
        radius = "10dp",
        layout_gravity = "center",
        layout_height = "11%h",
        backgroundColor = "0xFF00E000",
        {
          LinearLayout,
          layout_width = "fill",
          gravity = "center",
          layout_height = "fill",
          orientation = "vertical",
          {
            ImageView,
            layout_width = "50dp",
            src = "font/stop.png",
            layout_height = "50dp",
            layout_weight = "2",
          },
          {
            TextView,
            layout_width = "wrap",
            id = "txtStopCheat",
            textSize = "16sp",
            textColor = "0xFFFFFFFF",
            text = "Stop Hack",
            layout_height = "wrap",
            layout_marginBottom = "10dp",
          },
        },
      },
    },
    {
      LinearLayout,
      layout_width = "40dp",
      layout_height = "1%h",
    },
    {
      CardView,
      layout_width = "350dp",
      id = "abcdefg",
      radius = "10dp",
      layout_gravity = "center",
      layout_height = "wrap",
      BackgroundColor = "0x52000000",
      {
        LinearLayout,
        layout_margin = "1%h",
        orientation = "vertical",
        layout_height = "wrap_content",
        layout_marginLeft = "5dp",
        {
          LinearLayout,
          layout_width = "fill",
          layout_height = "2%h",
        },
        {
          LinearLayout,
          layout_width = "wrap",
          gravity = "center",
          layout_height = "wrap",
          orientation = "horizontal",
          {
            TextView,
            layout_width = "wrap",
            id = "Statuscheat",
            textSize = "14sp",
            textColor = "0xFFFFFFFF",
            text = " Bypass Status: ",
            layout_height = "match_parent",
            layout_marginTop = "6dp",
          },
          {
            TextView,
            layout_width = "wrap",
            id = "Safestatus",
            textSize = "14sp",
            textColor = "0xFF00FF00",
            text = "Safe ( 1.6.56 )",
            layout_height = "wrap",
            layout_marginTop = "6dp",
          },
        },
        {
          LinearLayout,
          layout_width = "fill",
          layout_height = "0.5%h",
        },
        {
          LinearLayout,
          layout_width = "wrap",
          gravity = "center",
          layout_height = "wrap",
          orientation = "horizontal",
          {
            TextView,
            layout_width = "wrap",
            id = "Devicemodel",
            textSize = "14sp",
            textColor = "0xFFFFFFFF",
            text = "    Device Model: ",
            layout_height = "wrap",
            layout_marginTop = "1dp",
          },
          {
            TextView,
            layout_width = "wrap",
            id = "Potatophone",
            textSize = "14sp",
            textColor = "0xFF00FF00",
            text = "",
            layout_height = "wrap",
            layout_marginTop = "1dp",
          },
        },
        {
          LinearLayout,
          layout_width = "fill",
          layout_height = "0.5%h",
        },
        {
          LinearLayout,
          layout_width = "wrap",
          gravity = "center",
          layout_height = "wrap",
          orientation = "horizontal",
          {
            TextView,
            layout_width = "wrap",
            id = "loaderserver",
            textSize = "14sp",
            text = "    Loader Server : ",
            layout_height = "wrap",
            textColor = "0xFFFFFFFF",
          },
          {
            TextView,
            layout_width = "wrap",
            id = "loaderonline",
            textSize = "14sp",
            text = "Online",
            layout_height = "wrap",
            textColor = "0xFF00FF00",
          },
        },
        {
          LinearLayout,
          layout_width = "fill",
          layout_height = "0.5%h",
        },
        {
          LinearLayout,
          layout_width = "wrap",
          gravity = "center",
          layout_height = "wrap",
          orientation = "horizontal",
          {
            TextView,
            layout_width = "wrap",
            id = "Statusroot",
            textSize = "14sp",
            text = "    Status: ",
            layout_height = "wrap",
            textColor = "0xFFFFFFFF",
          },
          {
            TextView,
            layout_width = "wrap",
            id = "rotornot",
            textSize = "14sp",
            text = "",
            layout_height = "wrap",
            textColor = "0xFFFFFFFF",
          },
        },
      },
    },
    {
      CardView,
      layout_width = "350dp",
      id = "game",
      radius = "10dp",
      layout_gravity = "center",
      layout_height = "35dp",
      layout_marginTop = "7dp",
      BackgroundColor = "0x52000000",
      {
        LinearLayout,
        layout_gravity = "center",
        orientation = "vertical",
        layout_height = "wrap",
        layout_width = "fill",
        {
          LinearLayout,
          layout_gravity = "center",
          orientation = "horizontal",
          layout_height = "wrap",
          layout_width = "fill",
          {
            LinearLayout,
            layout_gravity = "center",
            orientation = "vertical",
            layout_height = "wrap",
            layout_width = "80%w",
            {
              LinearLayout,
              layout_width = "fill",
              layout_gravity = "center",
              orientation = "horizontal",
              layout_height = "wrap",
              layout_marginLeft = "5dp",
              {
                TextView,
                layout_width = "wrap",
                id = "Oooo",
                layout_gravity = "center",
                text = "Garena Version",
                textColor = "0xFFFFFFFF",
                layout_marginLeft = "10dp",
              },
            },
          },
          {
            LinearLayout,
            layout_gravity = "center",
            orientation = "horizontal",
            layout_height = "wrap",
            layout_width = "fill",
            {
              LinearLayout,
              layout_gravity = "center",
              gravity = "right",
              layout_height = "wrap",
              layout_width = "fill",
              {
                ImageView,
                layout_width = "15dp",
                layout_margin = "10dp",
                colorFilter = "0xFFFFFFFF",
                layout_gravity = "center",
                id = "arrowImg1",
                layout_height = "15dp",
                src = "files/res/arrow.png",
              },
            },
          },
        },
      },
    },
    {
      CardView,
      layout_marginRight = "25dp",
      radius = 15,
      CardElevation = "0dp",
      layout_marginLeft = "25dp",
      layout_width = "fill",
      backgroundColor = "0x00000000",
      layout_gravity = "center",
      layout_height = "5%h",
      id = "T",
      {
        LinearLayout,
        layout_gravity = "center",
        orientation = "horizontal",
        layout_height = "fill",
        layout_width = "fill",
        {
          TextView,
          layout_width = "wrap",
          id = "test",
          textSize = "11sp",
          layout_gravity = "bottom",
          text = "Dev Note: Have fun! and keep supporting for more updates!",
          textColor = "0xFFFFFFFF",
        },
      },
    },
  }

  minlay = {
    LinearLayout,
    layout_width = "45dp",
    layout_height = "45dp",
    {
      ImageView,
      layout_width = "45dp",
      src = "icon.png",
      id = "Win_minWindow",
      layout_height = "45dp",
    },
  }

  local dgNames = {
    "Wallhack", "Kinetic Weapon", "No Recoil", "No Spread",
    "Esp Name Br", "Hitbox Body", "No Parachute", "Fast Scope",
    "Fast Switch", "No Reload", "Pump Boost", "Advance Uav",
    "Long Slide", "No FlashBang", "Walk Underwater", "Unlock BluePrint",
  }

  local bypassPageChildren = {
    LinearLayout,
    layout_height = "-1",
    layout_width = "-1",
    orientation = "vertical",
    {
      LinearLayout,
      layout_height = "wrap",
      orientation = "vertical",
      layout_width = "fill",
      gravity = "center",
    },
    {
      TextView,
      id = "Menubyp",
      text = "Bypass Menu",
      textSize = "15dp",
      textColor = "0xFFFFFFFF",
    },
    {
      TextView,
      id = "Menubyp1",
      text = "Make Sure To Enable Bypass Before Memory Features...",
      textSize = "11dp",
      textColor = "0x78FFFFFF",
    },
    {
      ToggleButton,
      layout_width = "-1",
      layout_height = "6%h",
      id = "logo",
      textColor = "#ffffffff",
      layout_margin = "3dp",
      layout_gravity = "center",
      text = "Bypass Logo",
      textOn = "Bypass Logo",
      textOff = "Bypass Logo",
      textSize = "12.4sp",
    },
    {
      ToggleButton,
      layout_width = "-1",
      layout_height = "6%h",
      id = "clearlogs",
      textColor = "#ffffffff",
      layout_margin = "3dp",
      layout_gravity = "center",
      text = "Clear Logs",
      textOn = "Clear Logs",
      textOff = "Clear Logs",
      textSize = "12.4sp",
    },
    {
      ToggleButton,
      layout_width = "-1",
      layout_height = "6%h",
      id = "fps",
      textColor = "#ffffffff",
      layout_margin = "3dp",
      layout_gravity = "center",
      text = "Smooth Fps",
      textOn = "Smooth Fps",
      textOff = "Smooth Fps",
      textSize = "12.4sp",
    },
    {
      ToggleButton,
      layout_width = "-1",
      layout_height = "6%h",
      id = "skip",
      textColor = "#ffffffff",
      layout_margin = "3dp",
      layout_gravity = "center",
      text = "Skip Tutorial",
      textOn = "Skip Tutorial",
      textOff = "Skip Tutorial",
      textSize = "12.4sp",
    },
  }

  local featuresChildren = {
    LinearLayout,
    layout_height = "-1",
    layout_width = "-1",
    orientation = "vertical",
    {
      LinearLayout,
      layout_height = "wrap",
      orientation = "vertical",
      layout_width = "fill",
      gravity = "center",
    },
    {
      TextView,
      id = "Menufeats",
      text = "Cheat Features",
      textSize = "15dp",
      textColor = "0xFFFFFFFF",
    },
    {
      TextView,
      id = "Menufeats1",
      text = "If Cheats Don't Work Just Restart The Game And Injector",
      textSize = "11dp",
      textColor = "0x74FFFFFF",
    },
    {
      ToggleButton,
      layout_width = "-1",
      layout_height = "5%h",
      id = "showmenu",
      textColor = "#ffffffff",
      layout_margin = "3dp",
      layout_gravity = "center",
      textOn = "Close Cheat Features",
      textOff = "Show Cheat Features",
      textSize = "13sp",
    },
  }

  local dgContainer = {
    LinearLayout,
    layout_height = "-1",
    layout_width = "-1",
    orientation = "vertical",
    id = "featuresm",
  }
  for i, name in ipairs(dgNames) do
    local cb = {
      CheckBox,
      layout_width = "-1",
      layout_height = "wrap",
      textColor = "0xFFFFFFFF",
      text = name,
      textSize = "15dp",
      id = "dg" .. i,
    }
    if i % 2 == 0 then
      cb.layout_marginLeft = "53%w"
      cb.layout_marginTop = "-32dp"
    end
    table.insert(dgContainer, cb)
  end
  table.insert(featuresChildren, dgContainer)

  table.insert(featuresChildren, {
    LinearLayout,
    orientation = "vertical",
    layout_width = "match",
    layout_height = "wrap",
    {
      TextView,
      textSize = "15sp",
      textColor = "0xFFFFFFFF",
      id = "Seekbars",
      gravity = "center",
      layout_width = "wrap",
      layout_height = "wrap",
      layout_gravity = "center",
      text = "SeekBar Menu",
    },
  })
  table.insert(featuresChildren, {
    TextView,
    text = "Adjustable Aim (0%)",
    textColor = "0xFFFFFFFF",
    textSize = "15sp",
    layout_width = "fill",
    layout_height = "wrap",
    layout_gravity = "center",
    id = "aimbot_text",
  })
  table.insert(featuresChildren, {
    SeekBar,
    layout_width = "fill",
    layout_height = "wrap",
    max = 100,
    progress = 0,
    id = "aimbot_seekbar",
  })
  table.insert(featuresChildren, {
    TextView,
    text = "Adjustable SnowB. (0%)",
    textColor = "0xFFFFFFFF",
    textSize = "15sp",
    layout_width = "fill",
    layout_height = "wrap",
    layout_gravity = "center",
    id = "snowboard_text",
  })
  table.insert(featuresChildren, {
    SeekBar,
    layout_width = "fill",
    layout_height = "wrap",
    max = 100,
    progress = 0,
    id = "snowboard_seekbar",
  })

  table.insert(featuresChildren, {
    LinearLayout,
    orientation = "vertical",
    layout_width = "match",
    layout_height = "wrap",
    {
      TextView,
      textSize = "15sp",
      textColor = "0xFFFFFFFF",
      id = "Antennatext",
      gravity = "center",
      layout_width = "wrap",
      layout_height = "wrap",
      layout_gravity = "center",
      text = "Antenna Menu",
    },
  })
  table.insert(featuresChildren, {
    LinearLayout,
    layout_height = "wrap",
    layout_width = "fill",
    layout_marginRight = "1dp",
    orientation = "horizontal",
    {
      CheckBox,
      id = "antenna",
      text = " [ RED ] ",
      padding = "wrap",
      layout_width = "wrap",
      layout_height = "25dp",
      textSize = "16sp",
      layout_gravity = "center",
      textColor = "0xFFFFFFFF",
      background = "0x8E615F5F",
    },
    {
      CheckBox,
      id = "antenna1",
      text = " [ CYAN ] ",
      padding = "wrap",
      layout_width = "wrap",
      layout_height = "25dp",
      textSize = "16sp",
      layout_gravity = "center",
      textColor = "0xFFFFFFFF",
      background = "0x8E615F5F",
    },
    {
      CheckBox,
      id = "antenna2",
      text = " [ BLACK ] ",
      padding = "wrap",
      layout_width = "wrap",
      layout_height = "25dp",
      textSize = "16sp",
      layout_gravity = "center",
      textColor = "0xFFFFFFFF",
      background = "0x8E615F5F",
    },
  })

-- ============================================================
-- CHARACTER SKINS
-- ============================================================
table.insert(featuresChildren, {
  TextView,
  id = "Character",
  text = "Character Skins",
  textSize = "15dp",
  textColor = "0xFFFFFFFF",
})
table.insert(featuresChildren, {
  TextView,
  id = "Character1",
  text = "To Work Character Skins Must Use Default Character",
  textSize = "11dp",
  textColor = "0x74FFFFFF",
})
table.insert(featuresChildren, {
  ToggleButton,
  layout_width = "-1",
  layout_height = "5%h",
  id = "characmenu",
  textColor = "#ffffffff",
  layout_margin = "3dp",
  layout_gravity = "center",
  textOn = "CLOSE CHARACTER SKINS",
  textOff = "SHOW CHARACTER SKINS",
  textSize = "13sp",
})

local csContainer = {
  LinearLayout,
  layout_height = "-1",
  layout_width = "-1",
  orientation = "vertical",
  id = "csContainer",
}

local csNames = {
  {id="kuiji", text="Kuiji Eternal Bearer"},
  {id="lazarus", text="Codename Lazarus Wath"},
  {id="shepherd", text="Darkshepherd Phantom"},
  {id="sophia", text="Sophia Errentknight"},
  {id="spectre", text="Spectre T-3"},
  {id="templar", text="Templar Cousteau's Oath"},
  {id="siren", text="Siren Siren Song"},
  {id="ghost", text="Ghost Eternal Siege"},
  {id="chunli", text="Shadowfall Chun li"},
  {id="ryu", text="Hidora kai Ryu"},
  {id="cammy", text="Fiona St. George Cammy"},
  {id="akuma", text="Shinegori Akuma"},
  {id="blacknoir", text="Black noir"},
  {id="starlight", text="Starlight"},
  {id="homelander", text="Homelander"},
}

for i, item in ipairs(csNames) do
  local radio = {
    RadioButton,
    text = item.text,
    textColor = "0xFFFFFFFF",
    id = item.id,
    textSize = "12sp",
    layout_width = "77%w",
    layout_height = "wrap",
    ButtonDrawable = {ColorFilter = "0xFF9C27B0"},
  }
  if i % 2 == 0 then
    radio.layout_marginLeft = "53%w"
    radio.layout_marginTop = "-28dp"
  end
  table.insert(csContainer, radio)
end
table.insert(featuresChildren, csContainer)

-- ============================================================
-- MYTHIC WEAPONS
-- ============================================================
table.insert(featuresChildren, {
  TextView,
  id = "MythicHeader",
  text = "Mythic Weapons",
  textColor = "0xFFFFFFFF",
  textSize = "15dp",
})
table.insert(featuresChildren, {
  TextView,
  id = "MythicNote",
  text = "Available Mythic Weapon Skins",
  textSize = "11dp",
  textColor = "0x74FFFFFF",
})
table.insert(featuresChildren, {
  ToggleButton,
  layout_width = "-1",
  layout_height = "5%h",
  id = "mythicmenu",
  textColor = "#ffffffff",
  layout_margin = "3dp",
  layout_gravity = "center",
  textOn = "CLOSE MYTHIC WEAPONS",
  textOff = "SHOW MYTHIC WEAPONS",
  textSize = "13sp",
})

local mythicContainer = {
  LinearLayout,
  layout_height = "-1",
  layout_width = "-1",
  orientation = "vertical",
  id = "mythicContainer",
}

local mythicNames = {
  {id="ak117lava", text="Ak117 Lava Remix"},
  {id="ak117", text="Ak117 Memento Mori"},
  {id="bp50", text="Bp50"},
  {id="ffar", text="Ffar"},
  {id="grau", text="Grau"},
  {id="krig6", text="Krig6"},
  {id="type19", text="Type19"},
  {id="oden", text="Oden"},
  {id="xm4", text="Xm4"},
  {id="ak47", text="Ak47"},
  {id="lw3", text="Tundra"},
  {id="dlq33", text="Dlq"},
  {id="vmp", text="Vmp"},
  {id="uss9", text="Uss9"},
  {id="kilo", text="Kilo"},
  {id="switchh", text="Switchblade"},
  {id="jak12", text="Jak12"},
  {id="cx9", text="Cx9"},
  {id="qq9", text="Qq9"},
  {id="mg42", text="Mg42"},
  {id="m13", text="M13"},
  {id="fennec", text="Fennec"},
  {id="rytec", text="Rytec"},
  {id="holger", text="Holger"},
  {id="em2", text="Em2"},
  {id="cbr", text="Cbr"},
  {id="asval", text="Asval"},
  {id="peace", text="Peacekeeper"},
  {id="ram7", text="Ram7"},
  {id="type25", text="Type25"},
  {id="so14", text="So14"},
  {id="lachmann", text="Lachmann"},
  {id="dp27", text="Dp27"},
}

for i, item in ipairs(mythicNames) do
  local radio = {
    RadioButton,
    text = item.text,
    textColor = "0xFFFFFFFF",
    id = item.id,
    textSize = "12sp",
    layout_width = "77%w",
    layout_height = "wrap",
    ButtonDrawable = {ColorFilter = "0xFF9C27B0"},
  }
  if i % 2 == 0 then
    radio.layout_marginLeft = "53%w"
    radio.layout_marginTop = "-28dp"
  end
  table.insert(mythicContainer, radio)
end
table.insert(featuresChildren, mythicContainer)

-- ============================================================
-- LEGENDARY WEAPONS
-- ============================================================
table.insert(featuresChildren, {
  TextView,
  id = "LegendaryHeader",
  text = "Legendary Weapons",
  textColor = "0xFFFFFFFF",
  textSize = "15dp",
})
table.insert(featuresChildren, {
  TextView,
  id = "LegendaryNote",
  text = "Available Legendary Weapon Skins",
  textSize = "11dp",
  textColor = "0x74FFFFFF",
})
table.insert(featuresChildren, {
  ToggleButton,
  layout_width = "-1",
  layout_height = "5%h",
  id = "legendarymenu",
  textColor = "#ffffffff",
  layout_margin = "3dp",
  layout_gravity = "center",
  textOn = "CLOSE LEGENDARY WEAPONS",
  textOff = "SHOW LEGENDARY WEAPONS",
  textSize = "13sp",
})

local legendaryContainer = {
  LinearLayout,
  layout_height = "-1",
  layout_width = "-1",
  orientation = "vertical",
  id = "legendaryContainer",
}

local legendaryNames = {
  {id="krm", text="Glorious Blaze"},
  {id="krmred", text="Red Fissure"},
  {id="krmload", text="Loaded Glitch"},
  {id="locus", text="Electron"},
  {id="locusdemon", text="Demon Breath"},
  {id="locusmetal", text="Metal Whiskers"},
  {id="locusneptune", text="Neptune"},
  {id="locuscomet", text="Red Comet"},
  {id="locusicy", text="Icy Claws"},
  {id="by15", text="Boba Blaster"},
  {id="hssong", text="Songtress"},
  {id="dlqholi", text="Holidays"},
  {id="dlqzealot", text="Zealot"},
}

for i, item in ipairs(legendaryNames) do
  local radio = {
    RadioButton,
    text = item.text,
    textColor = "0xFFFFFFFF",
    id = item.id,
    textSize = "12sp",
    layout_width = "77%w",
    layout_height = "wrap",
    ButtonDrawable = {ColorFilter = "0xFF9C27B0"},
  }
  if i % 2 == 0 then
    radio.layout_marginLeft = "53%w"
    radio.layout_marginTop = "-28dp"
  end
  table.insert(legendaryContainer, radio)
end
table.insert(featuresChildren, legendaryContainer)

-- ============================================================
-- MELEE WEAPONS
-- ============================================================
table.insert(featuresChildren, {
  TextView,
  id = "MeleeHeader",
  text = "Melee Weapons",
  textColor = "0xFFFFFFFF",
  textSize = "15dp",
})
table.insert(featuresChildren, {
  TextView,
  id = "MeleeNote",
  text = "Available Melee Weapon Skins",
  textSize = "11dp",
  textColor = "0x74FFFFFF",
})
table.insert(featuresChildren, {
  ToggleButton,
  layout_width = "-1",
  layout_height = "5%h",
  id = "meleemenu",
  textColor = "#ffffffff",
  layout_margin = "3dp",
  layout_gravity = "center",
  textOn = "CLOSE MELEE WEAPONS",
  textOff = "SHOW MELEE WEAPONS",
  textSize = "13sp",
})

local meleeContainer = {
  LinearLayout,
  layout_height = "-1",
  layout_width = "-1",
  orientation = "vertical",
  id = "meleeContainer",
}

local meleeNames = {
  {id="tang", text="Tang Knife"},
  {id="longq", text="Longquan"},
  {id="spear", text="Spear Azure"},
  {id="scissors", text="Scissors"},
  {id="tomahawk", text="Tomahawk"},
  {id="saber", text="Saber"},
  {id="fiery", text="Fiery"},
}

for i, item in ipairs(meleeNames) do
  local radio = {
    RadioButton,
    text = item.text,
    textColor = "0xFFFFFFFF",
    id = item.id,
    textSize = "12sp",
    layout_width = "77%w",
    layout_height = "wrap",
    ButtonDrawable = {ColorFilter = "0xFF9C27B0"},
  }
  if i % 2 == 0 then
    radio.layout_marginLeft = "53%w"
    radio.layout_marginTop = "-28dp"
  end
  table.insert(meleeContainer, radio)
end
table.insert(featuresChildren, meleeContainer)

-- ============================================================
-- EQUIPMENT & VEHICLE
-- ============================================================
table.insert(featuresChildren, {
  TextView,
  id = "VehicleHeader",
  text = "Equipment & Vehicle",
  textColor = "0xFFFFFFFF",
  textSize = "15dp",
})
table.insert(featuresChildren, {
  TextView,
  id = "VehicleNote",
  text = "Available Vehicle and Equipment Skins",
  textSize = "11dp",
  textColor = "0x74FFFFFF",
})
table.insert(featuresChildren, {
  ToggleButton,
  layout_width = "-1",
  layout_height = "5%h",
  id = "vehiclemenu",
  textColor = "#ffffffff",
  layout_margin = "3dp",
  layout_gravity = "center",
  textOn = "CLOSE VEHICLE SKINS",
  textOff = "SHOW VEHICLE SKINS",
  textSize = "13sp",
})

local vehicleContainer = {
  LinearLayout,
  layout_height = "-1",
  layout_width = "-1",
  orientation = "vertical",
  id = "vehicleContainer",
}

local vehicleNames = {
  {id="sand", text="Sand"},
  {id="blaze", text="Blaze"},
  {id="mech", text="Mech"},
  {id="parachute", text="Parachute"},
}

for i, item in ipairs(vehicleNames) do
  local radio = {
    RadioButton,
    text = item.text,
    textColor = "0xFFFFFFFF",
    id = item.id,
    textSize = "12sp",
    layout_width = "77%w",
    layout_height = "wrap",
    ButtonDrawable = {ColorFilter = "0xFF9C27B0"},
  }
  if i % 2 == 0 then
    radio.layout_marginLeft = "53%w"
    radio.layout_marginTop = "-28dp"
  end
  table.insert(vehicleContainer, radio)
end
table.insert(featuresChildren, vehicleContainer)

-- ============================================================
-- CAMO FEATURES
-- ============================================================
table.insert(featuresChildren, {
  TextView,
  id = "CamoHeader",
  text = "Camo Features",
  textColor = "0xFFFFFFFF",
  textSize = "15dp",
})
table.insert(featuresChildren, {
  TextView,
  id = "CamoNote",
  text = "Note: Inject Skin First",
  textColor = "0xFFFF6B6B",
  textSize = "12sp",
})
table.insert(featuresChildren, {
  ToggleButton,
  layout_width = "-1",
  layout_height = "5%h",
  id = "camomenu",
  textColor = "#ffffffff",
  layout_margin = "3dp",
  layout_gravity = "center",
  textOn = "CLOSE CAMO FEATURES",
  textOff = "SHOW CAMO FEATURES",
  textSize = "13sp",
})

local camoContainer = {
  LinearLayout,
  layout_height = "-1",
  layout_width = "-1",
  orientation = "vertical",
  id = "camoContainer",
}

local camoNames = {
  {id="diamond", text="Diamond"},
  {id="redsprite", text="RedSprite"},
  {id="glacial", text="GlacialRipple"},
  {id="aether", text="AetherCrystal"},
  {id="alchemy", text="Alchemy Stars"},
  {id="fluorescent", text="Fluorescent"},
  {id="lunar", text="Lunar Tear"},
  {id="sunken", text="Sunken Gambit"},
  {id="ocean", text="OceanWaves"},
  {id="psychic", text="Psychic Distortion"},
  {id="shimmer", text="Shimmer"},
  {id="astronomy", text="Astronomy"},
  {id="powernova", text="PowerNova"},
  {id="makingwaves", text="MakingWaves"},
  {id="blistering", text="BlisteringMagma"},
  {id="emerald", text="GoldenEmerald"},
  {id="assault", text="AssaultPattern"},
  {id="scorch", text="ScorchMelt"},
}

for i, item in ipairs(camoNames) do
  local radio = {
    RadioButton,
    text = item.text,
    textColor = "0xFFFFFFFF",
    id = item.id,
    textSize = "12sp",
    layout_width = "77%w",
    layout_height = "wrap",
    ButtonDrawable = {ColorFilter = "0xFF00BCD4"},
  }
  if i % 2 == 0 then
    radio.layout_marginLeft = "53%w"
    radio.layout_marginTop = "-28dp"
  end
  table.insert(camoContainer, radio)
end
table.insert(featuresChildren, camoContainer)

  local otherPageChildren = {
    LinearLayout,
    layout_height = "-1",
    layout_width = "-1",
    orientation = "vertical",
    {
      LinearLayout,
      orientation = "horizontal",
      layout_height = "10",
      layout_width = "-1",
    },
    {
      LinearLayout,
      layout_width = "fill",
      layout_height = "wrap",
      orientation = "horizontal",
      gravity = "center",
      {
        Button,
        layout_width = "30%w",
        layout_height = "5%h",
        id = "saveconfig",
        text = "SAVE",
        textColor = "#ffffffff",
        layout_margin = "3dp",
        textSize = "12sp",
      },
      {
        Button,
        layout_width = "30%w",
        layout_height = "5%h",
        id = "loadconfig",
        text = "LOAD",
        textColor = "#ffffffff",
        layout_margin = "3dp",
        textSize = "12sp",
      },
      {
        Button,
        layout_width = "30%w",
        layout_height = "5%h",
        id = "resetconfig",
        text = "RESET",
        textColor = "#ffffffff",
        layout_margin = "3dp",
        textSize = "12sp",
      },
    },
    {
      LinearLayout,
      layout_width = "fill",
      layout_height = "1%h",
    },
    {
      Button,
      layout_width = "-1",
      layout_height = "5%h",
      id = "closeui",
      text = "CLOSE MENU",
      textColor = "#ffffffff",
      layout_margin = "3dp",
      layout_gravity = "center",
      textSize = "13sp",
    },
    {
      TextView,
      id = "feedback",
      text = "Feedback Center",
      textSize = "15dp",
      textColor = "0xFFFFFFFF",
    },
    {
      TextView,
      id = "bugcenter",
      text = "Found Bug Or Want To Feedback? Just Dm Me In Telegram",
      textSize = "10dp",
      textColor = "0x74FFFFFF",
    },
    {
      Button,
      layout_width = "-1",
      layout_height = "5%h",
      id = "telegram",
      text = "Telegram Account",
      textColor = "#ffffffff",
      layout_margin = "3dp",
      layout_gravity = "center",
      textSize = "13sp",
    },
    {
      Button,
      layout_width = "-1",
      layout_height = "5%h",
      id = "telegramch",
      text = "Telegram Channel",
      textColor = "#ffffffff",
      layout_margin = "3dp",
      layout_gravity = "center",
      textSize = "13sp",
    },
    {
      Button,
      layout_width = "-1",
      layout_height = "5%h",
      id = "youtube",
      text = "YOUTUBE CHANNEL",
      textColor = "#ffffffff",
      layout_margin = "3dp",
      layout_gravity = "center",
      textSize = "13sp",
    },
  }

  winlay = {
    LinearLayout,
    layout_width = "-1",
    layout_height = "-1",
    background = "transparent",
    {
      CardView,
      id = "win_mainview",
      layout_width = "85%w",
      layout_height = "38%h",
      layout_margin = "5dp",
      CardElevation = "5dp",
      layout_gravity = "center",
      {
        LinearLayout,
        orientation = "vertical",
        layout_width = "fill_parent",
        background = "transparent",
        {
          LinearLayout,
          layout_width = "fill_parent",
          background = "transparent",
          {
            LinearLayout,
            orientation = "horizontal",
            layout_gravity = "center",
            layout_height = "5%h",
            layout_width = "fill_parent",
            gravity = "center",
            background = "transparent",
            padding = "0dp",
            id = "win_move",
            {
              TextView,
              textColor = "0xFFFF99CC",
              textSize = "20sp",
              text = "HexPhantom",
              ellipsize = "marquee",
              focusableInTouchMode = true,
              focusable = true,
              singleLine = false,
              layout_width = "43%w",
              layout_gravity = "center",
              gravity = "center",
              id = "title",
            },
            {
              LinearLayout,
              orientation = "horizontal",
              layout_height = "fill",
              layout_width = "fill",
              gravity = "center",
              background = "transparent",
              {
                ImageView,
                id = "lobby",
                src = "font/bypass.png",
                layout_width = "7.5%w",
                layout_height = "5%h",
                paddingLeft = "5dp",
                colorFilter = "0xFFFFFFFF",
                layout_gravity = "right",
              },
              {
                ImageView,
                id = "ingame",
                src = "font/features.png",
                layout_width = "9.5%w",
                layout_height = "5%h",
                paddingLeft = "9dp",
                colorFilter = "0xFFFFFFFF",
                layout_gravity = "right",
              },
              {
                ImageView,
                id = "other",
                src = "font/other.png",
                layout_width = "9.5%w",
                layout_height = "5%h",
                paddingLeft = "9dp",
                colorFilter = "0xFFFFFFFF",
                layout_gravity = "right",
              },
              {
                ImageView,
                id = "changeWindow",
                src = "font/hide.png",
                layout_width = "10%w",
                layout_height = "6%h",
                paddingLeft = "9dp",
                colorFilter = "0xFFFFFFFF",
                layout_gravity = "center",
              },
            },
          },
        },
        {
          PageView,
          id = "pg",
          layout_width = "fill",
          layout_height = "fill",
          pages = {
            {
              LinearLayout,
              orientation = "vertical",
              {
                ScrollView,
                layout_width = "fill",
                layout_height = "fill",
                layout_gravity = "center_horizontal",
                padding = "10",
                bypassPageChildren,
              },
            },
            {
              LinearLayout,
              orientation = "vertical",
              {
                ScrollView,
                layout_width = "fill_parent",
                layout_height = "fill",
                layout_gravity = "center_horizontal",
                padding = "10",
                featuresChildren,
              },
            },
            {
              LinearLayout,
              orientation = "vertical",
              {
                ScrollView,
                layout_width = "fill_parent",
                layout_height = "fill",
                layout_gravity = "center_horizontal",
                padding = "10",
                otherPageChildren,
              },
            },
          },
        },
      },
    },
  }

  pcall(function()
    activity.setTheme(R.AndLua12)
    activity.actionBar.hide()
  end)
  activity.setContentView(loadlayout(loaderLayout))
  local win = activity.getWindow()
  win.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
  win.setStatusBarColor(0xff970000)
  win.setNavigationBarColor(0xff970000)

  grad = GradientDrawable(GradientDrawable.Orientation.BOTTOM_TOP, { 0xff720000, 0xff000000, 0xff720000 })
  mainbg.setBackground(grad)

  Potatophone.setText("" .. Build.MODEL)

  LayoutVIP = activity.getSystemService(Context.WINDOW_SERVICE)
  HasFocus = false
  HasLaunch = false
  isMax = false

  WmHz = WindowManager.LayoutParams()
  if Build.VERSION.SDK_INT >= 26 then
    WmHz.type = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY
   else
    WmHz.type = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT
  end
  WmHz.format = PixelFormat.RGBA_8888
  WmHz.flags = WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
  WmHz.gravity = Gravity.LEFT | Gravity.TOP
  WmHz.x = 333
  WmHz.y = 333
  WmHz.width = WindowManager.LayoutParams.WRAP_CONTENT
  WmHz.height = WindowManager.LayoutParams.WRAP_CONTENT

  mainWindow = loadlayout(winlay)
  minWindow = loadlayout(minlay)

  view = title
  color1 = 0xffff0000
  color2 = 0xff8080ff
  color3 = 0xff80ffff
  color4 = 0xff80ff80
  colorAnim = ObjectAnimator.ofInt(view, "textColor", { color1, color2, color3, color4 })
  colorAnim.setDuration(2000)
  colorAnim.setEvaluator(ArgbEvaluator())
  colorAnim.setRepeatCount(ValueAnimator.INFINITE)
  colorAnim.setRepeatMode(ValueAnimator.REVERSE)
  colorAnim.start()

  Alpha = AlphaAnimation(0, 1)
  Alpha.setDuration(800)
  win_mainview.startAnimation(Alpha)

  closeui.onClick = function()
    HasLaunch = false
    isMax = false
    pcall(function() LayoutVIP.removeView(mainWindow) end)
    pcall(function() LayoutVIP.removeView(minWindow) end)
  end

  changeWindow.onClick = function()
    if isMax then
      isMax = false
      pcall(function() LayoutVIP.removeView(mainWindow) end)
      pcall(function() LayoutVIP.addView(minWindow, WmHz) end)
    end
  end

  Win_minWindow.onClick = function()
    if not isMax then
      isMax = true
      pcall(function() LayoutVIP.removeView(minWindow) end)
      pcall(function() LayoutVIP.addView(mainWindow, WmHz) end)
      win_mainview.startAnimation(Alpha)
    end
  end

  Win_minWindow.onTouch = function(v, event)
    local action = event.getAction()
    if action == MotionEvent.ACTION_DOWN then
      firstX = event.getRawX()
      firstY = event.getRawY()
      wmX = WmHz.x
      wmY = WmHz.y
     elseif action == MotionEvent.ACTION_MOVE then
      WmHz.x = wmX + event.getRawX() - firstX
      WmHz.y = wmY + event.getRawY() - firstY
      LayoutVIP.updateViewLayout(minWindow, WmHz)
     elseif action == MotionEvent.ACTION_UP then
      return false
    end
    return false
  end

  win_move.onTouch = function(v, event)
    local action = event.getAction()
    if action == MotionEvent.ACTION_DOWN then
      firstX = event.getRawX()
      firstY = event.getRawY()
      wmX = WmHz.x
      wmY = WmHz.y
     elseif action == MotionEvent.ACTION_MOVE then
      WmHz.x = wmX + event.getRawX() - firstX
      WmHz.y = wmY + event.getRawY() - firstY
      LayoutVIP.updateViewLayout(mainWindow, WmHz)
     elseif action == MotionEvent.ACTION_UP then
      return true
    end
    return true
  end

  start.onClick = function()
    Waterdropanimation(start, 100)
    win_mainview.startAnimation(Alpha)
    if HasLaunch then
      return
    end
    HasLaunch = true
    pcall(function()
      LayoutVIP.addView(minWindow, WmHz)
    end)
    pcall(function()
      local f, err = io.open("/data/data/com.xzykayl.vip.injector/files/xyzxros.lua")
      if err == nil then
        f:close()
        local intent = activity.getPackageManager().getLaunchIntentForPackage("andlua.layout.vip")
        if intent then
          activity.startActivity(intent)
        end
      end
    end)
  end

  game.onClick = function()
    Waterdropanimation(game, 100)
    local ok = pcall(function()
      activity.getPackageManager().getPackageInfo("com.garena.game.codm", 0)
    end)
    if ok then
      local intent = activity.getPackageManager().getLaunchIntentForPackage("com.garena.game.codm")
      this.startActivity(intent)
      isMax = true
     else
      print("CALL OF DUTY MOBILE NOT INSTALLED")
      idkcstmToast("CODM Garena not installed!")
    end
  end

  exit.onClick = function()
    Waterdropanimation(exit, 100)
    os.exit()
  end

  task(1000, function()
    if isRootAvailable() then
      rotornot.Text = "Root"
      rotornot.textColor = 0xff00ff00
     else
      rotornot.Text = "Non Root"
      rotornot.textColor = 0xffff0000
    end
  end)

  saveconfig.onClick = function()
    Waterdropanimation(saveconfig, 100)
    ConfigManager.saveConfig()
  end

  loadconfig.onClick = function()
    Waterdropanimation(loadconfig, 100)
    ConfigManager.loadConfig()
  end

  resetconfig.onClick = function()
    Waterdropanimation(resetconfig, 100)
    ConfigManager.resetConfig()
  end

  local fontPath = activity.getLuaDir() .. "/font/doom.ttf"
local fontViews = {
  "Statuscheat", "Safestatus", "Devicemodel", "Potatophone", "Statusroot",
  "rotornot", "txtStartCheat", "txtStopCheat", "test", "Oooo", "title",
  "fps", "logo", "clearlogs", "skip", "aimbot_text", "snowboard_text",
  "antenna", "antenna1", "antenna2",
  "loaderserver", "loaderonline", "Menubyp", "Menubyp1", "Antennatext",
  "Seekbars", "Menufeats", "Menufeats1", "Character", "Character1",
  "showmenu", "characmenu", "mythicmenu", "legendarymenu", "meleemenu",
  "vehiclemenu", "camomenu", "closeui", "bugcenter", "feedback",
  "telegram", "telegramch", "youtube", "MythicHeader", "LegendaryHeader",
  "MeleeHeader", "VehicleHeader", "CamoHeader",
  "CamoNote", "MeleeNote", "LegendaryNote", "MythicNote", "VehicleNote",
  "saveconfig", "loadconfig", "resetconfig"
}
  local allSkinIds = {}
  for _, item in ipairs(csNames) do table.insert(allSkinIds, item.id) end
  for _, item in ipairs(mythicNames) do table.insert(allSkinIds, item.id) end
  for _, item in ipairs(legendaryNames) do table.insert(allSkinIds, item.id) end
  for _, item in ipairs(meleeNames) do table.insert(allSkinIds, item.id) end
  for _, item in ipairs(vehicleNames) do table.insert(allSkinIds, item.id) end
  for _, item in ipairs(camoNames) do table.insert(allSkinIds, item.id) end
  
  for _, id in ipairs(allSkinIds) do
    table.insert(fontViews, id)
  end
  
  for i = 1, 16 do table.insert(fontViews, "dg" .. i) end
  
  for _, id in ipairs(fontViews) do
    pcall(FontN, _G[id], fontPath)
  end

  CircleButtonAsh1(closeui, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(abcdefg, 0xff520000, 0xff000000, 10, 0xff520000)
  CircleButtonAsh1(game, 0xff520000, 0xff000000, 10, 0xff520000)
  CircleButtonAsh(start, 0xa2000000, 0xff700000, 10, 0xff000000)
  CircleButtonAsh(exit, 0xff700000, 0xa2000000, 10, 0xff000000)
  CircleButtonAsh(win_mainview, 0xff000000, 0xff520000, 10, 0xff000000)
  CircleButtonAsh1(logo, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(fps, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(clearlogs, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(skip, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(showmenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(characmenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(mythicmenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(legendarymenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(meleemenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(vehiclemenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(camomenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(telegram, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(telegramch, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(youtube, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
  CircleButtonAsh1(saveconfig, 0xff0066cc, 0xff000000, 10, 0xff0066cc)
  CircleButtonAsh1(loadconfig, 0xff00cc66, 0xff000000, 10, 0xff00cc66)
  CircleButtonAsh1(resetconfig, 0xffcc3300, 0xff000000, 10, 0xffcc3300)

  showmenu.setText("SHOW CHEAT FEATURES")
  characmenu.setText("SHOW CHARACTER SKINS")
  mythicmenu.setText("SHOW MYTHIC WEAPONS")
  legendarymenu.setText("SHOW LEGENDARY WEAPONS")
  meleemenu.setText("SHOW MELEE WEAPONS")
  vehiclemenu.setText("SHOW VEHICLE SKINS")
  camomenu.setText("SHOW CAMO FEATURES")

  telegram.onClick = function()
    Waterdropanimation(telegram, 100)
    activity.startActivity(Intent(Intent.ACTION_VIEW, Uri.parse("https://t.me/Hexxx22")))
  end

  youtube.onClick = function()
    Waterdropanimation(youtube, 100)
    activity.startActivity(Intent(Intent.ACTION_VIEW, Uri.parse("https://youtube.com/@keemhaxx12?si=XTNMHbI8Dn3Heo-Y")))
  end

  telegramch.onClick = function()
    Waterdropanimation(telegramch, 100)
    activity.startActivity(Intent(Intent.ACTION_VIEW, Uri.parse("https://t.me/Hexxxx12")))
  end

  local Sleft, Sright = {}, {}
  for i = 0, 50 do
    Sleft[i] = TranslateAnimation(-1000, 0, 0, 0)
    Sleft[i].setDuration(300 + i * 20)
    Sleft[i].setFillAfter(false)
    Sright[i] = TranslateAnimation(1000, 0, 0, 0)
    Sright[i].setDuration(300 + i * 20)
    Sright[i].setFillAfter(false)
  end

  local function setListVisibility(ids, vis)
    for _, id in ipairs(ids) do
      pcall(function() 
        if _G[id] then
          _G[id].setVisibility(vis)
        end
      end)
    end
  end

  local function animateRadioButtons(viewIds)
    for i, id in ipairs(viewIds) do
      local view = _G[id]
      if view then
        if i % 2 == 1 then
          view.startAnimation(Sleft[math.floor((i - 1) / 2)])
        else
          view.startAnimation(Sright[math.floor((i - 1) / 2)])
        end
      end
    end
  end

  local dgViews = { "Seekbars", "aimbot_text", "aimbot_seekbar", "snowboard_text",
    "snowboard_seekbar", "Antennatext", "antenna", "antenna1", "antenna2" }
  for i = 1, 16 do table.insert(dgViews, "dg" .. i) end

  local csViews = {}
  for _, item in ipairs(csNames) do table.insert(csViews, item.id) end
  
  local mythicViews = {}
  for _, item in ipairs(mythicNames) do table.insert(mythicViews, item.id) end
  
  local legendaryViews = {}
  for _, item in ipairs(legendaryNames) do table.insert(legendaryViews, item.id) end
  
  local meleeViews = {}
  for _, item in ipairs(meleeNames) do table.insert(meleeViews, item.id) end
  
  local vehicleViews = {}
  for _, item in ipairs(vehicleNames) do table.insert(vehicleViews, item.id) end
  
  local camoViews = {}
  for _, item in ipairs(camoNames) do table.insert(camoViews, item.id) end

  setListVisibility(dgViews, View.GONE)
  setListVisibility(csViews, View.GONE)
  setListVisibility(mythicViews, View.GONE)
  setListVisibility(legendaryViews, View.GONE)
  setListVisibility(meleeViews, View.GONE)
  setListVisibility(vehicleViews, View.GONE)
  setListVisibility(camoViews, View.GONE)

  showmenu.OnCheckedChangeListener = function()
    if showmenu.checked then
      CircleButtonAsh1(showmenu, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      setListVisibility(dgViews, View.VISIBLE)
      for i = 1, 16 do
        local view = _G["dg" .. i]
        if view then
          if i % 2 == 1 then
            view.startAnimation(Sleft[math.floor((i - 1) / 2)])
          else
            view.startAnimation(Sright[math.floor((i - 1) / 2)])
          end
        end
      end
    else
      CircleButtonAsh1(showmenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
      setListVisibility(dgViews, View.GONE)
    end
  end

  characmenu.OnCheckedChangeListener = function()
    if characmenu.checked then
      CircleButtonAsh1(characmenu, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      setListVisibility(csViews, View.VISIBLE)
      animateRadioButtons(csViews)
    else
      CircleButtonAsh1(characmenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
      setListVisibility(csViews, View.GONE)
    end
  end

  mythicmenu.OnCheckedChangeListener = function()
    if mythicmenu.checked then
      CircleButtonAsh1(mythicmenu, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      setListVisibility(mythicViews, View.VISIBLE)
      animateRadioButtons(mythicViews)
    else
      CircleButtonAsh1(mythicmenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
      setListVisibility(mythicViews, View.GONE)
    end
  end

  legendarymenu.OnCheckedChangeListener = function()
    if legendarymenu.checked then
      CircleButtonAsh1(legendarymenu, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      setListVisibility(legendaryViews, View.VISIBLE)
      animateRadioButtons(legendaryViews)
    else
      CircleButtonAsh1(legendarymenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
      setListVisibility(legendaryViews, View.GONE)
    end
  end

  meleemenu.OnCheckedChangeListener = function()
    if meleemenu.checked then
      CircleButtonAsh1(meleemenu, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      setListVisibility(meleeViews, View.VISIBLE)
      animateRadioButtons(meleeViews)
    else
      CircleButtonAsh1(meleemenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
      setListVisibility(meleeViews, View.GONE)
    end
  end

  vehiclemenu.OnCheckedChangeListener = function()
    if vehiclemenu.checked then
      CircleButtonAsh1(vehiclemenu, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      setListVisibility(vehicleViews, View.VISIBLE)
      animateRadioButtons(vehicleViews)
    else
      CircleButtonAsh1(vehiclemenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
      setListVisibility(vehicleViews, View.GONE)
    end
  end

  camomenu.OnCheckedChangeListener = function()
    if camomenu.checked then
      CircleButtonAsh1(camomenu, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      setListVisibility(camoViews, View.VISIBLE)
      animateRadioButtons(camoViews)
    else
      CircleButtonAsh1(camomenu, 0xff5c0000, 0xff000000, 10, 0xff5c0000)
      setListVisibility(camoViews, View.GONE)
    end
  end

  pg.showPage(0)
  lobby.onClick = function() pg.showPage(0) end
  ingame.onClick = function() pg.showPage(1) end
  other.onClick = function() pg.showPage(2) end

  pg.addOnPageChangeListener({
    onPageScrolled = function() end,
    onPageSelected = function(pos)
      pcall(function()
        lobby.setColorFilter(0xFFFFFFFF)
        ingame.setColorFilter(0xFFFFFFFF)
        other.setColorFilter(0xFFFFFFFF)
        if pos == 0 then lobby.setColorFilter(0xff830000)
         elseif pos == 1 then ingame.setColorFilter(0xff830000)
         elseif pos == 2 then other.setColorFilter(0xff830000) end
      end)
    end,
  })

  local allChecks = { "antenna", "antenna1", "antenna2" }
  for i = 1, 16 do table.insert(allChecks, "dg" .. i) end
  
  for _, item in ipairs(csNames) do table.insert(allChecks, item.id) end
  for _, item in ipairs(mythicNames) do table.insert(allChecks, item.id) end
  for _, item in ipairs(legendaryNames) do table.insert(allChecks, item.id) end
  for _, item in ipairs(meleeNames) do table.insert(allChecks, item.id) end
  for _, item in ipairs(vehicleNames) do table.insert(allChecks, item.id) end
  for _, item in ipairs(camoNames) do table.insert(allChecks, item.id) end
  
  for _, id in ipairs(allChecks) do
    pcall(function()
      _G[id].ButtonDrawable.setColorFilter(PorterDuffColorFilter(0xff830000, PorterDuff.Mode.SRC_ATOP))
    end)
  end

  logo.OnCheckedChangeListener = function()
    if logo.checked then
      antihook() 
      CircleButtonAsh1(logo, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      cppPatch("bypass1", "10001")
      idkcstmToast("Bypass Logo Activated")
    end
  end

  clearlogs.OnCheckedChangeListener = function()
    if clearlogs.checked then
      local logPaths = {
        "/data/data/com.garena.game.codm/app_bugly",
        "/data/data/com.garena.game.codm/app_crashrecord",
        "/data/data/com.garena.game.codm/app_textures",
        "/data/data/com.garena.game.codm/app_webview",
        "/data/data/com.garena.game.codm/cache",
        "/data/data/com.garena.game.codm/code_cache",
        "/data/data/com.garena.game.codm/databases",
        "/data/data/com.garena.game.codm/files/AFRequestCache",
        "/data/data/com.garena.game.codm/files/com.gcloudsdk.gcloud.gvoice",
        "/data/data/com.garena.game.codm/files/facebook_ml",
        "/data/data/com.garena.game.codm/files/*.dat",
        "/data/data/com.garena.game.codm/files/itop_login.txt",
        "/data/data/com.garena.game.codm/files/tpnlcache.data",
        "/data/data/com.garena.game.codm/no_backup",
        "/data/data/com.garena.game.codm/oat",
        "/storage/emulated/0/Android/data/com.garena.game.codm/cache",
        "/storage/emulated/0/Android/data/com.garena.game.codm/files/ChatCache",
        "/storage/emulated/0/Android/data/com.garena.game.codm/files/Apollo/*",
        "/storage/emulated/0/Android/data/com.garena.game.codm/files/TGPA",
        "/storage/emulated/0/Android/data/com.garena.game.codm/files/VoiceCache",
        "/data/data/com.garena.game.codm/app_crashrecord/",
        "/data/data/com.garena.game.codm/files/tss_tmp/",
        "/data/data/com.garena.game.codm/app_crashrecord/1004",
        "/data/data/com.garena.game.codm/files/tss_tmp/codm_4_2_39.dat",
        "/data/data/com.garena.game.codm/files/tss_tmp/comm.dat",
        "/data/data/com.garena.game.codm/files/tss_tmp/config2.xml.aac30393",
        "/data/data/com.garena.game.codm/files/tss_tmp/config3.xml",
        "/data/data/com.garena.game.codm/files/tss_tmp/mn_cache.dat",
        "/data/data/com.garena.game.codm/files/tss_tmp/mrpcs_a.data",
        "/data/data/com.garena.game.codm/files/tss_tmp/shellcode_1021",
        "/data/data/com.garena.game.codm/files/tss_tmp/tdm_cache.dat",
        "/data/data/com.garena.game.codm/files/tss_tmp/tss_cef.dat",
        "/data/data/com.garena.game.codm/files/tss_tmp/tss_emu_c2.dat",
        "/data/data/com.garena.game.codm/files/tss_tmp/tss_lcp.dat",
        "/data/data/com.garena.game.codm/files/tss_tmp/tss_r_record.dat",
        "/data/data/com.garena.game.codm/files/tss_tmp/tss.ano2.dat",
        "/data/data/com.garena.game.codm/files/tss_tmp/tssmua.zip",
        "/data/data/com.garena.game.codm/files/tss_tmp/tssmua.zip/data",
        "/data/data/com.garena.game.codm/files/tss_tmp/tssmua.zip/data2",
        "/storage/emulated/0/MidasOversea",
        "/storage/emulated/0/tencent",
      }
      for _ = 1, 2 do
        for _, path in ipairs(logPaths) do
          os.remove(path)
        end
      end
      idkcstmToast("Clear Logs Activated")
      CircleButtonAsh1(clearlogs, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
    end
  end

  fps.OnCheckedChangeListener = function()
    if fps.checked then
      CircleButtonAsh1(fps, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      Mem.MemoryPatch("libunity.so", 0xa04f104, "h20 00 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa04f1d8, "h20 00 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa04f7d4, "h20 00 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa03f30c, "h20 00 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa03f5a4, "h00 24 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa03f5ac, "h00 24 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa049c4c, "h20 00 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa03e3e8, "h20 00 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa051778, "h20 00 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa03df88, "h20 00 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa03df98, "h00 24 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa0517d8, "h00 24 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa03f394, "h00 24 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa03fa14, "h00 24 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xa03f448, "hC0 00 80 D2 C0 03 5F D6")
      idkcstmToast("Smooth Fps Activated")
    end
  end

  skip.OnCheckedChangeListener = function()
    if skip.checked then
      CircleButtonAsh1(skip, 0xff00ff00, 0xff000000, 10, 0xff00ff00)
      Mem.MemoryPatch("libunity.so", 0xb7c2494, "h00 00 80 D2 C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0xae0d674, "h20 00 80 D2 C0 03 5F D6")
      idkcstmToast("Skip Tutorial Activated")
    end
  end

  local aimValue = 0
  aimbot_seekbar.setOnSeekBarChangeListener({
    onProgressChanged = function(seekBar, progress, fromUser)
      aimValue = progress
      aimbot_text.setText("Adjustable Aim (" .. aimValue .. "%)")
    end,
    onStartTrackingTouch = function() end,
    onStopTrackingTouch = function(seekBar)
      local hexValue = floatToHexLE(aimValue * 1)
      Mem.MemoryPatch("libunity.so", 0x4f478d0, "h40 00 00 1C")
      Mem.MemoryPatch("libunity.so", 0x4f478d4, "hC0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0x4f478d8, hexValue)
      Mem.MemoryPatch("libunity.so", 0x6a92d3c, "h40 00 00 1C")
      Mem.MemoryPatch("libunity.so", 0x6a92d40, "hC0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0x6a92d44, hexValue)
    end,
  })

  local snowValue = 0
  snowboard_seekbar.setOnSeekBarChangeListener({
    onProgressChanged = function(seekBar, progress, fromUser)
      snowValue = progress
      snowboard_text.setText("Adjustable SnowB. (" .. snowValue .. "%)")
    end,
    onStartTrackingTouch = function() end,
    onStopTrackingTouch = function(seekBar)
      local hexValue = floatToHexLE(snowValue * 1)
      Mem.MemoryPatch("libunity.so", 0x500dca0, "h40 00 00 1C C0 03 5F D6")
      Mem.MemoryPatch("libunity.so", 0x500dca4, "hC0 03 5F D6 00 00 7A 44")
      Mem.MemoryPatch("libunity.so", 0x500dca8, hexValue)
    end,
  })

  local dgPatches = {
    { name = "Wallhack", patches = {
        { "libunity.so", 0x51ec608, "h1F 20 03 D5", "h80 00 00 36" } } },
    { name = "Kinetic Weapon", patches = {
        { "libunity.so", 0x4f98bac, "h20 00 80 D2 C0 03 5F D6", "h01 00 A0 E3 1E FF 2F E1" } } },
    { name = "No Recoil", patches = {
        { "libunity.so", 0xc733be4, "h20 4C 40 BC C0 03 5F D6", "hE8 0F 1D FC F4 4F 01 A9" } } },
    { name = "No Spread", patches = {
        { "libunity.so", 0xc73224c, "h00 00 80 D2 C0 03 5F D6", "hE8 0F 1D FC F4 4F 01 A9" } } },
    { name = "Esp Name Br", patches = {
        { "libunity.so", 0x87b13ac, "h20 00 80 D2 C0 03 5F D6", "h20 00 80 D2 C0 03 5F D6" },
        { "libunity.so", 0x87b15bc, "h20 00 80 D2 C0 03 5F D6", "h20 00 80 D2 C0 03 5F D6" } } },
    { name = "Hitbox Body", patches = {
        { "libunity.so", 0xbd1d5c4, "h20 00 80 52 C0 03 5F D6", "h20 00 80 52 C0 03 5F D6" },
        { "libunity.so", 0xc72b1ec, "h20 00 80 52 C0 03 5F D6", "h20 00 80 52 C0 03 5F D6" },
        { "libunity.so", 0xc72bafc, "h20 00 80 52 C0 03 5F D6", "h20 00 80 52 C0 03 5F D6" } } },
    { name = "No Parachute", patches = {
        { "libunity.so", 0x614df2c, "h00 10 20 1E C0 03 5F D6", "hFF 43 02 D1 E9 23 02 6D" } } },
    { name = "Fast Scope", patches = {
        { "libunity.so", 0x4f138bc, "h00 2C 40 BC C0 03 5F D6", "hE8 0F 1D FC F4 4F 01 A9" } } },
    { name = "Fast Switch", patches = {
        { "libunity.so", 0x4ed67ac, "h00 2C 40 BC C0 03 5F D6", "h00 2C 40 BC C0 03 5F D6" } } },
    { name = "No Reload", patches = {
        { "libunity.so", 0xbd15568, "h40 00 00 1C C0 03 5F D6", "hEA 0F 1C FC E9 A3 00 6D" },
        { "libunity.so", 0x4ed5a5c, "h40 00 00 1C C0 03 5F D6", "hE8 0F 1D FC F4 4F 01 A9" } } },
    { name = "Pump Boost", patches = {
        { "libunity.so", 0x6ef5244, "h20 00 80 D2 C0 03 5F D6", "hE8 0F 1D FC F4 4F 01 A9" } } },
    { name = "Advance Uav", patches = {
        { "libunity.so", 0x67121c8, "h20 00 80 D2 C0 03 5F D6", "hFF 03 02 D1 F8 5F 04 A9" } } },
    { name = "Long Slide", patches = {
        { "libunity.so", 0xa40eae4, "h00 00 80 D2 C0 03 5F D6", "h00 00 80 D2 C0 03 5F D6" } } },
    { name = "No FlashBang", patches = {
        { "libunity.so", 0x8cfabc0, "h40 00 00 1C C0 03 5F D6", "h40 00 00 1C C0 03 5F D6" } } },
    { name = "Walk Underwater", patches = {
        { "libunity.so", 0x4fb7c5c, "h20 00 80 D2 C0 03 5F D6", "hFF C3 01 D1 EA 1B 00 FD" },
        { "libunity.so", 0x4fd558c, "h20 00 80 D2 C0 03 5F D6", "hE0 03 1F 2A C0 03 5F D6" },
        { "libunity.so", 0x521d60c, "h20 00 80 D2 C0 03 5F D6", "hFD 7B BF A9 FD 03 00 91" } } },
    { name = "Unlock BluePrint", patches = {
        { "libunity.so", 0x79b2828, "h20 00 80 D2 C0 03 5F D6", "h20 00 80 D2 C0 03 5F D6" },
        { "libunity.so", 0xa8b46ec, "h20 00 80 D2 C0 03 5F D6", "h20 00 80 D2 C0 03 5F D6" } } },
  }

  for i, cfg in ipairs(dgPatches) do
    local cb = _G["dg" .. i]
    cb.OnCheckedChangeListener = function()
      if cb.checked then
        antihook()
        for _, p in ipairs(cfg.patches) do
          Mem.MemoryPatch(p[1], p[2], p[3])
        end
        idkcstmToast(cfg.name .. " Activated")
       else
        for _, p in ipairs(cfg.patches) do
          Mem.MemoryPatch(p[1], p[2], p[4])
        end
        idkcstmToast(cfg.name .. " Deactivated")
      end
    end
  end

  local characterSkinMap = {
    kuiji = "90089",
    shepherd = "90099",
    sophia = "90100",
    spectre = "90098",
    templar = "90097",
    siren = "90096",
    ghost = "90095",
    lazarus = "90094",
    ryu = "90093",
    akuma = "90092",
    chunli = "90091",
    cammy = "90090",
    blacknoir = "90088",
    starlight = "90087",
    homelander = "90086"
  }
  
  local characterWowMap = {
    spectre = "90009",
    templar = "90008",
    siren = "90007",
    ghost = "90006",
    sophia = "90005",
    shepherd = "90004",
    kuiji = "89998",
    lazarus = "90003",
    ryu = "90002",
    akuma = "90001",
    chunli = "90000",
    cammy = "89999",
    blacknoir = "89997",
    starlight = "89996",
    homelander = "89995"
  }
  
  for id, code in pairs(characterSkinMap) do
    local btn = _G[id]
    if btn then
      btn.OnCheckedChangeListener = function()
        if btn.checked then
          for otherId, _ in pairs(characterSkinMap) do
            if otherId ~= id then
              pcall(function() _G[otherId].setChecked(false) end)
            end
          end
          btn.setChecked(true)
          cppPatch("charss", code)
          if characterWowMap[id] then
            cppPatch("wow2", characterWowMap[id])
          end
          idkcstmToast("Character Skin Activated")
        end
      end
    end
  end

  local mythicSkinMap = {
    ak117lava = "90999",
    ak117 = "90998",
    bp50 = "90997",
    ffar = "90996",
    grau = "90995",
    krig6 = "90994",
    type19 = "90993",
    oden = "90992",
    ak47 = "90991",
    mg42 = "90990",
    xm4 = "90989",
    lw3 = "90988",
    vmp = "90987",
    uss9 = "90986",
    dlq33 = "90985",
    kilo = "90984",
    switchh = "90983",
    jak12 = "90982",
    cx9 = "90981",
    qq9 = "90980",
    m13 = "90979",
    fennec = "90978",
    rytec = "90977",
    holger = "90976",
    em2 = "90975",
    cbr = "90974",
    asval = "90973",
    peace = "90972",
    ram7 = "90971",
    type25 = "90970",
    so14 = "90969",
    lachmann = "90968",
    dp27 = "90967"
  }
  
  local mythicWowMap = {
    ak117lava = "700009",
    ak117 = "700008",
    bp50 = "700007",
    ffar = "700006",
    grau = "700005",
    krig6 = "700004",
    type19 = "700003",
    oden = "700002",
    ak47 = "699999",
    mg42 = "699996",
    xm4 = "699987",
    lw3 = "699986",
    vmp = "699985",
    uss9 = "699984",
    dlq33 = "699983",
    kilo = "699982",
    switchh = "699981",
    jak12 = "699980",
    cx9 = "699978",
    qq9 = "699977",
    m13 = "699976",
    fennec = "699973",
    rytec = "699970",
    holger = "699969",
    em2 = "699968",
    cbr = "699966",
    asval = "699965",
    peace = "699964",
    ram7 = "699963",
    type25 = "699960",
    so14 = "699958",
    lachmann = "699957",
    dp27 = "699956"
  }
  
  for id, code in pairs(mythicSkinMap) do
    local btn = _G[id]
    if btn then
      btn.OnCheckedChangeListener = function()
        if btn.checked then
          for otherId, _ in pairs(mythicSkinMap) do
            if otherId ~= id then
              pcall(function() _G[otherId].setChecked(false) end)
            end
          end
          btn.setChecked(true)
          cppPatch("pogiako2", code)
          if mythicWowMap[id] then
            cppPatch("wow", mythicWowMap[id])
          end
          idkcstmToast("Mythic Weapon Activated")
        end
      end
    end
  end

  local legendarySkinMap = {
    krm = "100029",
    by15 = "100028",
    locus = "100027",
    dlqholi = "100026",
    hssong = "100025",
    locusdemon = "100024",
    locusmetal = "100023",
    locusneptune = "100022",
    locuscomet = "100021",
    locusicy = "100020",
    krmred = "100019",
    krmload = "100018",
    dlqzealot = "100017"
  }
  
  local legendaryWowMap = {
    krm = "700001",
    by15 = "700000",
    locus = "699998",
    dlqholi = "699997",
    hssong = "699995",
    locusdemon = "699994",
    locusmetal = "699993",
    locusneptune = "699992",
    locuscomet = "699991",
    locusicy = "699990",
    krmred = "699989",
    krmload = "699988",
    dlqzealot = "699979"
  }
  
  for id, code in pairs(legendarySkinMap) do
    local btn = _G[id]
    if btn then
      btn.OnCheckedChangeListener = function()
        if btn.checked then
          for otherId, _ in pairs(legendarySkinMap) do
            if otherId ~= id then
              pcall(function() _G[otherId].setChecked(false) end)
            end
          end
          btn.setChecked(true)
          cppPatch("pogiako", code)
          if legendaryWowMap[id] then
            cppPatch("wow", legendaryWowMap[id])
          end
          idkcstmToast("Legendary Weapon Activated")
        end
      end
    end
  end

  local meleeSkinMap = {
    tang = "1000",
    longq = "999",
    spear = "998",
    scissors = "997",
    tomahawk = "996",
    saber = "995",
    fiery = "994"
  }
  
  for id, code in pairs(meleeSkinMap) do
    local btn = _G[id]
    if btn then
      btn.OnCheckedChangeListener = function()
        if btn.checked then
          for otherId, _ in pairs(meleeSkinMap) do
            if otherId ~= id then
              pcall(function() _G[otherId].setChecked(false) end)
            end
          end
          btn.setChecked(true)
          cppPatch("fuckmellee", code)
          idkcstmToast("Melee Weapon Activated")
        end
      end
    end
  end

  local vehicleSkinMap = {
    sand = "28193",
    blaze = "18329",
    mech = "19482",
    parachute = "28371"
  }
  
  for id, code in pairs(vehicleSkinMap) do
    local btn = _G[id]
    if btn then
      btn.OnCheckedChangeListener = function()
        if btn.checked then
          for otherId, _ in pairs(vehicleSkinMap) do
            if otherId ~= id then
              pcall(function() _G[otherId].setChecked(false) end)
            end
          end
          btn.setChecked(true)
          cppPatch("gayontopp", code)
          idkcstmToast("Vehicle/Equipment Skin Activated")
        end
      end
    end
  end

  local camoSkinMap = {
    diamond = "80029",
    redsprite = "80028",
    glacial = "80027",
    aether = "80026",
    alchemy = "80025",
    fluorescent = "80024",
    lunar = "80023",
    sunken = "80022",
    ocean = "80021",
    psychic = "80020",
    shimmer = "80019",
    astronomy = "80018",
    powernova = "80017",
    makingwaves = "80016",
    blistering = "80015",
    emerald = "80014",
    assault = "80013",
    scorch = "80012"
  }
  
  for id, code in pairs(camoSkinMap) do
    local btn = _G[id]
    if btn then
      btn.OnCheckedChangeListener = function()
        if btn.checked then
          for otherId, _ in pairs(camoSkinMap) do
            if otherId ~= id then
              pcall(function() _G[otherId].setChecked(false) end)
            end
          end
          btn.setChecked(true)
          cppPatch("fretzHAHA", code)
          idkcstmToast("Camo Activated")
        end
      end
    end
  end

  antenna.OnCheckedChangeListener = function()
    if antenna.checked then
      cppPatch("thumbnail", "0900")
      idkcstmToast("Red Antenna Activated")
    end
  end

  antenna1.OnCheckedChangeListener = function()
    if antenna1.checked then
      cppPatch("thumbnail", "0903")
      idkcstmToast("Cyan Antenna Activated")
    end
  end

  antenna2.OnCheckedChangeListener = function()
    if antenna2.checked then
      cppPatch("thumbnail", "0904")
      idkcstmToast("Black Antenna Activated")
    end
  end

end

-- ============================================================
-- ENTRY POINT
-- ============================================================

if not Settings.canDrawOverlays(activity) then
  local intent = Intent("android.settings.action.MANAGE_OVERLAY_PERMISSION")
  intent.setData(Uri.parse("package:" .. this.getPackageName()))
  this.startActivity(intent)
end

-- Start with login UI
LoginUI()