# Fashionable Watch – PIC24

**Intro to Embedded Systems – Final Project**

This project implements a **fashionable digital/analog watch** on the **PIC24 Curiosity board** using an OLED display and an accelerometer.

The watch provides multiple display modes, a configurable alarm system, and a menu-based interface that allows the user to configure the watch using hardware buttons and a potentiometer.

The system maintains accurate time using a **hardware timer interrupt that updates the clock every second**.

---

# Hardware Components

The system uses the following hardware peripherals:

* **PIC24 Curiosity Board**
* **OLED Display (96×96 pixels)**
* **Accelerometer**
* **Potentiometer**
* **Push Buttons**

  * S1
  * S2
* **LED indicators**

---

# System Features

The watch provides the following functionality:

* Digital clock display
* Analog clock display with **three visual themes**
* Date display (day / month)
* Alarm system with visual alert
* Menu-based configuration
* User interaction using buttons, potentiometer, and accelerometer

The display is updated **once per second** to maintain accurate time.

---

# Clock Display Modes

## Digital Mode

The digital display presents the following information:

```
HH:MM:SS
DD/MM
Alarm icon (if enabled)
AM / PM indicator
```

Supported formats:

* **12-hour format (AM/PM)**
* **24-hour format**

The digital clock occupies most of the display area to ensure good readability.

---

## Analog Mode

The analog display renders a graphical clock with:

* Hour hand
* Minute hand
* Second hand
* Hour tick marks

### Hand Properties

| Hand   | Length | Thickness |
| ------ | ------ | --------- |
| Hour   | Short  | Thick     |
| Minute | Medium | Medium    |
| Second | Long   | Thin      |

### Analog Themes

Three visual themes are available:

* **Classic**
* **Cyberpunk**
* **Rainbow**

The theme is selected through a submenu in the display settings.

### Implementation

To avoid floating-point operations, the analog clock is implemented using:

* **1 center point**
* **60 precomputed edge points**

These points represent the positions of the 60 seconds around the clock face and allow the system to draw both the **tick marks and clock hands** efficiently.

---

# Menu System

The watch configuration is performed using a **menu interface**.

## Entering the Menu

Press **S1 for more than 2 seconds**.

## Exiting the Menu

The user can exit the menu by **flipping the watch so the OLED faces downward**.
The accelerometer detects this orientation change.

---

# Menu Navigation

| Control       | Function               |
| ------------- | ---------------------- |
| S1            | Move to next menu item |
| S2            | Select / confirm       |
| Potentiometer | Change values          |
| LED1          | Feedback for S1        |
| LED2          | Feedback for S2        |

Menu navigation is **cyclic**, meaning that the selection wraps around when reaching the end of the list.

While inside the menu, the **current time continues to run** and is displayed as a small digital clock in the upper corner of the screen.

---

# Menu Options

The menu allows the user to configure the following settings:

## Display Mode

Choose between:

* Digital display
* Analog display

When Analog is selected, a **submenu** allows selecting one of the three themes.

---

## Time Format

Select the time format:

* 12-hour format
* 24-hour format

---

## Set Time

Allows adjusting:

* Hour
* Minute
* Second

---

## Set Date

Allows adjusting day and month.
* February always has **28 days**
* The year is not stored

---


Alarm Toggle Logic
Auto-On: Setting the hour/minute automatically toggles the alarm.

Manual Toggle: Pressing S2 while the "OFF" text is highlighted disables the current alarm.

Visual Indicator: if an alarm was set : Alarm icon is visible in the top right corner.

OFF: Alarm icon is disabled.

## Disable Alarm

This option disables the alarm and removes the alarm icon from the display.

---

# Alarm Behavior

When the configured alarm time is reached:

* The display begins **flashing in inverse mode** to attract attention.

The alarm can be stopped by:

* Pressing any button
* Flipping the watch

If the user does not stop the alarm manually, it **automatically stops after 20 seconds**.

---

# Time Engine

Accurate timekeeping is implemented using a **hardware timer interrupt**.

Timer1 generates a **1-second interrupt**, which performs the following operations:

1. Updates the current time
2. Updates the date when midnight is reached
3. Triggers the display refresh

Using a hardware interrupt ensures that the clock remains accurate even when the system is processing menu input.

---

# Display Refresh

The display updates once per second.

For the analog clock:

1. The previous positions of the clock hands are erased.
2. The new hand positions are drawn.
3. Any tick marks that were overwritten are restored.

This approach minimizes flicker while keeping the display consistent.

---

# Running the Project

To run the watch:

1. Open the project in **MPLAB X**
2. Select the correct **PIC24 device**
3. Build the project using the **XC16 compiler**
4. Upload the firmware to the **PIC24 Curiosity board**

The watch starts automatically after programming.

---

# Example Usage

## Change Display Mode

```
Hold S1 for 2 seconds to enter the menu
Navigate to "Display Mode"
Press S2 to enter
Select "Analog"
Press S2 to enter the theme submenu
Choose a theme (Classic / Cyberpunk / Rainbow)
Press S2 to confirm
Flip the watch to see the changed Display Mode.
```

---


## Set Alarm
* **Enter Menu:** Hold **S1**.
* **Navigate:** Click **S1** to find "Alarm".
* **Enter:** Press **S2**.
* **Adjust Time:** * Use **Potentiometer** for Hour $\rightarrow$ Press **S2** to confirm.
    * Use **Potentiometer** for Minute $\rightarrow$ Press **S2** to confirm.
* **Auto-On:** The alarm is now **enabled**, and the icon appears.
* **Toggle (Optional):** Press **S2** on the "OFF" toggle on-screen to disable it manually.

---

## Disable Alarm
* **Action:** Select the "Disable Alarm" option in the menu.
* **Result:** Turns the alarm **OFF** and removes the icon from the display.

